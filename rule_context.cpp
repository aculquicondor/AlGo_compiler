#ifdef DEBUG
#include <cassert>
#endif

#include "rule_context.h"


RuleContext::RuleContext() :
        symbol_table(),
        lexemes(new std::stack<std::string>[Token::NUM_OF_TOKENS]),
        attributes(new std::stack<SymbolAttributes>[SyntaxSymbol::NUM_OF_SYMBOLS]) {
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


SymbolAttributes &RuleContext::get_attributes(SyntaxSymbol symbol) const {
#ifdef DEBUG
    assert(not attributes[symbol].empty());
#endif
    return attributes[symbol].top();
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


long RuleContext::get_int_value(Token token) const {
    std::string lex = get_lexeme(token);
    long value = 0;
    if (token == Token::DEC) {
        for (char c : lex)
            value = value * 10 + (c - '0');
    }
    if (token == Token::OCTAL) {
        for (char c : lex.substr(1))
            value = value * 8 + (c - '0');
    } else if (token == Token::HEXADEC) {
        for (char c : lex.substr(2))
            value = value * 16 + (c <= '9' ? c - '0' : c - 'a' + 10);
    }
#ifdef DEBUG
    else
        assert(false);
#endif
    return value;
}


double RuleContext::get_float_value(Token token) const {
#ifdef DEBUG
    if (token != Token::FLOAT)
        assert(false);
#endif
    std::string lex = get_lexeme(token);
    double value = 0;
    std::size_t i = 0;
    while (lex[i] != '.' and lex[i] != 'e' and lex[i] != 'E')
        value = value * 10 + (lex[i++] - '0');
    if (lex[i] == '.') {
        double div = .1;
        ++i;
        while (i < lex.size() and lex[i] != 'e' and lex[i] != 'E') {
            value += (lex[i++] - '0') * div;
            div *= .1;
        }
    }
    if (i < lex.size() and (lex[i] == 'e' or lex[i] == 'E')) {
        int exp = 0;
        bool neg = false;
        ++i;
        if (lex[i] == '-') {
            neg = true;
            ++i;
        } else if (lex[i] == '+') {
            ++i;
        }
        while (i < lex.size())
            exp = exp * 10 + (lex[i++] - '0');
        if (neg)
            exp = -exp;
        value = value * std::pow(10, exp);
    }
    return value;
}


char RuleContext::get_rune_value(Token token) const {
#ifdef DEBUG
    if (token != Token::RUNE)
        assert(false);
#endif
    std::string lex = get_lexeme(token);
    std::size_t p = 1;
    return _parse_rune(lex, p);
}


std::string RuleContext::get_string_value(Token token) const {
    std::string lex = get_lexeme(token);
    std::string value;
    if (token == Token::STRING) {
        for (std::size_t i = 1; i < lex.size() - 1; ++i)
            value.push_back(_parse_rune(lex, i));
    } else if (token == Token::R_STRING) {
        for (std::size_t i = 1; i < lex.size() - 1; ++i)
            value.push_back(lex[i]);
    }
#ifdef DEBUG
    else
        assert(false);
#endif
    return value;
}


char RuleContext::_parse_rune(std::string lex, std::size_t &p) const {
    if (lex[p] != '\\')
        return lex[1];
    ++p;
    if (lex[p] == 'n')
        return '\n';
    if (lex[p] == 't')
        return '\t';
    if (lex[p] == '\\')
        return '\\';
    if (lex[p] == '\'')
        return '\'';
    if (lex[p] == '"')
        return '"';
#ifdef DEBUG
    assert(false);
#endif
}
