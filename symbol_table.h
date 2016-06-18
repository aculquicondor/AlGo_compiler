#ifndef ALGO_SYMBOL_TABLE_H
#define ALGO_SYMBOL_TABLE_H

#include <set>
#include <stack>
#include <unordered_map>
#include <vector>

#include "definitions.h"


struct SymbolTableRecord {
    explicit SymbolTableRecord(Type type=Type::VOID) : type(type) { }

    struct Param {
        Type type;
        std::vector<std::size_t> dimension;
    };

    bool _const;
    Type type;
    long int_value;
    double float_value;
    char rune_value;
    std::string str_value;
    std::size_t address;
    std::vector<std::size_t> dimension;
    std::vector<Param> params;
};


class SymbolTable {
public:
    explicit SymbolTable();

    bool add_symbol(const std::string &symbol);

    bool has_symbol(const std::string &symbol);

    SymbolTableRecord get_record(const std::string &symbol);

    void start_scope();

    void end_scope();

private:
    std::unordered_map<std::string, std::stack<SymbolTableRecord>> table;
    std::stack<std::set<std::string>> scopes;
};

#endif //ALGO_SYMBOL_TABLE_H
