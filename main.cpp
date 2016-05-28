#include <iostream>

#include "lexical_analyzer.h"
#include "syntax_analyzer.h"


using namespace std;


int main(int argc, char *argv[]) {
    SourceCode src(argv[1]);
    LexicalAnalyzer lex(&src);
    LexicalDescriptor descriptor;
    SyntaxAnalyzer syntax(&lex);

    /*
    do {
        try {
            descriptor = lex.next();
        } catch (LexicalError &exc) {
            cerr << exc.what() << endl;
        }
        if (descriptor.token() != Token::NONE) {
            cout << "[line: " << descriptor.line() <<
                    ", token: " << descriptor.token().get_value() << "] " <<
                    descriptor.lexeme() << endl;
        }
    } while (descriptor.get_token() != Token::NONE);
     */

    return 0;
}