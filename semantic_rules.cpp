#include "semantic_rules.h"


const std::vector<SemanticRule> semantic_rules = {
    // 0: Set type in constant declaration
    [](RuleContext &context) {
        std::string variable = context.get_lexeme(Token::IDENT);
        if (not context.get_symbol_table().add_symbol(variable)) {
            throw SemanticError("Redeclaration of \"" + variable + "\"",
                                context.get_attributes(Token::IDENT).line_no);
        }
        context.get_symbol_table().get_record(variable).type =
                context.get_attributes(SyntaxSymbol::TYPE).type;
    }
};