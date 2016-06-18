#ifndef ALGO_ANALYZER_H
#define ALGO_ANALYZER_H

#include <fstream>
#include <iostream>
#include <stack>

#include "lexical_analyzer.h"
#include "semantic_rules.h"


struct ProductionItem {
    int value;
    enum Type {
        SYMBOL,
        RULE,
        PRODUCTION_END
    } type;

    ProductionItem(int value, Type type=SYMBOL) : value(value), type(type) {}
};

class Analyzer {
public:
    Analyzer(LexicalAnalyzer *lexical_analyzer);

    bool analyze();

private:
    ProductionItem _parse_production_item(std::string item);
    int _get_production(SyntaxSymbol symbol, LexicalDescriptor descriptor);
    bool _has_production(SyntaxSymbol symbol, LexicalDescriptor descriptor);
    void _clean_production(int production_id);

    LexicalAnalyzer *lexical_analyzer;
    std::vector<std::vector<ProductionItem>> productions;
    std::vector<std::map<Token, int>> syntactic_table;
    RuleContext context;
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

#endif //ALGO_ANALYZER_H
