#include "lexical_analyzer.h"

LexicalAnalyzer::LexicalAnalyzer(SourceCode *source_code) :
        source_code(source_code), line_no(1) {
    finished = not source_code->get(curr_char);
}


LexicalAnalyzer::~LexicalAnalyzer() { }


LexicalDescriptor LexicalAnalyzer::next() {
    while (not finished and _is_white_space(curr_char)) {
        if (curr_char == '\n')
            line_no++;
        next_char();
    }
    if (finished)
        return {};

    curr_lexeme = std::string(1, curr_char);

    auto single_char = single_char_token.find(curr_char);

    if (single_char != single_char_token.end()) {
        next_char();
        return {single_char->second, curr_lexeme, line_no};
    } else if (_is_alpha(curr_char)) {
        return _identifier();
    } else if (_is_decimal(curr_char)) {
        return _number(false);
    } else if (curr_char == '\'') {
        return _rune();
    } else if (curr_char == '"') {
        return _string();
    } else if (curr_char == '`') {
        return _raw_string();
    } else if (curr_char == '.') {
        next_char();
        if (finished or not _is_decimal(curr_char))
            return {Token::DOT, curr_lexeme, line_no};
        else {
            curr_lexeme.push_back(curr_char);
            return _number(true);
        }
    } /*else if (curr_char == ':') {
        next_char();
        if (finished or curr_char != '=')
            throw LexicalError(curr_lexeme, line_no);
        curr_lexeme.push_back(curr_char);
        next_char();
        return {Token::DECL_ASSIGN, curr_lexeme, line_no};
    } */else if (operator_start.find(curr_char) != operator_start.end()) {
        return _operator();
    }
    else {
        throw LexicalError(curr_lexeme, line_no);
    }
}


void LexicalAnalyzer::next_char() {
    finished = not source_code->get(curr_char);
}


LexicalDescriptor LexicalAnalyzer::_identifier() {
    next_char();
    while (not finished and _is_alpha_num(curr_char)) {
        curr_lexeme.push_back(curr_char);
        next_char();
    }
    auto reserved_word = reserved_words.find(curr_lexeme);
    if (reserved_word != reserved_words.end())
        return LexicalDescriptor(reserved_word->second, curr_lexeme, line_no);
    return {Token::IDENT, curr_lexeme, line_no};
}

LexicalDescriptor LexicalAnalyzer::_rune() {
    next_char();
    if (finished or not _is_printable(curr_char))
        throw LexicalError(curr_lexeme, line_no);
    curr_lexeme.push_back(curr_char);
    if (curr_char == '\\') {
        next_char();
        if (finished)
            throw LexicalError(curr_lexeme, line_no);
        curr_lexeme.push_back(curr_char);
        if (not std::binary_search(escaped_chars.begin(), escaped_chars.end(), curr_char))
            throw LexicalError(curr_lexeme, line_no);
    }
    next_char();
    if (finished or curr_char != '\'')
        throw LexicalError(curr_lexeme, line_no);
    curr_lexeme.push_back(curr_char);
    next_char();
    return {Token::RUNE, curr_lexeme, line_no};
}

LexicalDescriptor LexicalAnalyzer::_string() {
    next_char();
    while (not finished and _is_printable(curr_char) and curr_char != '"') {
        curr_lexeme.push_back(curr_char);
        if (curr_char == '\\') {
            next_char();
            if (finished)
                throw LexicalError(curr_lexeme, line_no);
            curr_lexeme.push_back(curr_char);
            if (not std::binary_search(escaped_chars.begin(), escaped_chars.end(), curr_char))
                throw LexicalError(curr_lexeme, line_no);
        }
        next_char();
    }
    if (finished or curr_char != '"')
        throw LexicalError(curr_lexeme, line_no);
    curr_lexeme.push_back(curr_char);
    next_char();
    return {Token::STRING, curr_lexeme, line_no};
}


LexicalDescriptor LexicalAnalyzer::_raw_string() {
    next_char();
    while (not finished and curr_char != '`') {
        if (curr_char == '\n')
            ++line_no;
        curr_lexeme.push_back(curr_char);
        next_char();
    }
    if (finished or curr_char != '`')
        throw LexicalError(curr_lexeme, line_no);
    curr_lexeme.push_back(curr_char);
    next_char();
    return {Token::R_STRING, curr_lexeme, line_no};
}


