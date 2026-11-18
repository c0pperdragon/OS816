#include <stdlib.h>
#include <string.h>
#include "data.h"

// singular basic values
Value Nothing = {0, TYPE_NOTHING}; 
Value Empty = {0, TYPE_EMPTY}; 
Value False = {0, TYPE_BOOL};
Value True = {1, TYPE_BOOL};


// non-collecting allocator. simple as can be, but will run out of memory eventually.
Cell *cellbufferptr = NULL;
unsigned int cellbufferremaining = 0;

Cell* create_cell(Value car, Value cdr)
{
    Cell* c;
    if (cellbufferremaining<1)
    {
        cellbufferremaining = 128;
        cellbufferptr = (Cell*) malloc(128 * sizeof(Cell));
        if (!cellbufferptr) { exit(0); }    
    }
    c = cellbufferptr++;
    cellbufferremaining--;
    c->car = car;
    c->cdr = cdr;
    return c;
}

// symbol table manager - extra simple to begin with
typedef struct SymbolEntry 
{
    struct SymbolEntry* next;
    unsigned char identifier[1];
} SymbolEntry;
SymbolEntry* symboltable = NULL;

unsigned int get_symbol(unsigned char* identifier)
{
    SymbolEntry* last = NULL;
    SymbolEntry* n = symboltable;
    unsigned int idx = 0;
    while (n)
    {
        if (strcmp(identifier, n->identifier)==0) { return idx; }
        last = n;
        n = n->next;
        idx++;
    }
    n = (SymbolEntry*) malloc(sizeof(SymbolEntry) + strlen(identifier));
    n->next = NULL;
    strcpy(n->identifier, identifier);
    if (last) { last->next = n; }
    else { symboltable = n; }
    return idx;
}
unsigned char* get_identifier(unsigned int symbol)
{
    SymbolEntry* n = symboltable;
    while (symbol) 
    { 
        symbol--;
        n = n->next;
    }
    return n->identifier;
}

Value create_cell_v(Value car, Value cdr)
{
    Value v;
    v.cell = create_cell(car, cdr);
    return v;
}
Value get_symbol_v(unsigned char* identifier)
{
    Value v;
    v.basic.type = TYPE_SYMBOL;
    v.basic.value.u = get_symbol(identifier);
    return v;
}
Value get_int_v(int i)
{
    Value v;
    v.basic.type = TYPE_INT;
    v.basic.value.s = i;
    return v;
}
