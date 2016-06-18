#ifdef DEBUG
#include <cassert>
#endif

#include "rule_context.h"


RuleContext::RuleContext() :
        symbol_table(),
        lexemes(new std::stack<std::string>[SyntaxSymbol::NUM_OF_SYMBOLS]),
        attributes(new std::stack<SymbolAttributes>[SyntaxSymbol::NUM_OF_SYMBOLS]) {
}


RuleContext::~RuleContext() {
#ifdef DEBUG
    for (std::size_t i = 0; i < SyntaxSymbol::NUM_OF_SYMBOLS; ++i) {
        assert(lexemes[i].empty());
        assert(attributes[i].empty());
    }
#endif
    delete[] lexemes;
    delete[] attributes;
}


void RuleContext::add_symbol(SyntaxSymbol symbol) {
    attributes[symbol].push(SymbolAttributes{});
}


void RuleContext::set_lexeme(SyntaxSymbol symbol, std::string lex) {
#ifdef DEBUG
    assert(not attributes[symbol].empty());
#endif
    lexemes[symbol].push(lex);
}


void RuleContext::remove_symbol(SyntaxSymbol symbol) {
    if (symbol.variable_lexeme()) {
#ifdef DEBUG
        assert(not lexemes[symbol].empty());
#endif
        lexemes[symbol].pop();
    }
#ifdef DEBUG
    assert(not attributes[symbol].empty());
#endif
    attributes[symbol].pop();
}


SymbolAttributes &RuleContext::get_attributes(SyntaxSymbol symbol) const {
#ifdef DEBUG
    assert(not attributes[symbol].empty());
#endif
    return attributes[symbol].top();
}
