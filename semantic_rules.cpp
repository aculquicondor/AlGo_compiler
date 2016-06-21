#include "semantic_rules.h"
#include <iostream>


std::string add_ident(RuleContext &context) {
    std::string name = context.get_lexeme(Token::IDENT);
    if (not context.get_symbol_table().add_symbol(name)) {
        throw SemanticError("Redeclaration of \"" + name + "\"",
                            context.get_attributes(Token::IDENT).line_no);
    }
    return name;
}

void declaration(RuleContext &context, bool is_const) {
    std::string name = add_ident(context);
    auto &record = context.get_symbol_table().get_record(name);
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

void forward_add_params(RuleContext &context, SyntaxSymbol symbol, std::size_t r_idx) {
    add_ident(context);
    auto &params = context.get_attributes(SyntaxSymbol::PARAM_LISTp).params;
    params = context.get_attributes(symbol, r_idx).params;
    params.push_back(context.get_attributes(SyntaxSymbol::TYPEp).type_dim);
}

void forward_return_loop_info(RuleContext &context, SyntaxSymbol symbol) {
    auto &to_attributes = context.get_attributes(symbol);
    const auto &from_attributes = context.get_attributes(symbol, 1);
    to_attributes.return_type_dim = from_attributes.return_type_dim;
    to_attributes.in_loop = from_attributes.in_loop;
}

void forward_return_loop_info_2(RuleContext &context, SyntaxSymbol to_symbol, SyntaxSymbol from_symbol) {
    auto &to_attributes = context.get_attributes(to_symbol);
    const auto &from_attributes = context.get_attributes(from_symbol);
    to_attributes.return_type_dim = from_attributes.return_type_dim;
    to_attributes.in_loop = from_attributes.in_loop;
}

void verify_inside_loop(RuleContext &context, SyntaxSymbol symbol) {
    if (not context.get_attributes(SyntaxSymbol::BLOCK_UNIT).in_loop) {
        throw SemanticError("Statement not inside a loop",
                            context.get_attributes(symbol).line_no);
    }
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

        // 15: forward TYPEp attributes
        [](RuleContext &context) {
            auto &t = context.get_attributes(SyntaxSymbol::TYPEp).type_dim;
            t.dimension = context.get_attributes(SyntaxSymbol::TYPEp, 1).type_dim.dimension;
            t.dimension.push_back((std::size_t)context.get_attributes(SyntaxSymbol::INT_LIT).int_value);
        },
        // 16: copy-back TYPEp attributes
        std::bind(copy_back, std::placeholders::_1, SyntaxSymbol::TYPEp),
        // 17: copy-back attributes from TYPE to TYPEp
        std::bind(copy_back_2, std::placeholders::_1, SyntaxSymbol::TYPEp, SyntaxSymbol::TYPE),

        // 18: variable declaration
        std::bind(declaration, std::placeholders::_1, false),

        // 19: copy-back attributes from TYPEp to FUNC_DECLPp
        std::bind(copy_back_2, std::placeholders::_1, SyntaxSymbol::FUNC_DECLp, SyntaxSymbol::TYPEp),

        // 20: declare function and create new scope
        [](RuleContext &context) {
            add_ident(context);
            context.get_symbol_table().start_scope();
        },
        // 21: set function params and return type
        [](RuleContext &context) {
            std::string name = add_ident(context);
            auto &record = context.get_symbol_table().get_record(name);
            record.type_dim = context.get_attributes(SyntaxSymbol::FUNC_DECLp).type_dim;
            record.params = context.get_attributes(SyntaxSymbol::PARAM_LIST).params;

            auto &attributes = context.get_attributes(SyntaxSymbol::BLOCK);
            attributes.return_type_dim = record.type_dim;
            attributes.in_loop = false;
        },

        // 22: end scope
        [](RuleContext &context) {
            context.get_symbol_table().end_scope();
        },

        // 23: forward & add params from PARAM_LIST
        std::bind(forward_add_params, std::placeholders::_1, SyntaxSymbol::PARAM_LIST, 0),
        // 24: forward/copy-back params
        std::bind(copy_back_2, std::placeholders::_1,
                  SyntaxSymbol::PARAM_LIST, SyntaxSymbol::PARAM_LISTp),
        // 25: forward & add params from PARAM_LISTp
        std::bind(forward_add_params, std::placeholders::_1, SyntaxSymbol::PARAM_LISTp, 1),
        // 26 copy-back params
        std::bind(copy_back, std::placeholders::_1, SyntaxSymbol::PARAM_LISTp),

        // 27: forward return and loop information
        std::bind(forward_return_loop_info_2, std::placeholders::_1,
                  SyntaxSymbol::BLOCK_CONTS, SyntaxSymbol::BLOCK),
        // 28
        std::bind(forward_return_loop_info_2, std::placeholders::_1,
                  SyntaxSymbol::BLOCK_UNIT, SyntaxSymbol::BLOCK_CONTS),
        // 29
        std::bind(forward_return_loop_info, std::placeholders::_1, SyntaxSymbol::BLOCK_CONTS),
        // 30
        std::bind(forward_return_loop_info_2, std::placeholders::_1,
                  SyntaxSymbol::IF_CONST, SyntaxSymbol::BLOCK_UNIT),
        // 31
        std::bind(forward_return_loop_info_2, std::placeholders::_1,
                  SyntaxSymbol::BLOCK, SyntaxSymbol::IF_CONST),
        // 32
        std::bind(forward_return_loop_info_2, std::placeholders::_1,
                  SyntaxSymbol::FOR_CONST, SyntaxSymbol::BLOCK_UNIT),
        // 33: forward return and set loop information
        [](RuleContext &context) {
            auto &attributes = context.get_attributes(SyntaxSymbol::BLOCK);
            attributes.in_loop = true;
            attributes.return_type_dim = context.get_attributes(SyntaxSymbol::FOR_CONST).type_dim;
        },
        // 34: verify continue inside loop
        std::bind(verify_inside_loop, std::placeholders::_1, Token::CONTINUE),
        // 35: verify break inside loop
        std::bind(verify_inside_loop, std::placeholders::_1, Token::BREAK),
        // 36: forward return and loop information
        std::bind(forward_return_loop_info_2, std::placeholders::_1,
                  SyntaxSymbol::IF_CONSTp, SyntaxSymbol::IF_CONST),
        // 37
        std::bind(forward_return_loop_info_2, std::placeholders::_1,
                  SyntaxSymbol::ELSEp, SyntaxSymbol::IF_CONSTp),
        // 38
        std::bind(forward_return_loop_info_2, std::placeholders::_1,
                  SyntaxSymbol::IF_CONST, SyntaxSymbol::ELSEp),
        // 39
        std::bind(forward_return_loop_info_2, std::placeholders::_1,
                  SyntaxSymbol::BLOCK, SyntaxSymbol::ELSEp),
};