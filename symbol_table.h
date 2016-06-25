#ifndef ALGO_SYMBOL_TABLE_H
#define ALGO_SYMBOL_TABLE_H

#include <set>
#include <stack>
#include <unordered_map>
#include <vector>

#include "definitions.h"


struct SymbolTableRecord {
    explicit SymbolTableRecord(Type type=Type::VOID) :
            type_dim(TypeDim{type}) {
    }

    struct TypeDim {
        Type type;
        std::vector<std::size_t> dimension;
    };

    bool is_const;
    bool is_function;
    TypeDim type_dim;
    bool bool_value;
    long int_value;
    double float_value;
    char rune_value;
    std::string str_value;
    std::vector<TypeDim> params;
};


class SymbolTable {
public:
    explicit SymbolTable();

    bool add_symbol(const std::string &symbol);

    bool has_symbol(const std::string &symbol);

    SymbolTableRecord &get_record(const std::string &symbol);

    void start_scope();

    void end_scope();

private:
    std::unordered_map<std::string, std::stack<SymbolTableRecord>> table;
    std::stack<std::set<std::string>> scopes;
};

#endif //ALGO_SYMBOL_TABLE_H
