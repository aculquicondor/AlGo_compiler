#ifndef ALGO_SEMANTIC_RULES_H
#define ALGO_SEMANTIC_RULES_H

#include <exception>
#include <sstream>
#include <string>


class SemanticError : public std::exception {
public:
    SemanticError(std::string error, std::size_t line_no) : line_no(line_no) {
        std::stringstream ss;
        ss << error << " at line " << line_no;
        msg = ss.str();
    }

    virtual ~SemanticError() {
    }

    virtual const char *what() const throw() {
        return msg.c_str();
    }

    std::size_t get_line_no() const {
        return line_no;
    }
private:
    std::string msg;
    std::size_t line_no;
};

#endif //ALGO_SEMANTIC_RULES_H
