
// These data structures assume a little-endian system with 16-bit words 

// Specifies tags for basic types (everthing with no high bit is a Node reference)
#define TYPE_SYMBOL   0x8000
#define TYPE_INT      0x8001
#define TYPE_BOOL     0x8002

#define MASK_ANYBASIC 0x8000

// basic values are tagged with their 16-bit type and contain 16 bit of information  
// which can be either signed or unsigned
typedef struct 
{
    union {
        int s;
        unsigned int u;
    } value;
    unsigned int type;     
} BasicValue;

// Values are eighter references of Cells or basic values.
// Distinction is made by looking at the highest bit of of the 
// basic type. If this is cleared, it means it is a reference to a cell.
typedef union 
{
    BasicValue basic;	
    struct Cell* cell;
} Value;

// Cells (a.k.a "cons cells") have two values inside
typedef struct Cell
{
    Value car;
    Value cdr;
} Cell;


// API accessing data management 

// create a new cons cell
Cell* create_cell(Value car, Value cdr);

// look up/create a symbol in the global table
unsigned int get_symbol(unsigned char* identifier);
unsigned char* get_identifier(unsigned int symbol);

// convenience methods to create values
Value create_cell_v(Value car, Value cdr);
Value get_nil_v(void);
Value get_symbol_v(unsigned char* identifier);
Value get_int_v(int i);
Value get_bool_v(int i);
