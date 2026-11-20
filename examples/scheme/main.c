// main loop to read, evaluate, print

#include <stdio.h>
#include <stdlib.h>
#include "data.h"
#include "io.h"
#include "evaluate.h"

int main(int arc, char** argv)
{
    puts("Small scheme interpreter");
    initialize_symbols();
            
    receiveline(']',0);
    for (;;)
    {
        Value v = readvalue();
        v = evaluate(v);
        if (v.basic.type != TYPE_NOTHING)
        {
            writevalue(v);
            putchar('\n');
        }
        receiveline(']',1);
    }
    return 0;
}
