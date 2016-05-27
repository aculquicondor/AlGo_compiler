#ifndef ALGO_LEXICAL_DESCRIPTOR_H
#define ALGO_LEXICAL_DESCRIPTOR_H

#include <string>

#include "definitions.h"


class LexicalDescriptor {
public:
    LexicalDescriptor();

    LexicalDescriptor(Token token, const std::string &lexeme, size_t line);

    virtual ~LexicalDescriptor();

    Token token() const;

    const std::string &lexeme() const;

    std::size_t line() const;

private:
    Token _token;
    std::string _lexeme;
    std::size_t _line;
};

#endif //ALGO_LEXICAL_DESCRIPTOR_H
