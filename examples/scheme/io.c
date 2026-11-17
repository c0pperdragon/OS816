#include <stdio.h>
#include "data.h"
#include "io.h"

Value readvalue()
{
    return get_nil_v();
}

void writevalue(Value v)
{
    Cell* c;
    switch (v.basic.type)
    {
        case TYPE_SYMBOL: 
            printf("%s",(get_identifier(v.basic.value.u)));
            break;
        case TYPE_INT:
            printf("%d", v.basic.value.s);
            break;
        case TYPE_BOOL:
            printf(v.basic.value.s ? "#t" : "#f");
            break;
        default:
            c = v.cell;
            printf("(");
            if (c) // list has at least one element
            {   
                for (;;) // go throught the list
                {
                    writevalue(c->car);
                    // special handling of improper list
                    if (c->cdr.basic.type & MASK_ANYBASIC)
                    {
                        printf(" . ");
                        writevalue(c->cdr);
                        break;
                    }
                    c = c->cdr.cell;
                    if (c) { printf(" "); }
                    else { break; }
                }
            }
            printf(")");
            break;
    }
}

