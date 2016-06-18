#include "symbol_table.h"


SymbolTable::SymbolTable() {
    scopes.push({});
}


void SymbolTable::start_scope() {
    scopes.push({});
}


void SymbolTable::end_scope() {
    for (std::string symbol : scopes.top()) {
        auto symbol_stack = table[symbol];
        symbol_stack.pop();
        if (symbol_stack.empty())
            table.erase(symbol);
    }
    scopes.pop();
}


bool SymbolTable::has_symbol(const std::string &symbol) {
    return table.find(symbol) != table.end();
}


SymbolTableRecord SymbolTable::get_record(const std::string &symbol) {
    return table[symbol].top();
}


bool SymbolTable::add_symbol(const std::string &symbol) {
    if (scopes.top().find(symbol) != scopes.top().end())
        return false;
    scopes.top().insert(symbol);
    table[symbol].push(SymbolTableRecord{});
    return true;
}

