#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data.h"
#include "evaluate.h"

#define CAR(v) (v.cell->car)
#define CDR(v) (v.cell->cdr)
#define CADR(v) (v.cell->cdr.cell->car)
#define CADDR(v) (v.cell->crd.cell->cdr.cell->car)

Value symbol_quote;
Value symbol_define;


typedef Value (*builtinfunction) (unsigned int, Value);

Value builtin_plus(unsigned int numargs, Value args)
{
    if (numargs>=2 && ISINT(CAR(args)) && ISINT(CADR(args)))
    {
        return get_int_v(CAR(args).basic.value.s + CADR(args).basic.value.s);
    }
    return get_int_v(0);
}
Value builtin_minus(unsigned int numargs, Value args)
{
    if (numargs>=2 && ISINT(CAR(args)) && ISINT(CADR(args)))
    {
        return get_int_v(CAR(args).basic.value.s - CADR(args).basic.value.s);
    }
    return get_int_v(0);
}
Value builtin_car(unsigned int numargs, Value args)
{
    if (numargs>=1)
    {
        Value v = CAR(args);
        if (ISLIST(v)) { return CAR(v); }
    }
    return Nothing;
}
Value builtin_cdr(unsigned int numargs, Value args)
{
    if (numargs>=1)
    {
        Value v = CAR(args);
        if (ISLIST(v)) { return CDR(v); }
    }
    return Empty;
}
Value builtin_cons(unsigned int numargs, Value args)
{
    return (numargs>=2) ? create_cell_v(CAR(args),CADR(args)) : Empty;
}
Value builtin_list(unsigned int numargs, Value args)
{
    return args;
}

builtinfunction builtintable[] = 
{
    builtin_plus,
    builtin_minus,
    builtin_car,
    builtin_cdr,
    builtin_cons,
    builtin_list
};

void initialize_symbols(void)
{
    set_global_value(get_symbol("+"), get_builtint_v(0));
    set_global_value(get_symbol("-"), get_builtint_v(1));
    set_global_value(get_symbol("car"), get_builtint_v(2));
    set_global_value(get_symbol("cdr"), get_builtint_v(3));
    set_global_value(get_symbol("cons"), get_builtint_v(4));
    set_global_value(get_symbol("list"), get_builtint_v(5));
    symbol_define = get_symbol_v("define");    
    symbol_quote = get_symbol_v("quote");    
}


Value evaluate(Value v)
{
    // basic types are easy to evaluate
    if (ISBASIC(v))
    {
        if (v.basic.type == TYPE_SYMBOL) 
        { 
            return get_global_value(v.basic.value.u);    
        }
    }
    // evaluate list
    else
    {
        // handle special syntax 
        if (CAR(v) == symbol_define)
        {
            if (ISLIST(CDR(v)) && ISLIST(CDR(CDR(v))) && ISSYMBOL(CAR(CDR(v))))
            {
                set_global_value(CAR(CDR(v)).basic.value.u, evaluate(CAR(CDR(CDR(v)))));
                return Nothing;
            }
        }
        else if (CAR(v) == symbol_quote)
        {
            return ISLIST(CDR(v)) ? CADR(v) : Nothing;
        }
        // other lists will be treated as procedure calls with all parts evaluated first
        else
        {
            Value first = evaluate(CAR(v));
            Value top = create_cell_v(first, Empty);
            int numargs=0;
            Cell* last = top.cell;
            v = CDR(v);
            while (ISLIST(v))
            {
                last->cdr = create_cell_v(evaluate(CAR(v)), Empty);
                last = last->cdr.cell;
                v = CDR(v);
                numargs++;
            }
            // built-in procedure
            if (ISBUILTIN(first)) 
            {
                return builtintable[first.basic.value.u] (numargs, CDR(top)); 
            }
            
            return top;
        }
    }
    // anything else evaluates to itself
    return v;
}
