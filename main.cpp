#include <iostream>

#include "lexical_analyzer.h"


using namespace std;


int main(int argc, char *argv[]) {
    SourceCode src(argv[1]);
    LexicalAnalyzer lex(&src);
    LexicalDescriptor descriptor;

    do {
        try {
            descriptor = lex.next();
        } catch (LexicalAnalyzerException &exc) {
            cerr << exc.what() << endl;
        }
        if (descriptor.token() != Token::NONE) {
            cout << "[line: " << descriptor.line() <<
                    ", token: " << descriptor.token().get_value() << "] " <<
                    descriptor.lexeme() << endl;
        }
    } while (descriptor.token() != Token::NONE);

    return 0;
}