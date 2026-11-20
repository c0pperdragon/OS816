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

// symbol table manager
typedef struct SymbolEntry 
{
    unsigned char *identifier;  // string pointer (never freed)
    Value value;                // value assigned to the symbol in global environment
} SymbolEntry;
SymbolEntry* symboltable = NULL;
unsigned int symboltablelength = 0;
unsigned int symboltableused = 0;

unsigned int get_symbol(unsigned char* identifier)
{
    unsigned int idx=0;
    for (idx=0; idx<symboltableused; idx++)
    {
        if (strcmp(identifier, symboltable[idx].identifier)==0) { return idx; }
    }
    if (symboltableused>=symboltablelength)
    {
        unsigned int newlength = symboltablelength ? symboltablelength*2 : 32;
        SymbolEntry* newtable = malloc(newlength*sizeof(SymbolEntry));
        if (symboltable) 
        {
            memcpy(newtable,symboltable,symboltablelength*sizeof(SymbolEntry));
            free(symboltable); 
        }
        symboltable = newtable;
        symboltablelength = newlength;
    }
    {
        unsigned char* newi = malloc(strlen(identifier)+1);
        strcpy(newi, identifier);
        idx = symboltableused++;
        symboltable[idx].identifier = newi;
        symboltable[idx].value.basic.type = TYPE_NOTHING;
    }
    return idx;
}

unsigned char* get_identifier(unsigned int symbol)
{
    return symboltable[symbol].identifier;
}

Value get_global_value(unsigned int symbol)
{
    return symboltable[symbol].value;
}

void set_global_value(unsigned int symbol, Value v)
{
    symboltable[symbol].value = v;
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
Value get_builtint_v(unsigned int u)
{
    Value v;
    v.basic.type = TYPE_BUILTIN;
    v.basic.value.u = u;
    return v;
}
