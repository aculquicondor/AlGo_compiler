#include "lexical_descriptor.h"


LexicalDescriptor::LexicalDescriptor() : _token() { }


LexicalDescriptor::LexicalDescriptor(Token token, const std::string &lexeme, size_t line) :
        _token(token), _lexeme(lexeme), _line(line) { }


LexicalDescriptor::~LexicalDescriptor() { }


Token LexicalDescriptor::token() const {
    return _token;
}


const std::string &LexicalDescriptor::lexeme() const {
    return _lexeme;
}


size_t LexicalDescriptor::line() const {
    return _line;
}
