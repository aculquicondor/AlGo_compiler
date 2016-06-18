#ifndef ALGO_SYMBOL_TABLE_RECORD_H
#define ALGO_SYMBOL_TABLE_RECORD_H

#include <vector>
#include <string>


struct SymbolTableRecord {
    enum Type {
        NONE,
        UINT,
        UINT32,
        UINT64,
        INT,
        INT32,
        INT64,
        FLOAT32,
        FLOAT64,
        RUNE,
        STRING
    };

    explicit SymbolTableRecord(Type type=NONE) : type(type) { }

    struct Param {
        Type type;
        std::vector<std::size_t> dimension;
    };

    Type type;
    long int_value;
    double float_value;
    std::string str_value;
    std::size_t address;
    std::vector<std::size_t> dimension;
    std::vector<Param> params;
};

#endif //ALGO_SYMBOL_TABLE_RECORD_H
