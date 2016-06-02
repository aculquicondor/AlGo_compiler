#include "definitions.h"


const int SyntaxSymbol::FIRST_NT = PACKAGE;


const std::map<std::string, int> SyntaxSymbol::str_to_value{
        {"NONE", NONE},
        {"COL", COL},
        {"DOT", DOT},
        {"SEMICOL", SEMICOL},
        {"O_PAREN", O_PAREN},
        {"C_PAREN", C_PAREN},
        {"O_BRACK", O_BRACK},
        {"C_BRACK", C_BRACK},
        {"O_SQBRACK", O_SQBRACK},
        {"C_SQBRACK", C_SQBRACK},
        {"PLUS", PLUS},
        {"MINUS", MINUS},
        {"TIMES", TIMES},
        {"DIV", DIV},
        {"MOD", MOD},
        {"BW_AND", BW_AND},
        {"BW_AND_NOT", BW_AND_NOT},
        {"BW_OR", BW_OR},
        {"BW_XOR_NEG", BW_XOR_NEG},
        {"L_SHIFT", L_SHIFT},
        {"R_SHIFT", R_SHIFT},
        {"A_PLUS", A_PLUS},
        {"A_MINUS", A_MINUS},
        {"A_TIMES", A_TIMES},
        {"A_DIV", A_DIV},
        {"A_MOD", A_MOD},
        {"A_BW_AND", A_BW_AND},
        {"A_BW_AND_NOT", A_BW_AND_NOT},
        {"A_BW_OR", A_BW_OR},
        {"A_BW_XOR_NEG", A_BW_XOR_NEG},
        {"A_L_SHIFT", A_L_SHIFT},
        {"A_R_SHIFT", A_R_SHIFT},
        {"INCR", INCR},
        {"DECR", DECR},
        {"ASSIGN", ASSIGN},
        {"EQ", EQ},
        {"NEQ", NEQ},
        {"LT", LT},
        {"GT", GT},
        {"LTE", LTE},
        {"GTE", GTE},
        {"OR", OR},
        {"AND", AND},
        {"NOT", NOT},
        {"IDENT", IDENT},
        {"DEC", DEC},
        {"OCTAL", OCTAL},
        {"HEXADEC", HEXADEC},
        {"FLOAT", FLOAT},
        {"RUNE", RUNE},
        {"STRING", STRING},
        {"R_STRING", R_STRING},
        {"TRUE", TRUE},
        {"FALSE", FALSE},
        {"CONST", CONST},
        {"VAR", VAR},
        {"FOR", FOR},
        {"IF", IF},
        {"ELSE", ELSE},
        {"BREAK", BREAK},
        {"CONTINUE", CONTINUE},
        {"RETURN", RETURN},
        {"FUNC", FUNC},
        {"PKG", PKG},
        {"IMP", IMP},
        {"BOOL", BOOL},
        {"INT", INT},
        {"I32", I32},
        {"I64", I64},
        {"UINT", UINT},
        {"UI32", UI32},
        {"UI64", UI64},
        {"FL32", FL32},
        {"FL64", FL64},
        {"RN", RN},
        {"STR", STR},
        {"PACKAGE", PACKAGE},
        {"IMPORT_DECLS", IMPORT_DECLS},
        {"PKG_DECLS", PKG_DECLS},
        {"CONST_DECL", CONST_DECL},
        {"VAR_DECL", VAR_DECL},
        {"VAR_DECLp", VAR_DECLp},
        {"VAR_DECLpp", VAR_DECLpp},
        {"INT_LIT", INT_LIT},
        {"FUNC_DECL", FUNC_DECL},
        {"FUNC_DECLp", FUNC_DECLp},
        {"PARAM_LIST", PARAM_LIST},
        {"PARAM_LISTp", PARAM_LISTp},
        {"BLOCK", BLOCK},
        {"TYPE", TYPE},
        {"BLOCK_CONTS", BLOCK_CONTS},
        {"BLOCK_UNIT", BLOCK_UNIT},
        {"RETURNp", RETURNp},
        {"EXPR", EXPR},
        {"EXPRp", EXPRp},
        {"ASSIGN_OPER", ASSIGN_OPER},
        {"LV1EXPR", LV1EXPR},
        {"LV1EXPRp", LV1EXPRp},
        {"LV1OPER", LV1OPER},
        {"LV2EXPR", LV2EXPR},
        {"LV2EXPRp", LV2EXPRp},
        {"LV2OPER", LV2OPER},
        {"LV3EXPR", LV3EXPR},
        {"LV3EXPRp", LV3EXPRp},
        {"LV3OPER", LV3OPER},
        {"LV4EXPR", LV4EXPR},
        {"LV4EXPRp", LV4EXPRp},
        {"LV4OPER", LV4OPER},
        {"LV5EXPR", LV5EXPR},
        {"LV5EXPRp", LV5EXPRp},
        {"LV5OPER", LV5OPER},
        {"UNARYOPER", UNARYOPER},
        {"TERM", TERM},
        {"IDCR", IDCR},
        {"CAST_ARR", CAST_ARR},
        {"LIST", LIST},
        {"LISTp", LISTp},
        {"ACCESS", ACCESS},
        {"FUNC_CALL", FUNC_CALL},
        {"ARRAY_ACC", ARRAY_ACC},
        {"ARRAY_ACCp", ARRAY_ACCp},
        {"PARAMS", PARAMS},
        {"PARAMSp", PARAMSp},
        {"IF_CONST", IF_CONST},
        {"IF_CONSTp", IF_CONSTp},
        {"ELSEp", ELSEp},
        {"FOR_CONST", FOR_CONST},
        {"FOR_CONSTp", FOR_CONSTp},
        {"FOR_CONSTpp", FOR_CONSTpp}
};
