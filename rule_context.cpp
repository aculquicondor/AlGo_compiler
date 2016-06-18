#ifdef DEBUG
#include <cassert>
#endif

#include "rule_context.h"


RuleContext::RuleContext() :
        symbol_table(),
        lexemes(new std::stack<std::string>[Token::NUM_OF_TOKENS]),
        attributes(new std::stack<SymbolAttributes>[Token::NUM_OF_TOKENS]) {
}


RuleContext::~RuleContext() {
#ifdef DEBUG
    for (std::size_t i = 0; i < Token::NUM_OF_TOKENS; ++i) {
        assert(lexemes[i].empty());
        assert(attributes[i].empty());
    }
#endif
    delete[] lexemes;
    delete[] attributes;
}


void RuleContext::add_symbol(Token token) {
    attributes[token].push(SymbolAttributes{});
}


void RuleContext::set_lexeme(Token token, std::string lex) {
#ifdef DEBUG
    assert(not attributes[token].empty());
#endif
    lexemes[token].push(lex);
}


void RuleContext::remove_symbol(Token token) {
    if (token.variable_lexeme()) {
#ifdef DEBUG
        assert(not lexemes[token].empty());
#endif
        lexemes[token].pop();
    }
#ifdef DEBUG
    assert(not attributes[token].empty());
#endif
    attributes[token].pop();
}


SymbolAttributes &RuleContext::get_attributes(Token token) const {
#ifdef DEBUG
    assert(not attributes[token].empty());
#endif
    return attributes[token].top();
}


std::string RuleContext::get_lexeme(Token token) const {
#ifdef DEBUG
    assert(not lexemes[token].empty());
#endif
    return lexemes[token].top();
}

bool RuleContext::get_bool_value(Token token) const {
    if (token == Token::TRUE)
        return true;
    if (token == Token::FALSE)
        return false;
#ifdef DEBUG
    assert(false);
#endif
}
