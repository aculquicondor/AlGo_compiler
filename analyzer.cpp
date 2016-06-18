#ifdef DEBUG
#include <cassert>
#endif

#include "analyzer.h"


Analyzer::Analyzer(LexicalAnalyzer *lexical_analyzer) :
        lexical_analyzer(lexical_analyzer), context() {
    std::string line, symbol;

    std::ifstream productions_file("productions.csv");
    getline(productions_file, line);
    while (line.size()) {
        std::stringstream ss(line);
        std::vector<ProductionItem> production;
        getline(ss, symbol, ',');
        while (not ss.eof()) {
            getline(ss, symbol, ',');
            if (symbol.size())
                production.push_back(_parse_production_item(symbol));
        }
        productions.push_back(production);
        getline(productions_file, line);
    }

    std::ifstream syntactic_table_file("syntactic_table.csv");
    getline(syntactic_table_file, line);
    getline(syntactic_table_file, line);
    for (int row_count = 0; line.size(); ++row_count) {
        std::stringstream ss(line);
        std::map<Token, int> row;
        getline(ss, symbol, ',');
#ifdef DEBUG
        assert(SyntaxSymbol(symbol) == row_count + SyntaxSymbol::FIRST_NON_TERMINAL);
#endif
        for (int token = 0; not ss.eof(); ++token) {
            getline(ss, symbol, ',');
            if (symbol.size()) {
                std::stringstream(symbol) >> row[token];
                --row[token];
#ifdef DEBUG
                assert(row[token] < productions.size());
                assert(token < SyntaxSymbol::FIRST_NON_TERMINAL);
#endif
            }
        }
        syntactic_table.push_back(row);
        getline(syntactic_table_file, line);
    }
}


bool Analyzer::analyze() {
    std::stack<ProductionItem> stack;
    stack.push({SyntaxSymbol::NONE});
    stack.push({SyntaxSymbol::PACKAGE});

    LexicalDescriptor descriptor = lexical_analyzer->next();
    bool found_errors = false;
    do {
        if (stack.top().type == ProductionItem::SYMBOL) {
            SyntaxSymbol curr_symbol{stack.top().value};

            bool need_next_token = false;

            try {
                if (curr_symbol.is_terminal()) {
                    if (curr_symbol == descriptor.get_token()) {
                        stack.pop();

                        if (curr_symbol.variable_lexeme())
                            context.set_lexeme(curr_symbol, descriptor.get_lexeme());

                        need_next_token = true;

                        if (descriptor.get_token() == Token::NONE)
                            break;
                    } else {
                        stack.pop();
                        throw SyntaxError(descriptor, curr_symbol);
                    }
                } else {
                    int production_id = _get_production(curr_symbol, descriptor);
                    stack.pop();
                    stack.push({production_id, ProductionItem::PRODUCTION_END});
                    for (auto it = productions[production_id].rbegin();
                            it != productions[production_id].rend(); ++it) {
                        if (it->type == ProductionItem::SYMBOL)
                            context.add_symbol(it->value);
                        stack.push(*it);
                    }
                }

            } catch (SyntaxError &err) {
                std::cerr << err.what() << std::endl;
                found_errors = true;

                if (descriptor.get_token() == Token::NONE)
                    break;

                if (not err.get_expected()) {
                    while (not stack.empty() and not _has_production(curr_symbol, descriptor))
                        stack.pop();
                }
            }

            if (need_next_token) {
                while (true) {
                    try {
                        descriptor = lexical_analyzer->next();
                        break;
                    } catch (LexicalError &err) {
                        std::cerr << err.what() << std::endl;
                        found_errors = true;
                    }
                }
            }
        } else if (stack.top().type == ProductionItem::RULE) {
            // TODO: semantic rule execution
            stack.pop();
        } else {
            int production_id = stack.top().value;
            for (auto it = productions[production_id].rbegin(); it != productions[production_id].rend(); ++it)
                if (it->type == ProductionItem::SYMBOL)
                    context.remove_symbol(it->value);
            stack.pop();
        }
    } while (not stack.empty());

    return not found_errors and stack.empty();
}


ProductionItem Analyzer::_parse_production_item(std::string item) {
    if (item[0] >= '0' and item[0] <= '9') {
        std::stringstream ss(item);
        int value;
        ss >> value;
        return {value, ProductionItem::RULE};
    } else {
        SyntaxSymbol symbol(item);
#ifdef DEBUG
        assert(symbol != SyntaxSymbol::NONE);
#endif
        return {symbol, ProductionItem::SYMBOL};
    }
}


int Analyzer::_get_production(SyntaxSymbol symbol, LexicalDescriptor descriptor) {
    auto &row = syntactic_table[symbol - SyntaxSymbol::FIRST_NON_TERMINAL];
    auto it = row.find(descriptor.get_token());
    if (it == row.end())
        throw SyntaxError(descriptor);
    return it->second;
}


bool Analyzer::_has_production(SyntaxSymbol symbol, LexicalDescriptor descriptor) {
    if (symbol.is_terminal())
        return symbol == descriptor.get_token();
    auto &row = syntactic_table[symbol - SyntaxSymbol::FIRST_NON_TERMINAL];
    return row.find(descriptor.get_token()) != row.end();
}


SyntaxError::SyntaxError(const LexicalDescriptor &lex, Token expected) :
        descriptor(lex), expected(expected) {
    std::stringstream ss;
    ss << "Unexpected token " << lex.get_token() <<
            " <" << lex.get_lexeme() << "> at line " << lex.get_line_no() << ".";
    if (expected != Token::NONE) {
        ss << " Expected token " << expected << ".";
    }
    msg = ss.str();
}


SyntaxError::~SyntaxError() { }
