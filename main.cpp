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
            exit(EXIT_FAILURE);
        }
        if (descriptor.token()) {
            cout << descriptor.line() << ": " << descriptor.lexeme() << " (token " <<
                    descriptor.token() << ")\n";
        }
    } while (descriptor.token());

    return 0;
}