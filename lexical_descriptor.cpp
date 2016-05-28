#include "lexical_descriptor.h"


LexicalDescriptor::LexicalDescriptor() : token() { }


LexicalDescriptor::LexicalDescriptor(Token token, const std::string &lexeme, size_t line) :
        token(token), lexeme(lexeme), line(line) { }


LexicalDescriptor::~LexicalDescriptor() { }


Token LexicalDescriptor::get_token() const {
    return token;
}


const std::string &LexicalDescriptor::get_lexeme() const {
    return lexeme;
}


size_t LexicalDescriptor::get_line_no() const {
    return line;
}
