#ifndef ALGO_SYNTAX_ANALYZER_H
#define ALGO_SYNTAX_ANALYZER_H

#include <fstream>
#include <iostream>
#include <stack>

#include "lexical_analyzer.h"

class SyntaxAnalyzer {
public:
    SyntaxAnalyzer(LexicalAnalyzer *lexical_analyzer);

    bool analyze();

private:
    std::vector<SyntaxSymbol> _get_production(SyntaxSymbol symbol, LexicalDescriptor descriptor);
    bool _has_production(SyntaxSymbol symbol, LexicalDescriptor descriptor);

    LexicalAnalyzer *lexical_analyzer;
    std::vector<std::vector<SyntaxSymbol>> productions;
    std::vector<std::map<Token, int>> syntactic_table;
};


class SyntaxError : public std::exception {
public:
    SyntaxError(const LexicalDescriptor &lex, Token expected = Token::NONE);

    virtual ~SyntaxError();

    virtual const char *what() const throw() {
        return msg.c_str();
    }

    const std::string &get_lexeme() const {
        return descriptor.get_lexeme();
    }

    std::size_t get_line_no() const {
        return descriptor.get_line_no();
    }

    SyntaxSymbol get_expected() const {
        return expected;
    }

private:
    LexicalDescriptor descriptor;
    SyntaxSymbol expected;
    std::string msg;
};

#endif //ALGO_SYNTAX_ANALYZER_H
