#ifndef ALGO_RULE_CONTEXT_H
#define ALGO_RULE_CONTEXT_H

#include "definitions.h"
#include "symbol_table.h"


struct SymbolAttributes : public SymbolTableRecord {
};


class RuleContext {
public:
    explicit RuleContext();

    virtual ~RuleContext();

    void add_symbol(Token token);

    void set_lexeme(Token token, std::string lex);

    void remove_symbol(Token token);

    SymbolAttributes &get_attributes(Token token) const;

    std::string get_lexeme(Token token) const;

    bool get_bool_value(Token token) const;

    long get_int_value(Token token) const;

    double get_float_value(Token token) const;

    char get_rune_value(Token token) const;

    std::string get_string_value(Token token) const;

    SymbolTable &get_symbol_table() {
        return symbol_table;
    }

private:
    SymbolTable symbol_table;
    std::stack<std::string> *lexemes;
    std::stack<SymbolAttributes> *attributes;
};

#endif //ALGO_RULE_CONTEXT_H