#include "semantic_rules.h"


const std::vector<SemanticRule> semantic_rules = {

    [](RuleContext &context) { // 0: Set type in constant declaration
        std::string variable = context.get_lexeme(Token::IDENT);
        if (not context.get_symbol_table().add_symbol(variable)) {
            throw SemanticError("Redeclaration of \"" + variable + "\"",
                                context.get_attributes(Token::IDENT).line_no);
        }
        context.get_symbol_table().get_record(variable).type =
                context.get_attributes(SyntaxSymbol::TYPE).type;
    },

    [](RuleContext &context) { // 1: Setting bool type
        context.get_attributes(SyntaxSymbol::TYPE).type = Type::BOOL;
    },
    [](RuleContext &context) { // 2: Setting int type
        context.get_attributes(SyntaxSymbol::TYPE).type = Type::INT;
    },
    [](RuleContext &context) { // 3: Setting int32 type
        context.get_attributes(SyntaxSymbol::TYPE).type = Type::INT32;
    },
    [](RuleContext &context) { // 4: Setting int64 type
        context.get_attributes(SyntaxSymbol::TYPE).type = Type::INT64;
    },
    [](RuleContext &context) { // 5: Setting uint type
        context.get_attributes(SyntaxSymbol::TYPE).type = Type::UINT;
    },
    [](RuleContext &context) { // 6: Setting uint32 type
        context.get_attributes(SyntaxSymbol::TYPE).type = Type::UINT32;
    },
    [](RuleContext &context) { // 7: Setting uint64 type
        context.get_attributes(SyntaxSymbol::TYPE).type = Type::UINT64;
    },
    [](RuleContext &context) { // 8: Setting float32 type
        context.get_attributes(SyntaxSymbol::TYPE).type = Type::FLOAT32;
    },
    [](RuleContext &context) { // 9: Setting float64 type
        context.get_attributes(SyntaxSymbol::TYPE).type = Type::FLOAT64;
    },
    [](RuleContext &context) { // 10: Setting rune type
        context.get_attributes(SyntaxSymbol::TYPE).type = Type::RUNE;
    },
    [](RuleContext &context) { // 11: Setting string type
        context.get_attributes(SyntaxSymbol::TYPE).type = Type::STRING;
    },

};