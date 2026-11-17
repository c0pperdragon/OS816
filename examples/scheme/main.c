// main loop to read, evaluate, print

#include <stdio.h>
#include <stdlib.h>
#include "data.h"
#include "io.h"


int main(int arc, char** argv)
{
    Value v = 
    create_cell_v(get_symbol_v("HELLO"),
      create_cell_v(get_symbol_v("WORLD"),
        create_cell_v(get_symbol_v("!"),
          create_cell_v(get_int_v(4711),
            create_cell_v(get_bool_v(0),
              create_cell_v(create_cell_v(get_symbol_v("x"),get_symbol_v("y")),
                get_nil_v()
              )
            )
          )
        )
      )
    );
    writevalue(v);
    printf("\n");
    return 0;
}
