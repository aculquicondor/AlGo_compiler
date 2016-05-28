#include <iostream>

#include "lexical_analyzer.h"
#include "syntax_analyzer.h"


using namespace std;


int main(int argc, char *argv[]) {
    SourceCode src(argv[1]);
    LexicalAnalyzer lex(&src);
    SyntaxAnalyzer syntax(&lex);

    cout << syntax.analyze() << endl;

    return 0;
}