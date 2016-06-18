#ifndef ALGO_RULE_CONTEXT_H
#define ALGO_RULE_CONTEXT_H

#include "definitions.h"
#include "symbol_table.h"


struct SymbolAttributes {

};


class RuleContext {
public:
    explicit RuleContext();

    virtual ~RuleContext();

    void add_symbol(SyntaxSymbol symbol);

    void set_lexeme(SyntaxSymbol symbol, std::string lex);

    void remove_symbol(SyntaxSymbol symbol);

    SymbolAttributes &get_attributes(SyntaxSymbol symbol) const;

    std::string get_lex(SyntaxSymbol symbol) const;

    bool get_bool_value(SyntaxSymbol symbol) const;

    long get_int_value(SyntaxSymbol symbol) const;

    double get_float_value(SyntaxSymbol symbol) const;

    char get_rune_value(SyntaxSymbol symbol) const;

    std::string get_string_value(SyntaxSymbol symbol) const;

    SymbolTable &get_symbol_table() {
        return symbol_table;
    }

private:
    SymbolTable symbol_table;
    std::stack<std::string> *lexemes;
    std::stack<SymbolAttributes> *attributes;
};

#endif //ALGO_RULE_CONTEXT_H
