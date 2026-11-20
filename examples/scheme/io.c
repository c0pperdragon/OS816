#include <stdio.h>
#include "data.h"
#include "io.h"

#define BUFFERSIZE 78
unsigned char buffer[BUFFERSIZE];
unsigned int cursor;

void receiveline(unsigned int prompt, int ifempty)
{
    unsigned int len = 0;
    
    if (ifempty && buffer[cursor]) { return; }
    
    cursor = 0;
    buffer[0] = '\0';
    putchar(prompt);
    putchar(' ');
    for (;;)
    {
        unsigned int c = getchar();
        if (c>=32 && c<127 && len+1<BUFFERSIZE)
        {
            buffer[len] = (char) c;
            len++;
            buffer[len] = '\0';
            putchar(c);
        }
        else if (c=='\n' || c=='\r')
        {            
            putchar('\n');            
            return; 
        }
        else if (c=='\b' && len>0)
        {
            if (buffer[0]!=':') 
            { 
                putchar('\b'); 
                putchar('\033');
                putchar('[');
                putchar('0');
                putchar('K');
            }
            len--;
            buffer[len] = '\0';
        }
    }
}

int parsepositive(unsigned char* txt, unsigned int len)
{
    unsigned int v = 0;
    unsigned int i;
    for (i=0; i<len; i++)
    {
        unsigned int c = txt[i];
        if (c<'0' || c>'9') { break; }
        v = (v<<3) + v + v + (c-'0');  // multiply by 10 and add next digit
    }
    return v;
}

Value parsebasicvalue(unsigned char* txt, int len)
{
    Value v = Nothing;
    // some literals
    if (txt[0]=='#') 
    {
        if (len==2 && (txt[1]=='f' || txt[1]=='F')) { v = False; }
        if (len==2 && (txt[1]=='t' || txt[1]=='T')) { v = True; }
    }
    // accept negative numbers
    else if (txt[0]=='-' && len>1 && txt[1]>='0' && txt[1]<='9')
    {
        v = get_int_v(-parsepositive(txt+1,len-1));
    }
    // accept numbers
    else if (txt[0]>='0' && txt[0]<='9') 
    {
        v = get_int_v(parsepositive(txt,len));
    }
    // default to symbol
    else 
    {
        char tmp = txt[len];
        txt[len] = '\0'; // temporarily insert string delimiter
        v = get_symbol_v(txt); 
        txt[len] = tmp;
    }
    return v;
}

#define THIS c=buffer[cursor]
#define NEXT cursor++; THIS
#define QUOTE 39
Value readvalue()
{
    int start;
    unsigned char c;
    
    // skip whitespaces
    THIS;
    while (c && c<=32) { NEXT; }
    if (!c) { return Nothing; }
    if (c==')' || c=='.') { NEXT; return Nothing; }  // TODO: error reporting

    // found quotation mark as shortcut for (quote .. )
    if (c==QUOTE)
    {
        NEXT;
        return create_cell_v(get_symbol_v("quote"), create_cell_v(readvalue(), Empty));
    }
    // parse a list
    else if (c=='(') 
    {
        Value head = Empty;
        Cell* tail = NULL;
        int founddot = 0;
        NEXT;
        while (c!=')')
        {
            if (!c) { receiveline('|',0); THIS; }  // request follow-up line
            else if (c<=32) { NEXT; }
            else if (c=='.') { founddot = 1; NEXT; }
            else if (founddot) 
            {
                if (tail) { tail->cdr = readvalue(); }
                else { readvalue(); }
                THIS;
            }
            else 
            {
                Value n = create_cell_v(readvalue(), Empty);
                if (tail) { tail->cdr = n; }
                else { head = n; }
                tail = n.cell;
                THIS;
            }
        }
        NEXT;
        return head;
    }
    
    // read whole token
    start = cursor;
    while (c>32 && c!='(' && c!=')' && c!='.' && c!=QUOTE) { NEXT; }
    // parse basic value
    return parsebasicvalue(buffer+start,cursor-start);
}

unsigned int placevalues[5] = {10000,1000,100,10,1};
void writeunsigned(unsigned int i)
{
    unsigned int p;
    unsigned int didwrite=0;
    for (p=0; p<5; p++)
    {
        unsigned int pv = placevalues[p];
        unsigned char digit = 0;
        while (i>=pv) 
        {
            i-=pv;
            digit++;
        }
        if (digit || didwrite || p==4)
        {
            putchar('0' + digit);
            didwrite++;
        }
    }
}

void writevalue(Value v)
{
    switch (v.basic.type)
    {
        case TYPE_NOTHING: 
            putchar('#');
            putchar('n');
            break;
        case TYPE_EMPTY: 
            putchar('(');
            putchar(')');
            break;
        case TYPE_SYMBOL: 
            {
                unsigned char* id = get_identifier(v.basic.value.u);
//             putchar('@');
                while (*id) { putchar (*(id++)); }
            }
            break;
        case TYPE_INT:
            {
                int i = v.basic.value.s;
                if (i>=0) 
                { 
                    writeunsigned((unsigned int) i); 
                }
                else 
                {
                    putchar('-');
                    if (i==-32768) { writeunsigned(32768u); }
                    else { writeunsigned((unsigned int) (-i)); }
                }
            }
            break;
        case TYPE_BOOL:
            putchar('#');
            putchar(v.basic.value.s ? 't' : 'f');
            break;
        case TYPE_BUILTIN:
            putchar('#');
            writeunsigned(v.basic.value);
            break;        
        default:  // everything else is a non-empty list
            {
                Cell* c = v.cell;
                putchar('(');
                for (;;) // go throught the list
                {
                    writevalue(c->car);
                    // normal end of list
                    if (c->cdr.basic.type == TYPE_EMPTY) 
                    { 
                        break; 
                    } 
                    putchar(' ');
                    // ending of improper list
                    if (ISBASIC(c->cdr))
                    {
                        putchar('.');
                        putchar(' ');
                        writevalue(c->cdr);
                        break;
                    }
                    // list continues
                    c = c->cdr.cell;
                }
                putchar(')');
            }
            break;
    }
}

