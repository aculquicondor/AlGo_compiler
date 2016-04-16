#include "source_code.h"


SourceCode::SourceCode(std::string filename) :
        input(filename), buffer_position(0), buffer_length(0), finished(false) { }


bool SourceCode::get(char &c) {
    if (buffer_position == buffer_length) {
        if (finished)
            return false;
        input.read(buffer, sizeof buffer);
        buffer_length = input.gcount();
        if (buffer_length == 0)
            return false;
        if (buffer_length < sizeof buffer)
            finished = true;
        buffer_position = 0;
    }
    c = buffer[buffer_position++];
    return true;
}
