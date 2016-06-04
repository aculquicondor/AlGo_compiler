#ifdef DEBUG
#include <cassert>
#endif

#include "syntax_analyzer.h"


SyntaxAnalyzer::SyntaxAnalyzer(LexicalAnalyzer *lexical_analyzer) : lexical_analyzer(lexical_analyzer) {
    std::string line, symbol;

    std::ifstream productions_file("productions.csv");
    getline(productions_file, line);
    while (line.size()) {
        std::stringstream ss(line);
        std::vector<SyntaxSymbol> production;
        getline(ss, symbol, ',');
        while (not ss.eof()) {
            getline(ss, symbol, ',');
            if (symbol.size()) {
                production.push_back(symbol);
#ifdef DEBUG
                assert(production.back() != SyntaxSymbol::NONE);
#endif
            }
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
        assert(SyntaxSymbol(symbol) == row_count + SyntaxSymbol::FIRST_NT);
#endif
        for (int token = 0; not ss.eof(); ++token) {
            getline(ss, symbol, ',');
            if (symbol.size()) {
                std::stringstream(symbol) >> row[token];
                --row[token];
#ifdef DEBUG
                assert(row[token] < productions.size());
                assert(token < SyntaxSymbol::FIRST_NT);
#endif
            }
        }
        syntactic_table.push_back(row);
        getline(syntactic_table_file, line);
    }
}


bool SyntaxAnalyzer::analyze() {
    std::stack<SyntaxSymbol> stack;
    stack.push(SyntaxSymbol::NONE);
    stack.push(SyntaxSymbol::PACKAGE);

    LexicalDescriptor descriptor = lexical_analyzer->next();
    bool found_errors = false;
    do {
        bool need_next_token = false;
        try {

            if (stack.top().is_terminal()) {
                if (stack.top() == descriptor.get_token()) {
                    stack.pop();
                    need_next_token = true;

                    if (descriptor.get_token() == Token::NONE)
                        break;
                } else {
                    SyntaxSymbol expected = stack.top();
                    stack.pop();
                    throw SyntaxError(descriptor, expected);
                }
            } else {
                auto production = _get_production(stack.top(), descriptor);
                stack.pop();
                for (auto it = production.rbegin(); it != production.rend(); ++it)
                    stack.push(*it);
            }

        } catch (SyntaxError &err) {
            std::cerr << err.what() << std::endl;
            found_errors = true;

            if (descriptor.get_token() == Token::NONE)
                break;

            if (not err.get_expected()) {
                while (not stack.empty() and not _has_production(stack.top(), descriptor))
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
    } while (not stack.empty());

    return not found_errors and stack.empty();
}


std::vector<SyntaxSymbol> SyntaxAnalyzer::_get_production(SyntaxSymbol symbol, LexicalDescriptor descriptor) {
    auto &row = syntactic_table[symbol - SyntaxSymbol::FIRST_NT];
    auto it = row.find(descriptor.get_token());
    if (it == row.end())
        throw SyntaxError(descriptor);
    return productions[it->second];
}


bool SyntaxAnalyzer::_has_production(SyntaxSymbol symbol, LexicalDescriptor descriptor) {
    if (symbol.is_terminal())
        return symbol == descriptor.get_token();
    auto &row = syntactic_table[symbol - SyntaxSymbol::FIRST_NT];
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
