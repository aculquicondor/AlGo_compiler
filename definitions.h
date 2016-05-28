#ifndef ALGO_DEFINITIONS_H
#define ALGO_DEFINITIONS_H

#include <map>


class Token {
public:
    enum _Token {
        NONE,
        COLON,
        DOT,
        SEMICOLON,
        O_PAREN,
        C_PAREN,
        O_BRACK,
        C_BRACK,
        O_SQBRACK,
        C_SQBRACK,
        PLUS,
        MINUS,
        TIMES,
        DIV,
        MOD,
        BW_AND,
        BW_AND_NOT,
        BW_OR,
        BW_XOR_NEG,
        L_SHIFT,
        R_SHIFT,
        A_PLUS,
        A_MINUS,
        A_TIMES,
        A_DIV,
        A_MOD,
        A_BW_AND,
        A_BW_AND_NOT,
        A_BW_OR,
        A_BW_XOR_NEG,
        A_L_SHIFT,
        A_R_SHIFT,
        INCR,
        DECR,
        ASSIGN,
        // DECL_ASSIGN,
        EQ,
        NEQ,
        LT,
        GT,
        LTE,
        GTE,
        OR,
        AND,
        NOT,
        IDENT,
        DEC,
        OCTAL,
        HEXADEC,
        FLOAT,
        RUNE,
        STRING,
        R_STRING,
        TRUE,
        FALSE,
        CONST,
        VAR,
        FOR,
        IF,
        ELSE,
        BREAK,
        CONTINUE,
        RETURN,
        FUNC,
        PKG,
        IMP,
        BOOL,
        INT,
        I32,
        I64,
        UINT,
        UI32,
        UI64,
        FL32,
        FL64,
        RN,
        STR
    };

    Token(int value = NONE) : value(value) { }

    bool operator==(const Token &t) const {
        return value == t.value;
    }

    bool operator!=(const Token &t) const {
        return value != t.value;
    }

    int get_value() const {
        return value;
    }

protected:
    int value;
};


class SyntaxSymbol : public Token {
public:
    enum _SyntaxSymbol {
        PACKAGE = Token::STR + 1,
        IMPORT_DECLS,
        PKG_DECLS,
        CONST_DECL,
        VAR_DECL,
        VAR_DECLp,
        FUNC_DECL,
        PARAM_LIST,
        PARAM_LISTp,
        BLOCK,
        TYPE,
        BLOCK_CONTS,
        BLOCK_UNIT,
        RETURNp,
        DECL,
        EXPR,
        EXPRp,
        ASSIGNMENT,
        ASSIGN_OPER,
        LV1EXPR,
        LV1EXPRp,
        LV1OPER,
        LV2EXPR,
        LV2EXPRp,
        LV2OPER,
        LV3EXPR,
        LV3EXPRp,
        LV3OPER,
        LV4EXPR,
        LV4EXPRp,
        LV4OPER,
        LV5EXPR,
        LV5EXPRp,
        LV5OPER,
        UNARYOPER,
        TERM,
        IDCR,
        CAST,
        ACCESS,
        FUNC_CALL,
        ARRAY_ACC,
        PARAMS,
        PARAMSp,
        IF_CONST,
        IF_CONSTp,
        ELSEp,
        FOR_CONST,
        FOR_CONSTp,
        FOR_CONSTpp
    };

    SyntaxSymbol(int value = NONE) : Token(value) { }

    SyntaxSymbol(const std::string name) {
        auto it = str_to_value.find(name);
        value = it != str_to_value.end() ? it->second : NONE;
    }

    bool operator==(const SyntaxSymbol &s) const {
        return value == s.value;
    }

    bool operator!=(const SyntaxSymbol &s) const {
        return value != s.value;
    }

    bool is_terminal() const {
        return value < PACKAGE;
    }

private:
    static const std::map<std::string, int> str_to_value;
};

#endif //ALGO_DEFINITIONS_H
