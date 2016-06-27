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
    record.is_function = false;
}

void set_type(RuleContext &context, Type type) {
    context.get_attributes(SyntaxSymbol::TYPE).type_dim.type = type;
}

void set_int_value(RuleContext &context, Token token) {
    context.get_attributes(SyntaxSymbol::INT_LIT).int_value =
            context.get_int_value(token);
}

void copy_back(RuleContext &context, SyntaxSymbol symbol) {
    context.get_attributes(symbol, 1) = context.get_attributes(symbol);
}

void copy_2(RuleContext &context, SyntaxSymbol to_symbol, SyntaxSymbol from_symbol) {
    context.get_attributes(to_symbol) = context.get_attributes(from_symbol);
}

void forward_add_params(RuleContext &context, SyntaxSymbol symbol, std::size_t r_idx) {
    std::string name = add_ident(context);
    auto &params = context.get_attributes(SyntaxSymbol::PARAM_LISTp).params;
    params = context.get_attributes(symbol, r_idx).params;
    const auto &attributes = context.get_attributes(SyntaxSymbol::TYPEp);
    params.push_back(attributes.type_dim);
    auto &record = context.get_symbol_table().get_record(name);
    record.type_dim = attributes.type_dim;
    record.is_const = false;
    record.is_function = false;
}

void forward_return_loop_info(RuleContext &context, SyntaxSymbol symbol) {
    auto &to_attributes = context.get_attributes(symbol);
    const auto &from_attributes = context.get_attributes(symbol, 1);
    to_attributes.return_type_dim = from_attributes.return_type_dim;
    to_attributes.in_loop = from_attributes.in_loop;
}

void forward_return_loop_info_2(RuleContext &context, SyntaxSymbol to, SyntaxSymbol from) {
    auto &to_attributes = context.get_attributes(to);
    const auto &from_attributes = context.get_attributes(from);
    to_attributes.return_type_dim = from_attributes.return_type_dim;
    to_attributes.in_loop = from_attributes.in_loop;
}

void verify_inside_loop(RuleContext &context, SyntaxSymbol symbol) {
    if (not context.get_attributes(SyntaxSymbol::BLOCK_UNIT).in_loop) {
        throw SemanticError("Statement not inside a loop",
                            context.get_attributes(symbol).line_no);
    }
}

bool equal_dimension(std::vector<std::size_t> d1, std::vector<std::size_t> d2) {
    if (d1.size() != d2.size())
        return false;
    for (std::size_t i = 0; i < d1.size(); ++i)
        if (d1[i] != d2[i])
            return false;
    return true;
}

bool is_int_type(Type type) {
    return type >= Type::UINT and type <= Type::INT64;
}

bool is_float_type(Type type) {
    return type == Type::FLOAT32 or type == Type::FLOAT64;
}

SymbolAttributes check_types(const SymbolAttributes &op1, const SymbolAttributes &op2, std::size_t line_no) {
    SymbolAttributes attributes;
    SemanticError error("Types mismatch", line_no);
    if (not equal_dimension(op1.type_dim.dimension, op2.type_dim.dimension))
        throw SemanticError("Dimensions mismatch", line_no);
    if (op1.is_literal == op2.is_literal) {
        if (op1.type_dim.type != op2.type_dim.type)
            throw error;
        attributes.is_literal = op1.is_literal;
    } else {
        const SymbolAttributes &literal = op1.is_literal ? op1 : op2;
        const SymbolAttributes &variable = op1.is_literal ? op2 : op1;
        if (literal.type_dim.type == Type::INT64) {
             if (not is_int_type(variable.type_dim.type))
                 throw error;
        } else if (literal.type_dim.type == Type::FLOAT64) {
          if (not is_float_type(variable.type_dim.type))
              throw error;
        } else if (literal.type_dim.type != variable.type_dim.type)
            throw error;
        attributes.is_literal = false;
    }
    attributes.is_const = op1.is_const and op2.is_const;
    return attributes;
}

void set_operation(RuleContext &context, SyntaxSymbol to, SyntaxSymbol from, Operation operation) {
    auto &attributes = context.get_attributes(to);
    attributes.operation = operation;
    attributes.line_no = context.get_attributes(from).line_no;
}

