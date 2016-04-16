#ifndef ALGO_LEXICAL_ANALYZER_H
#define ALGO_LEXICAL_ANALYZER_H

#include <algorithm>
#include <exception>
#include <map>
#include <vector>
#include <sstream>

#include "source_code.h"
#include "lexical_descriptor.h"


class LexicalAnalyzer {
public:
    LexicalAnalyzer(SourceCode *source_code);
    LexicalDescriptor next();

private:
    void next_char();
    LexicalDescriptor _identifier();
    LexicalDescriptor _string();
    LexicalDescriptor _raw_string();
    LexicalDescriptor _number(bool dot_start);
    LexicalDescriptor _operator();
    LexicalDescriptor _line_comment();
    LexicalDescriptor _block_comment();

    bool _is_decimal(char c);
    bool _is_octal(char c);
    bool _is_hexadecimal(char c);
    bool _is_alpha(char c);
    bool _is_alpha_num(char c);
    bool _is_white_space(char c);
    bool _is_printable(char c);

    static const std::vector<char> escaped_chars;
    static const std::map<std::string, Token> reserved_words;
    static const std::map<char, Token> single_char_token;
    static const std::map<char, Token> operator_start;
    static const std::map<char, Token> operator_equal;

    SourceCode *source_code;
    std::size_t line_no;
    char curr_char;
    bool finished;
    std::string curr_lexeme;
};


class LexicalAnalyzerException : public std::exception {
public:
    LexicalAnalyzerException(const std::string &lexeme, std::size_t line_no);
    virtual const char *what() const throw();

    const std::string &lexeme() const;
    std::size_t line_no() const;

private:
    std::string _lexeme;
    std::size_t _line_no;
    std::string msg;
};


#endif //ALGO_LEXICAL_ANALYZER_H
