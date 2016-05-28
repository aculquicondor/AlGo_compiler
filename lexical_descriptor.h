#ifndef ALGO_LEXICAL_DESCRIPTOR_H
#define ALGO_LEXICAL_DESCRIPTOR_H

#include <string>

#include "definitions.h"


class LexicalDescriptor {
public:
    LexicalDescriptor();

    LexicalDescriptor(Token token, const std::string &lexeme, size_t line);

    virtual ~LexicalDescriptor();

    const Token &get_token() const;

    const std::string &get_lexeme() const;

    std::size_t get_line_no() const;

private:
    Token token;
    std::string lexeme;
    std::size_t line;
};

#endif //ALGO_LEXICAL_DESCRIPTOR_H