void pass_operation(RuleContext &context, SyntaxSymbol to, SyntaxSymbol from) {
    auto &to_attributes = context.get_attributes(to);
    const auto &from_attributes = context.get_attributes(from);
    to_attributes.operation = from_attributes.operation;
    to_attributes.line_no = from_attributes.line_no;
}

void check_operation_for_typedim(SymbolAttributes::TypeDim type_dim, Operation operation, std::size_t line_no) {
    if (type_dim.type == Type::STRING and operation > Operation::ADD)
        throw SemanticError("Can't use operator for strings", line_no);
    if (type_dim.type == Type::RUNE and operation >= Operation::ADD)
        throw SemanticError("Can't use operator for runes", line_no);
    if (not type_dim.dimension.empty() and operation > Operation::NONE)
        throw SemanticError("Can't use operator for arrays", line_no);
    if (is_float_type(type_dim.type) and operation >= Operation ::MOD)
        throw SemanticError("Can't use operator for floats", line_no);
    if (is_int_type(type_dim.type) and operation >= Operation ::OR)
        throw SemanticError("Can't use operator for integers", line_no);
}

void operate(RuleContext &context, SyntaxSymbol left, SyntaxSymbol right) {
    const auto &left_attributes = context.get_attributes(left);
    if (context.get_attributes(left).operation == Operation::NONE)
        return;
    auto &right_attributes = context.get_attributes(right);
    check_operation_for_typedim(left_attributes.type_dim, left_attributes.operation, left_attributes.line_no);
    SymbolAttributes result = check_types(left_attributes, right_attributes, left_attributes.line_no);
    right_attributes.is_const = result.is_const;
    right_attributes.is_literal = result.is_literal;
    right_attributes.is_lvalue = false;
    if (left_attributes.operation < Operation::ADD)
        right_attributes.type_dim = {Type::BOOL, {}};
}

