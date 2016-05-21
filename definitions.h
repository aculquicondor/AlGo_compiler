#ifndef ALGO_DEFINITIONS_H
#define ALGO_DEFINITIONS_H


class Token {
public:
    enum _token_id {
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
        DECL_ASSIGN,
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

    Token(_token_id id = NONE) : _id(id) { }

    Token &operator=(_token_id id) {
        this->_id = id;
        return *this;
    }

    bool operator==(_token_id id) {
        return this->_id == id;
    }

    bool operator!=(_token_id id) {
        return this->_id != id;
    }

    int id() const {
        return _id;
    }

private:
    int _id;
};


class SyntaxSymbol : public Token {

};

#endif //ALGO_DEFINITIONS_H