LexicalDescriptor LexicalAnalyzer::_number(bool dot_start) {
    bool dot = dot_start, exp_part = false;
    if (not dot_start) {
        if (curr_char == '0') {
            next_char();
            if (not finished and (curr_char == 'x' or curr_char == 'X')) {
                curr_lexeme.push_back(curr_char);
                next_char();
                while (not finished and _is_hexadecimal(curr_char)) {
                    curr_lexeme.push_back(curr_char);
                    next_char();
                }
                if (curr_lexeme.size() == 2)
                    throw LexicalError(curr_lexeme, line_no);
                return {Token::HEXADEC, curr_lexeme, line_no};
            }
        } else {
            next_char();
        }
        while (not finished and _is_decimal(curr_char)) {
            curr_lexeme.push_back(curr_char);
            next_char();
        }

        if (not finished and curr_char == '.') {
            dot = true;
            curr_lexeme.push_back(curr_char);
            next_char();
            while (not finished and _is_decimal(curr_char)) {
                curr_lexeme.push_back(curr_char);
                next_char();
            }
        }
    } else {
        while (not finished and _is_decimal(curr_char)) {
            curr_lexeme.push_back(curr_char);
            next_char();
        }
    }

    if (not finished and (curr_char == 'e' or curr_char == 'E')) {
        exp_part = true;
        curr_lexeme.push_back(curr_char);
        next_char();
        if (finished or _is_white_space(curr_char))
            throw LexicalError(curr_lexeme, line_no);
        curr_lexeme.push_back(curr_char);
        if (curr_char == '+' or curr_char == '-')
            next_char();
        bool seen_number = false;
        while (not finished and _is_decimal(curr_char)) {
            curr_lexeme.push_back(curr_char);
            next_char();
            seen_number = true;
        }
        if (not seen_number)
            throw LexicalError(curr_lexeme, line_no);
    }

    bool extra_characters = false;
    while (not finished and (_is_alpha_num(curr_char) or curr_char == '.')) {
        curr_lexeme.push_back(curr_char);
        extra_characters = true;
        next_char();
    }
    if (extra_characters)
        throw LexicalError(curr_lexeme, line_no);

    if (not dot and not exp_part) {
        if (curr_lexeme[0] == '0') {
            for (char c : curr_lexeme)
                if (not _is_octal(c))
                    throw LexicalError(curr_lexeme, line_no);
            return {Token::OCTAL, curr_lexeme, line_no};
        }
        return {Token::DEC, curr_lexeme, line_no};
    }

    return {Token::FLOAT, curr_lexeme, line_no};
}


LexicalDescriptor LexicalAnalyzer::_operator() {
    next_char();
    if (not finished) {
        if (curr_lexeme[0] == '/') {
            if (curr_char == '/') {
                return _line_comment();
            }
            if (curr_char == '*') {
                curr_lexeme.push_back(curr_char);
                return _block_comment();
            }
        }
        if (curr_lexeme[0] == '+' and curr_char == '+') {
            curr_lexeme.push_back(curr_char);
            next_char();
            return {Token::INCR, curr_lexeme, line_no};
        }
        if (curr_lexeme[0] == '-' and curr_char == '-') {
            curr_lexeme.push_back(curr_char);
            next_char();
            return {Token::DECR, curr_lexeme, line_no};
        }
        if (curr_lexeme[0] == '&' and curr_char == '&') {
            curr_lexeme.push_back(curr_char);
            next_char();
            return {Token::AND, curr_lexeme, line_no};
        }
        if (curr_lexeme[0] == '|' and curr_char == '|') {
            curr_lexeme.push_back(curr_char);
            next_char();
            return {Token::OR, curr_lexeme, line_no};
        }
        if ((curr_lexeme[0] == '<' or curr_lexeme[0] == '>') and curr_char == curr_lexeme[0]) {
            curr_lexeme.push_back(curr_char);
            next_char();
            if (not finished and curr_char == '=') {
                curr_lexeme.push_back(curr_char);
                next_char();
                return {curr_lexeme[0] == '<' ? Token::A_L_SHIFT : Token::A_R_SHIFT, curr_lexeme, line_no};
            }
            return {curr_lexeme[0] == '<' ? Token::L_SHIFT : Token::R_SHIFT, curr_lexeme, line_no};
        }
        if (curr_lexeme[0] == '&' and curr_char == '^') {
            curr_lexeme.push_back(curr_char);
            next_char();
            if (not finished and curr_char == '=') {
                curr_lexeme.push_back(curr_char);
                next_char();
                return {Token::A_BW_AND_NOT, curr_lexeme, line_no};
            }
            return {Token::BW_AND_NOT, curr_lexeme, line_no};
        }
    }
    if (curr_char == '=') {
        curr_lexeme.push_back(curr_char);
        next_char();
        return {operator_equal.find(curr_lexeme[0])->second, curr_lexeme, line_no};
    }
    return {operator_start.find(curr_lexeme[0])->second, curr_lexeme, line_no};
}