void get_literal_info(RuleContext &context, Type type, Token token) {
    auto &attributes = context.get_attributes(SyntaxSymbol::TERM);
    attributes.type_dim = {type, {}};
    attributes.is_lvalue = false;
    attributes.is_const = true;
    attributes.is_literal = true;
    if (type == Type::INT)
        attributes.int_value = context.get_int_value(token);
    else if (type == Type::FLOAT64)
        attributes.float_value = context.get_float_value(token);
    else if (type == Type::STRING)
        attributes.str_value = context.get_string_value(token);
    else if (type == Type::RUNE)
        attributes.rune_value = context.get_rune_value(token);
    else if (type == Type::BOOL)
        attributes.bool_value = context.get_bool_value(token);
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
            auto &dimension = context.get_attributes(SyntaxSymbol::TYPEp).type_dim.dimension;
            dimension = context.get_attributes(SyntaxSymbol::TYPEp, 1).type_dim.dimension;
            dimension.push_back((std::size_t)context.get_attributes(SyntaxSymbol::INT_LIT).int_value);
        },
        // 16: copy-back TYPEp attributes
        std::bind(copy_back, std::placeholders::_1, SyntaxSymbol::TYPEp),
        // 17: copy-back attributes from TYPE to TYPEp
        [](RuleContext &context) {
            context.get_attributes(SyntaxSymbol::TYPEp).type_dim.type =
                    context.get_attributes(SyntaxSymbol::TYPE).type_dim.type;
        },

        // 18: variable declaration
        std::bind(declaration, std::placeholders::_1, false),

        // 19: copy-back attributes from TYPEp to FUNC_DECLPp
        std::bind(copy_2, std::placeholders::_1, SyntaxSymbol::FUNC_DECLp, SyntaxSymbol::TYPEp),

        // 20: declare function and create new scope
        [](RuleContext &context) {
            add_ident(context);
            context.get_symbol_table().start_scope();
        },
        // 21: set function params and return type
        [](RuleContext &context) {
            std::string name = context.get_lexeme(Token::IDENT);
            auto &record = context.get_symbol_table().get_record(name);
            record.is_function = true;
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
        std::bind(copy_2, std::placeholders::_1,
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

        // 40: create scope
        [](RuleContext &context) {
            context.get_symbol_table().start_scope();
        },

        // 41: forward l_value and identifier
        [](RuleContext &context) {
            const auto &lv1expr_attributes = context.get_attributes(SyntaxSymbol::LV1EXPR);
            if (lv1expr_attributes.is_lvalue) {
                auto &exprp_attributes = context.get_attributes(SyntaxSymbol::EXPRp);
                exprp_attributes.is_lvalue = true;
                exprp_attributes.identifier = lv1expr_attributes.identifier;
                exprp_attributes.type_dim = lv1expr_attributes.type_dim;
                exprp_attributes.is_const = lv1expr_attributes.is_const;
            }
        },

        // 42: assignment
        [](RuleContext &context) {
            const auto &exprp_attributes = context.get_attributes(SyntaxSymbol::EXPRp);
            const auto &assign_oper_attributes = context.get_attributes(SyntaxSymbol::ASSIGN_OPER);
            if (not exprp_attributes.is_lvalue)
                throw SemanticError("Expression not an lvalue",
                                    assign_oper_attributes.line_no);
            if (exprp_attributes.is_const)
                throw SemanticError("Can't modify a const value", assign_oper_attributes.line_no);
            check_operation_for_typedim(exprp_attributes.type_dim, assign_oper_attributes.operation,
                                        assign_oper_attributes.line_no);
            check_types(exprp_attributes, context.get_attributes(SyntaxSymbol::EXPR),
                        assign_oper_attributes.line_no);
        },

        // 43: const declaration assignment
        [](RuleContext &context) {
            SymbolAttributes attributes;
            attributes.type_dim = context.get_symbol_table().get_record(context.get_lexeme(Token::IDENT)).type_dim;
            attributes.is_literal = false;
            check_types(attributes, context.get_attributes(SyntaxSymbol::EXPR),
                        context.get_attributes(SyntaxSymbol::ASSIGN).line_no);
        },

        // 44: set operation
        std::bind(set_operation, std::placeholders::_1,
                  SyntaxSymbol::ASSIGN_OPER, SyntaxSymbol::ASSIGN, Operation::NONE),
        // 45
        std::bind(set_operation, std::placeholders::_1,
                  SyntaxSymbol::ASSIGN_OPER, SyntaxSymbol::A_PLUS, Operation::ADD),
        // 46
        std::bind(set_operation, std::placeholders::_1,
                  SyntaxSymbol::ASSIGN_OPER, SyntaxSymbol::A_MINUS, Operation::SUBS),
        // 47
        std::bind(set_operation, std::placeholders::_1,
                  SyntaxSymbol::ASSIGN_OPER, SyntaxSymbol::A_TIMES, Operation::MULT),
        // 48
        std::bind(set_operation, std::placeholders::_1,
                  SyntaxSymbol::ASSIGN_OPER, SyntaxSymbol::A_DIV, Operation::DIV),
        // 49
        std::bind(set_operation, std::placeholders::_1,
                  SyntaxSymbol::ASSIGN_OPER, SyntaxSymbol::A_MOD, Operation::MOD),
        // 50
        std::bind(set_operation, std::placeholders::_1,
                  SyntaxSymbol::ASSIGN_OPER, SyntaxSymbol::A_BW_AND, Operation::BW_AND),
        // 51
        std::bind(set_operation, std::placeholders::_1,
                  SyntaxSymbol::ASSIGN_OPER, SyntaxSymbol::A_BW_AND_NOT, Operation::BW_AND_NOT),
        // 52
        std::bind(set_operation, std::placeholders::_1,
                  SyntaxSymbol::ASSIGN_OPER, SyntaxSymbol::A_BW_OR, Operation::BW_OR),
        // 53
        std::bind(set_operation, std::placeholders::_1,
                  SyntaxSymbol::ASSIGN_OPER, SyntaxSymbol::A_BW_XOR, Operation::BW_XOR),
        // 54
        std::bind(set_operation, std::placeholders::_1,
                  SyntaxSymbol::ASSIGN_OPER, SyntaxSymbol::A_L_SHIFT, Operation::L_SHIFT),
        // 55
        std::bind(set_operation, std::placeholders::_1,
                  SyntaxSymbol::ASSIGN_OPER, SyntaxSymbol::A_R_SHIFT, Operation::R_SHIFT),

        // 56: forward at lv1expr
        std::bind(copy_2, std::placeholders::_1, SyntaxSymbol::LV1EXPRp, SyntaxSymbol::LV2EXPR),
        // 57: copy-back/forward at lv1expr
        std::bind(copy_2, std::placeholders::_1, SyntaxSymbol::LV1EXPR, SyntaxSymbol::LV1EXPRp),
        // 58: copy operation at lv1exprp
        std::bind(pass_operation, std::placeholders::_1, SyntaxSymbol::LV1EXPR, SyntaxSymbol::LV1OPER),
        // 59 operate at lv1expr
        std::bind(operate, std::placeholders::_1, SyntaxSymbol::LV1EXPR, SyntaxSymbol::LV2EXPR),
        // 60: copy back at lv1exprp
        std::bind(copy_2, std::placeholders::_1, SyntaxSymbol::LV1EXPRp, SyntaxSymbol::LV1EXPR),
        // 61: set lv1oper
        std::bind(set_operation, std::placeholders::_1,
                  SyntaxSymbol::LV1OPER, SyntaxSymbol::OR, Operation::OR),

        // 62: forward at lv2expr
        std::bind(copy_2, std::placeholders::_1, SyntaxSymbol::LV2EXPRp, SyntaxSymbol::LV3EXPR),
        // 63: copy-back/forward at lv2expr
        std::bind(copy_2, std::placeholders::_1, SyntaxSymbol::LV2EXPR, SyntaxSymbol::LV2EXPRp),
        // 64: copy operation at lv2exprp
        std::bind(pass_operation, std::placeholders::_1, SyntaxSymbol::LV2EXPR, SyntaxSymbol::LV2OPER),
        // 65 operate at lv2expr
        std::bind(operate, std::placeholders::_1, SyntaxSymbol::LV2EXPR, SyntaxSymbol::LV3EXPR),
        // 66: copy back at lv2exprp
        std::bind(copy_2, std::placeholders::_1, SyntaxSymbol::LV2EXPRp, SyntaxSymbol::LV2EXPR),
        // 67: set lv2oper
        std::bind(set_operation, std::placeholders::_1,
                  SyntaxSymbol::LV2OPER, SyntaxSymbol::AND, Operation::AND),

        // 68: forward at lv3expr
        std::bind(copy_2, std::placeholders::_1, SyntaxSymbol::LV3EXPRp, SyntaxSymbol::LV4EXPR),
        // 69: copy-back/forward at lv3expr
        std::bind(copy_2, std::placeholders::_1, SyntaxSymbol::LV3EXPR, SyntaxSymbol::LV3EXPRp),
        // 70: copy operation at lv3exprp
        std::bind(pass_operation, std::placeholders::_1, SyntaxSymbol::LV3EXPR, SyntaxSymbol::LV3OPER),
        // 71 operate at lv3expr
        std::bind(operate, std::placeholders::_1, SyntaxSymbol::LV3EXPR, SyntaxSymbol::LV4EXPR),
        // 72: copy back at lv3exprp
        std::bind(copy_2, std::placeholders::_1, SyntaxSymbol::LV3EXPRp, SyntaxSymbol::LV3EXPR),
        // 73: set lv3oper
        std::bind(set_operation, std::placeholders::_1,
                  SyntaxSymbol::LV3OPER, SyntaxSymbol::EQ, Operation::EQ),
        // 74
        std::bind(set_operation, std::placeholders::_1,
                  SyntaxSymbol::LV3OPER, SyntaxSymbol::NEQ, Operation::NEQ),
        // 75
        std::bind(set_operation, std::placeholders::_1,
                  SyntaxSymbol::LV3OPER, SyntaxSymbol::LT, Operation::LT),
        // 76
        std::bind(set_operation, std::placeholders::_1,
                  SyntaxSymbol::LV3OPER, SyntaxSymbol::GT, Operation::GT),
        // 77
        std::bind(set_operation, std::placeholders::_1,
                  SyntaxSymbol::LV3OPER, SyntaxSymbol::LTE, Operation::LTE),
        // 78
        std::bind(set_operation, std::placeholders::_1,
                  SyntaxSymbol::LV3OPER, SyntaxSymbol::GTE, Operation::GTE),

        // 79: forward at lv4expr
        std::bind(copy_2, std::placeholders::_1, SyntaxSymbol::LV4EXPRp, SyntaxSymbol::LV5EXPR),
        // 80: copy-back/forward at lv4expr
        std::bind(copy_2, std::placeholders::_1, SyntaxSymbol::LV4EXPR, SyntaxSymbol::LV4EXPRp),
        // 81: copy operation at lv4exprp
        std::bind(pass_operation, std::placeholders::_1, SyntaxSymbol::LV4EXPR, SyntaxSymbol::LV4OPER),
        // 82 operate at lv4expr
        std::bind(operate, std::placeholders::_1, SyntaxSymbol::LV4EXPR, SyntaxSymbol::LV5EXPR),
        // 83: copy back at lv4exprp
        std::bind(copy_2, std::placeholders::_1, SyntaxSymbol::LV4EXPRp, SyntaxSymbol::LV4EXPR),
        // 84: set lv4oper
        std::bind(set_operation, std::placeholders::_1,
                  SyntaxSymbol::LV4OPER, SyntaxSymbol::PLUS, Operation::ADD),
        // 85
        std::bind(set_operation, std::placeholders::_1,
                  SyntaxSymbol::LV4OPER, SyntaxSymbol::MINUS, Operation::SUBS),
        // 86
        std::bind(set_operation, std::placeholders::_1,
                  SyntaxSymbol::LV4OPER, SyntaxSymbol::BW_OR, Operation::BW_OR),
        // 87
        std::bind(set_operation, std::placeholders::_1,
                  SyntaxSymbol::LV4OPER, SyntaxSymbol::BW_XOR_NEG, Operation::BW_XOR),

        // 88: unary operation
        [](RuleContext &context) {
            const auto &operation_attributes = context.get_attributes(SyntaxSymbol::UNARYOPER);
            auto &attributes = context.get_attributes(SyntaxSymbol::TERM);
            check_operation_for_typedim(attributes.type_dim, operation_attributes.operation,
                                        operation_attributes.line_no);
            if (operation_attributes.operation == Operation::INCR or
                        operation_attributes.operation == Operation::DECR) {
                if (not attributes.is_lvalue)
                    throw SemanticError("Expression not an lvalue", operation_attributes.line_no);
            } else {
                attributes.is_lvalue = false;
            }
        },
        // 89: forward at lv5expr
        std::bind(copy_2, std::placeholders::_1, SyntaxSymbol::LV5EXPRp, SyntaxSymbol::TERM),
        // 90: copy-back/forward at lv5expr
        std::bind(copy_2, std::placeholders::_1, SyntaxSymbol::LV5EXPR, SyntaxSymbol::LV5EXPRp),
        // 91: copy operation at lv5exprp
        std::bind(pass_operation, std::placeholders::_1, SyntaxSymbol::LV5EXPR, SyntaxSymbol::LV5OPER),
        // 92: operate at lv5expr
        std::bind(operate, std::placeholders::_1, SyntaxSymbol::LV5EXPR, SyntaxSymbol::TERM),
        // 93: copy back at lv4exprp
        std::bind(copy_2, std::placeholders::_1, SyntaxSymbol::LV5EXPRp, SyntaxSymbol::LV5EXPR),
        // 94: set lv5oper
        std::bind(set_operation, std::placeholders::_1,
                  SyntaxSymbol::LV5OPER, SyntaxSymbol::TIMES, Operation::MULT),
        // 95
        std::bind(set_operation, std::placeholders::_1,
                  SyntaxSymbol::LV5OPER, SyntaxSymbol::DIV, Operation::DIV),
        // 96
        std::bind(set_operation, std::placeholders::_1,
                  SyntaxSymbol::LV5OPER, SyntaxSymbol::MOD, Operation::MOD),
        // 97
        std::bind(set_operation, std::placeholders::_1,
                  SyntaxSymbol::LV5OPER, SyntaxSymbol::BW_AND, Operation::BW_AND),
        // 98
        std::bind(set_operation, std::placeholders::_1,
                  SyntaxSymbol::LV5OPER, SyntaxSymbol::BW_AND_NOT, Operation::BW_AND_NOT),
        // 99
        std::bind(set_operation, std::placeholders::_1,
                  SyntaxSymbol::LV5OPER, SyntaxSymbol::L_SHIFT, Operation::L_SHIFT),
        // 100
        std::bind(set_operation, std::placeholders::_1,
                  SyntaxSymbol::LV5OPER, SyntaxSymbol::R_SHIFT, Operation::R_SHIFT),
        // 101: set unary operator
        std::bind(set_operation, std::placeholders::_1,
                  SyntaxSymbol::UNARYOPER, SyntaxSymbol::PLUS, Operation::ADD),
        // 102
        std::bind(set_operation, std::placeholders::_1,
                  SyntaxSymbol::UNARYOPER, SyntaxSymbol::MINUS, Operation::SUBS),
        // 103
        std::bind(set_operation, std::placeholders::_1,
                  SyntaxSymbol::UNARYOPER, SyntaxSymbol::BW_XOR_NEG, Operation::BW_NEG),
        // 104
        std::bind(set_operation, std::placeholders::_1,
                  SyntaxSymbol::UNARYOPER, SyntaxSymbol::INCR, Operation::INCR),
        // 105
        std::bind(set_operation, std::placeholders::_1,
                  SyntaxSymbol::UNARYOPER, SyntaxSymbol::DECR, Operation::DECR),
        // 106
        std::bind(set_operation, std::placeholders::_1,
                  SyntaxSymbol::UNARYOPER, SyntaxSymbol::NOT, Operation::NOT),

        // 107 copy back at expr
        std::bind(copy_2, std::placeholders::_1, SyntaxSymbol::EXPR, SyntaxSymbol::LV1EXPR),

        // 108 get identifier info
        [](RuleContext &context) {
            std::string name = context.get_lexeme(SyntaxSymbol::IDENT);
            if (not context.get_symbol_table().has_symbol(name))
                throw SemanticError("Unknown identifier \"" + name + "\"",
                                    context.get_attributes(SyntaxSymbol::IDENT).line_no);
            const auto &record = context.get_symbol_table().get_record(name);
            auto &attributes = context.get_attributes(SyntaxSymbol::IDENT);
            attributes.identifier = name;
            attributes.is_lvalue = not record.is_function;
            attributes.is_const = record.is_const;
            attributes.type_dim = record.type_dim;
            attributes.is_literal = false;
        },

        // 109 get decimal literal info
        std::bind(get_literal_info, std::placeholders::_1, Type::INT, Token::DEC),
        // 110 get octal literal info
        std::bind(get_literal_info, std::placeholders::_1, Type::INT, Token::OCTAL),
        // 111 get hexadecimal literal info
        std::bind(get_literal_info, std::placeholders::_1, Type::INT, Token::HEXADEC),
        // 112 get float literal info
        std::bind(get_literal_info, std::placeholders::_1, Type::FLOAT64, Token::FLOAT),
        // 113 get rune literal info
        std::bind(get_literal_info, std::placeholders::_1, Type::RUNE, Token::RUNE),
        // 114 get string literal info
        std::bind(get_literal_info, std::placeholders::_1, Type::STRING, Token::STRING),
        // 115 get raw string literal info
        std::bind(get_literal_info, std::placeholders::_1, Type::STRING, Token::R_STRING),
        // 116 get true literal info
        std::bind(get_literal_info, std::placeholders::_1, Type::BOOL, Token::TRUE),
        // 117 get true literal info
        std::bind(get_literal_info, std::placeholders::_1, Type::BOOL, Token::FALSE),

        // 118 copy back parenthesized lv1expr
        std::bind(copy_2, std::placeholders::_1, SyntaxSymbol::TERM, SyntaxSymbol::LV1EXPR),

        // 119 copy back from cast
        std::bind(copy_2, std::placeholders::_1, SyntaxSymbol::TERM, SyntaxSymbol::CAST),
        // 120 copy back to cast
        std::bind(copy_2, std::placeholders::_1, SyntaxSymbol::CAST, SyntaxSymbol::LV1EXPR),
        // 121: do cast
        [](RuleContext &context) {
            context.get_attributes(SyntaxSymbol::CAST).type_dim =
                    {context.get_attributes(SyntaxSymbol::TYPE).type_dim.type, {}};
        },

        // 122: forward to access
        std::bind(copy_2, std::placeholders::_1, SyntaxSymbol::ACCESS, SyntaxSymbol::IDENT),
        // 123: forward to func_call
        [](RuleContext &context) {
            context.get_attributes(SyntaxSymbol::FUNC_CALL) =
                    context.get_attributes(SyntaxSymbol::ACCESS, 1);
        },
        // 124: forward to array_acc
        [](RuleContext &context) {
            context.get_attributes(SyntaxSymbol::ARRAY_ACC) =
                    context.get_attributes(SyntaxSymbol::ACCESS, 1);
        },
        // 125: verify not function
        [](RuleContext &context) {
            const auto &attributes = context.get_attributes(SyntaxSymbol::ACCESS);
            if (attributes.is_function)
                throw SemanticError(attributes.identifier + " is a function", attributes.line_no);
        },
        // 126: verify is function
        [](RuleContext &context) {
            auto &attributes = context.get_attributes(SyntaxSymbol::FUNC_CALL);
            if (not attributes.is_function)
                throw SemanticError(attributes.identifier + " is not a function",
                                    context.get_attributes(SyntaxSymbol::O_PAREN).line_no);
            attributes.is_function = false;
            attributes.is_lvalue = false;
        },
        // 127: access array
        [](RuleContext &context) {
            auto &attributes = context.get_attributes(SyntaxSymbol::ARRAY_ACC);
            std::size_t line_no = context.get_attributes(SyntaxSymbol::C_SQBRACK).line_no;
            if (attributes.type_dim.dimension.empty())
                throw SemanticError("Dimensions mismatch on access to " + attributes.identifier, line_no);
            const auto &index_typedim = context.get_attributes(SyntaxSymbol::LV1EXPR).type_dim;
            if (not is_int_type(index_typedim.type) or not index_typedim.dimension.empty())
                throw SemanticError("Not a valid index", line_no);
            attributes.type_dim.dimension.pop_back();
        },
        // 128: forward to access
        std::bind(copy_2, std::placeholders::_1, SyntaxSymbol::ACCESS, SyntaxSymbol::FUNC_CALL),
        // 129
        std::bind(copy_2, std::placeholders::_1, SyntaxSymbol::ACCESS, SyntaxSymbol::ARRAY_ACC),
        // 130: copy back access
        std::bind(copy_back, std::placeholders::_1, SyntaxSymbol::ACCESS),
        // 131: copy back from access to term
        std::bind(copy_2, std::placeholders::_1, SyntaxSymbol::TERM, SyntaxSymbol::ACCESS),

        // 132: check if expr type
        [](RuleContext &context) {
            const auto &type_dim = context.get_attributes(SyntaxSymbol::EXPR).type_dim;
            if (type_dim.type != Type::BOOL or not type_dim.dimension.empty())
                throw SemanticError("Not a boolean expression",
                                    context.get_attributes(SyntaxSymbol::IF).line_no);
        },
        // 133: check for_constpp expr type
        [](RuleContext &context) {
            const auto &type_dim = context.get_attributes(SyntaxSymbol::EXPR).type_dim;
            if (type_dim.type != Type::BOOL or not type_dim.dimension.empty())
                throw SemanticError("Not a boolean expression",
                                    context.get_attributes(SyntaxSymbol::SEMICOL).line_no);
            context.get_attributes(SyntaxSymbol::FOR_CONSTpp).three_for = true;
        },
        // 134
        [](RuleContext &context) {
            context.get_attributes(SyntaxSymbol::FOR_CONSTpp).three_for = false;
        },
        // 135
        [](RuleContext &context) {
            if (context.get_attributes(SyntaxSymbol::FOR_CONSTpp).three_for)
                return;
            const auto &type_dim = context.get_attributes(SyntaxSymbol::EXPR).type_dim;
            if (type_dim.type != Type::BOOL or not type_dim.dimension.empty())
                throw SemanticError("Not a boolean expression",
                                    context.get_attributes(SyntaxSymbol::FOR).line_no);
        },
};
