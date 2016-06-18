#include "rule_context.h"


RuleContext::RuleContext() :
        symbol_table(), lexemes(new std::stack<std::string>[SyntaxSymbol::NUM_OF_SYMBOLS]) {
}


RuleContext::~RuleContext() {
    delete[] lexemes;
}
