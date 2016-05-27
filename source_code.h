#ifndef ALGO_SOURCE_CODE_H
#define ALGO_SOURCE_CODE_H

#include <fstream>
#include <string>


class SourceCode {
public:
    SourceCode(std::string filename);

    virtual ~SourceCode();

    bool get(char &c);

private:
    std::ifstream input;
    char buffer[8192];
    std::size_t buffer_position;
    std::streamsize buffer_length;
    bool finished;
};


#endif //ALGO_SOURCE_CODE_H
