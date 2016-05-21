#include "lexical_analyzer.h"

namespace algo {

    LexicalAnalyzer::LexicalAnalyzer(SourceCode *source_code) :
            source_code(source_code), line_no(1) {
        finished = not source_code->get(curr_char);
    }


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
                return {DOT, curr_lexeme, line_no};
            else {
                curr_lexeme.push_back(curr_char);
                return _number(true);
            }
        } else if (curr_char == ':') {
            next_char();
            if (finished or curr_char != '=')
                throw LexicalAnalyzerException(curr_lexeme, line_no);
            curr_lexeme.push_back(curr_char);
            next_char();
            return {DECL_ASSIGN, curr_lexeme, line_no};
        } else if (operator_start.find(curr_char) != operator_start.end()) {
            return _operator();
        }
        else {
            throw LexicalAnalyzerException(curr_lexeme, line_no);
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
        return {IDENT, curr_lexeme, line_no};
    }

    LexicalDescriptor LexicalAnalyzer::_rune() {
        next_char();
        if (finished or not _is_printable(curr_char))
            throw LexicalAnalyzerException(curr_lexeme, line_no);
        curr_lexeme.push_back(curr_char);
        if (curr_char == '\\') {
            next_char();
            if (finished)
                throw LexicalAnalyzerException(curr_lexeme, line_no);
            curr_lexeme.push_back(curr_char);
            if (not std::binary_search(escaped_chars.begin(), escaped_chars.end(), curr_char))
                throw LexicalAnalyzerException(curr_lexeme, line_no);
        }
        next_char();
        if (finished or curr_char != '\'')
            throw LexicalAnalyzerException(curr_lexeme, line_no);
        curr_lexeme.push_back(curr_char);
        next_char();
        return {RUNE, curr_lexeme, line_no};
    }

    LexicalDescriptor LexicalAnalyzer::_string() {
        next_char();
        while (not finished and _is_printable(curr_char) and curr_char != '"') {
            curr_lexeme.push_back(curr_char);
            if (curr_char == '\\') {
                next_char();
                if (finished)
                    throw LexicalAnalyzerException(curr_lexeme, line_no);
                curr_lexeme.push_back(curr_char);
                if (not std::binary_search(escaped_chars.begin(), escaped_chars.end(), curr_char))
                    throw LexicalAnalyzerException(curr_lexeme, line_no);
            }
            next_char();
        }
        if (finished or curr_char != '"')
            throw LexicalAnalyzerException(curr_lexeme, line_no);
        curr_lexeme.push_back(curr_char);
        next_char();
        return {STRING, curr_lexeme, line_no};
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
            throw LexicalAnalyzerException(curr_lexeme, line_no);
        curr_lexeme.push_back(curr_char);
        next_char();
        return {R_STRING, curr_lexeme, line_no};
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
                        throw LexicalAnalyzerException(curr_lexeme, line_no);
                    return {HEXADEC, curr_lexeme, line_no};
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
                throw LexicalAnalyzerException(curr_lexeme, line_no);
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
                throw LexicalAnalyzerException(curr_lexeme, line_no);
        }

        if (not finished and (_is_alpha_num(curr_char) or curr_char == '.')) {
            curr_lexeme.push_back(curr_char);
            throw LexicalAnalyzerException(curr_lexeme, line_no);
        }

        if (not dot and not exp_part) {
            if (curr_lexeme[0] == '0') {
                for (char c : curr_lexeme)
                    if (not _is_octal(c))
                        throw LexicalAnalyzerException(curr_lexeme, line_no);
                return {OCTAL, curr_lexeme, line_no};
            }
            return {DEC, curr_lexeme, line_no};
        }

        return {FLOAT, curr_lexeme, line_no};
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
                return {INCR, curr_lexeme, line_no};
            }
            if (curr_lexeme[0] == '-' and curr_char == '-') {
                curr_lexeme.push_back(curr_char);
                next_char();
                return {DECR, curr_lexeme, line_no};
            }
            if (curr_lexeme[0] == '&' and curr_char == '&') {
                curr_lexeme.push_back(curr_char);
                next_char();
                return {AND, curr_lexeme, line_no};
            }
            if (curr_lexeme[0] == '|' and curr_char == '|') {
                curr_lexeme.push_back(curr_char);
                next_char();
                return {OR, curr_lexeme, line_no};
            }
            if ((curr_lexeme[0] == '<' or curr_lexeme[0] == '>') and curr_char == curr_lexeme[0]) {
                curr_lexeme.push_back(curr_char);
                next_char();
                if (not finished and curr_char == '=') {
                    curr_lexeme.push_back(curr_char);
                    next_char();
                    return {curr_lexeme[0] == '<' ? A_L_SHIFT : A_R_SHIFT, curr_lexeme, line_no};
                }
                return {curr_lexeme[0] == '<' ? L_SHIFT : R_SHIFT, curr_lexeme, line_no};
            }
            if (curr_lexeme[0] == '&' and curr_char == '^') {
                curr_lexeme.push_back(curr_char);
                next_char();
                if (not finished and curr_char == '=') {
                    curr_lexeme.push_back(curr_char);
                    next_char();
                    return {A_BW_AND_NOT, curr_lexeme, line_no};
                }
                return {BW_AND_NOT, curr_lexeme, line_no};
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
        throw LexicalAnalyzerException(curr_lexeme, line_no);
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
            {"const",    CONST},
            {"var",      VAR},
            {"for",      FOR},
            {"if",       IF},
            {"else",     ELSE},
            {"break",    BREAK},
            {"continue", CONTINUE},
            {"return",   RETURN},
            {"func",     FUNC},
            {"package",  PKG},
            {"import",   IMP},
            {"true",     TRUE},
            {"false",    FALSE},
            {"bool",     BOOL},
            {"int",      INT},
            {"int32",    I32},
            {"int64",    I64},
            {"uint",     UINT},
            {"uint32",   UI32},
            {"uint64",   UI64},
            {"float32",  FL32},
            {"float64",  FL64},
            {"rune",     RN},
            {"string",   STR}
    };


    const std::map<char, Token> LexicalAnalyzer::single_char_token = {
            {',', COLON},
            {';', SEMICOLON},
            {'(', O_PAREN},
            {')', C_PAREN},
            {'{', O_BRACK},
            {'}', C_BRACK},
            {'[', O_SQBRACK},
            {']', C_SQBRACK}
    };


    const std::map<char, Token> LexicalAnalyzer::operator_start = {
            {'!', NOT},
            {'%', MOD},
            {'&', BW_AND},
            {'*', TIMES},
            {'+', PLUS},
            {'-', MINUS},
            {'/', DIV},
            {'<', LT},
            {'=', ASSIGN},
            {'>', GT},
            {'^', BW_XOR_NEG},
            {'|', BW_OR}
    };


    const std::map<char, Token> LexicalAnalyzer::operator_equal = {
            {'!', NEQ},
            {'%', A_MOD},
            {'&', A_BW_AND},
            {'*', A_TIMES},
            {'+', A_PLUS},
            {'-', A_MINUS},
            {'/', A_DIV},
            {'<', LTE},
            {'=', EQ},
            {'>', GTE},
            {'^', A_BW_XOR_NEG},
            {'|', A_BW_OR}
    };


    LexicalAnalyzerException::LexicalAnalyzerException(const std::string &lexeme, std::size_t line_no) :
            _lexeme(lexeme), _line_no(line_no) {
        std::stringstream ss;
        ss << "Unknown lexeme \"" << lexeme << "\"" << "at line " << line_no << ".";
        msg = ss.str();
    }


    const char *LexicalAnalyzerException::what() const throw() {
        return msg.c_str();
    }


    const std::string &LexicalAnalyzerException::lexeme() const {
        return _lexeme;
    }

    std::size_t LexicalAnalyzerException::line_no() const {
        return _line_no;
    }

}
