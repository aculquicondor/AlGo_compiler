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
            if (symbol.size())
                production.push_back(symbol);
        }
        productions.push_back(production);
        getline(productions_file, line);
    }

    std::ifstream syntactic_table_file("syntactic_table.csv");
    getline(syntactic_table_file, line);
    getline(syntactic_table_file, line);
    while (line.size()) {
        std::stringstream ss(line);
        std::map<Token, int> row;
        getline(ss, symbol, ',');
        for (int token = 1; not ss.eof(); ++token) {
            getline(ss, symbol, ',');
            if (symbol.size()) {
                std::stringstream(symbol) >> row[token];
                --row[token];
            }
        }
        syntactic_table.push_back(row);
        getline(syntactic_table_file, line);
    }
}


std::vector<SyntaxSymbol> SyntaxAnalyzer::_get_production(SyntaxSymbol symbol, LexicalDescriptor descriptor) {
    auto &row = syntactic_table[symbol - SyntaxSymbol::FIRST_NT];
    auto it = row.find(descriptor.get_token());
    if (it == row.end())
        throw SyntaxError(descriptor);
    return productions[it->second];
}


SyntaxError::SyntaxError(const LexicalDescriptor &lex) {
    std::stringstream ss;
    ss << "Unexpected token " << lex.get_token() <<
            " <" << lex.get_lexeme() << "> at line " << lex.get_line_no() << ".";
    msg = ss.str();
}


SyntaxError::~SyntaxError() { }