LexicalDescriptor LexicalAnalyzer::_line_comment() {
    next_char();
    while (not finished and curr_char != '\n')
        next_char();
    return next();
}


LexicalDescriptor LexicalAnalyzer::_block_comment() {
    next_char();
    bool star = false;
    while (not finished) {
        if (star and curr_char == '/') {
            next_char();
            return next();
        }
        star = curr_char == '*';
        curr_lexeme.push_back(curr_char);
        if (curr_char == '\n')
            ++line_no;
        next_char();
    }
    throw LexicalError(curr_lexeme, line_no);
}


bool LexicalAnalyzer::_is_decimal(char c) {
    return c >= '0' and c <= '9';
}


bool LexicalAnalyzer::_is_octal(char c) {
    return c >= '0' and c < '8';
}


bool LexicalAnalyzer::_is_hexadecimal(char c) {
    return (c >= '0' and c < '9') or
           (c >= 'a' and c <= 'f') or
           (c >= 'A' and c <= 'F');
}


bool LexicalAnalyzer::_is_alpha(char c) {
    return (c >= 'A' and c <= 'Z') or
           (c >= 'a' and c <= 'z') or
           c == '_';
}


bool LexicalAnalyzer::_is_alpha_num(char c) {
    return _is_decimal(c) or _is_alpha(c);
}


bool LexicalAnalyzer::_is_white_space(char c) {
    return c == ' ' or c == '\t' or c == '\t' or c == '\n';
}


bool LexicalAnalyzer::_is_printable(char c) {
    return c >= 32 and c <= 126;
}


const std::vector<char> LexicalAnalyzer::escaped_chars = {'"', '\'', '\\', 'n', 't'};


const std::map<std::string, Token> LexicalAnalyzer::reserved_words = {
        {"const",    Token::CONST},
        {"var",      Token::VAR},
        {"for",      Token::FOR},
        {"if",       Token::IF},
        {"else",     Token::ELSE},
        {"break",    Token::BREAK},
        {"continue", Token::CONTINUE},
        {"return",   Token::RETURN},
        {"func",     Token::FUNC},
        {"package",  Token::PKG},
        {"import",   Token::IMP},
        {"true",     Token::TRUE},
        {"false",    Token::FALSE},
        {"bool",     Token::BOOL},
        {"int",      Token::INT},
        {"int32",    Token::I32},
        {"int64",    Token::I64},
        {"uint",     Token::UINT},
        {"uint32",   Token::UI32},
        {"uint64",   Token::UI64},
        {"float32",  Token::FL32},
        {"float64",  Token::FL64},
        {"rune",     Token::RN},
        {"string",   Token::STR}
};


const std::map<char, Token> LexicalAnalyzer::single_char_token = {
        {',', Token::COL},
        {';', Token::SEMICOL},
        {'(', Token::O_PAREN},
        {')', Token::C_PAREN},
        {'{', Token::O_BRACK},
        {'}', Token::C_BRACK},
        {'[', Token::O_SQBRACK},
        {']', Token::C_SQBRACK}
};


const std::map<char, Token> LexicalAnalyzer::operator_start = {
        {'!', Token::NOT},
        {'%', Token::MOD},
        {'&', Token::BW_AND},
        {'*', Token::TIMES},
        {'+', Token::PLUS},
        {'-', Token::MINUS},
        {'/', Token::DIV},
        {'<', Token::LT},
        {'=', Token::ASSIGN},
        {'>', Token::GT},
        {'^', Token::BW_XOR_NEG},
        {'|', Token::BW_OR}
};


const std::map<char, Token> LexicalAnalyzer::operator_equal = {
        {'!', Token::NEQ},
        {'%', Token::A_MOD},
        {'&', Token::A_BW_AND},
        {'*', Token::A_TIMES},
        {'+', Token::A_PLUS},
        {'-', Token::A_MINUS},
        {'/', Token::A_DIV},
        {'<', Token::LTE},
        {'=', Token::EQ},
        {'>', Token::GTE},
        {'^', Token::A_BW_XOR},
        {'|', Token::A_BW_OR}
};


LexicalError::LexicalError(const std::string &lexeme, std::size_t line_no) :
        lexeme(lexeme), line_no(line_no) {
    std::stringstream ss;
    ss << "Unknown lexeme \"" << lexeme << "\"" << "at line " << line_no << ".";
    msg = ss.str();
}


LexicalError::~LexicalError() { }
