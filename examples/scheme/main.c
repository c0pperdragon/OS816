// main loop to read, evaluate, print

#include <stdio.h>
#include <stdlib.h>
#include "data.h"
#include "io.h"


int main(int arc, char** argv)
{
    puts("Small scheme interpreter");
    receiveline(']',0);
    for (;;)
    {
        Value v = readvalue();
        if (v.basic.type != TYPE_NOTHING)
        {
            writevalue(v);
            putchar('\n');
        }
        receiveline(']',1);
    }
    return 0;
}
