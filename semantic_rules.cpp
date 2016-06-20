#include "semantic_rules.h"
#include <iostream>


void declaration(RuleContext &context, bool is_const) {
    std::string variable = context.get_lexeme(Token::IDENT);
    if (not context.get_symbol_table().add_symbol(variable)) {
        throw SemanticError("Redeclaration of \"" + variable + "\"",
                            context.get_attributes(Token::IDENT).line_no);
    }
    auto &record = context.get_symbol_table().get_record(variable);
    record.type_dim = context.get_attributes(SyntaxSymbol::TYPEp).type_dim;
    record.is_const = is_const;
}

void set_type(RuleContext &context, Type type) {
    context.get_attributes(SyntaxSymbol::TYPE).type_dim.type = type;
}

void set_int_value(RuleContext &context, Token token) {
    context.get_attributes(SyntaxSymbol::INT_LIT).int_value =
            context.get_int_value(token);
}

void copy_back(RuleContext &context, SyntaxSymbol symbol) {
    context.get_attributes(symbol, 1) =
            context.get_attributes(symbol);
}

void copy_back_2(RuleContext &context, SyntaxSymbol to_symbol, SyntaxSymbol from_symbol) {
    context.get_attributes(to_symbol) =
            context.get_attributes(from_symbol);
}

const std::vector<SemanticRule> semantic_rules = {
        // 0: const declaration
        std::bind(declaration, std::placeholders::_1, true),

        // 1: Set bool type
        std::bind(set_type, std::placeholders::_1, Type::BOOL),
        // 2: Set int type
        std::bind(set_type, std::placeholders::_1, Type::INT),
        // 3: Set int32 type
        std::bind(set_type, std::placeholders::_1, Type::INT32),
        // 4: Set int64 type
        std::bind(set_type, std::placeholders::_1, Type::INT64),
        // 5: Set uint type
        std::bind(set_type, std::placeholders::_1, Type::UINT),
        // 6: Set uint32 type
        std::bind(set_type, std::placeholders::_1, Type::UINT32),
        // 7: Set uint64 type
        std::bind(set_type, std::placeholders::_1, Type::UINT64),
        // 8: Set float32 type
        std::bind(set_type, std::placeholders::_1, Type::FLOAT32),
        // 9: Set float64 type
        std::bind(set_type, std::placeholders::_1, Type::FLOAT64),
        // 10: set rune type
        std::bind(set_type, std::placeholders::_1, Type::RUNE),
        // 11: set string type
        std::bind(set_type, std::placeholders::_1, Type::STRING),

        // 12: get int value from decimal
        std::bind(set_int_value, std::placeholders::_1, Token::DEC),
        // 13: get int value from decimal
        std::bind(set_int_value, std::placeholders::_1, Token::OCTAL),
        // 14: get int value from decimal
        std::bind(set_int_value, std::placeholders::_1, Token::HEXADEC),

        [](RuleContext &context) { // 15: forward-transmit TYPEp attributes
            auto &t = context.get_attributes(SyntaxSymbol::TYPEp).type_dim;
            t.dimension = context.get_attributes(SyntaxSymbol::TYPEp, 1).type_dim.dimension;
            t.dimension.push_back((std::size_t)context.get_attributes(SyntaxSymbol::INT_LIT).int_value);
        },
        // 16: copy back TYPEp attributes
        std::bind(copy_back, std::placeholders::_1, SyntaxSymbol::TYPEp),
        // 17: copy back type from TYPE to TYPEp
        std::bind(copy_back_2, std::placeholders::_1, SyntaxSymbol::TYPEp, SyntaxSymbol::TYPE),

        // 18: variable declaration
        std::bind(declaration, std::placeholders::_1, false),

};