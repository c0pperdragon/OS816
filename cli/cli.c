// command line interface to work with files and to start programs

#include "os816.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <math.h>


#define BUFFERSIZE 200
#define MAXTOKENS 10

typedef void (*main_t)(u16 argc, char** argv);

void cmd_dir(u16 argc, char* argv[])
{
    print("dir command not implemented yet\n");
}

void cmd_run(u16 argc, char* argv[])
{
    main_t p;
    char buffer[200];
    FILE *f;
    byte *ex;
    
    if (argc<2) 
    {
        print ("No program given to execute\n");
        return;
    }
    
    f = fopen(argv[0], "r");
    if (f==NULL)
    {
        printf("Can not open executable. errno:%d\n",17); // errno);
        return;
    }
    
    ex = malloc(1000);
    fread(ex, 1, 1000, f);
    fclose(f);
    free(ex);
    

}

void cmd_tokens(u16 argc, char* argv[])
{
    u16 i;
    for (i=0; i<argc; i++)
    {
        print("[");
        print(argv[i]);
        print("]\n");
    }
}

void cmd_help(u16 argc, char* argv[])
{
    print("Built-in commands:\n");
    print("    help   ......... this info\n");
    print("    dir <prefix> ... show all files whose name starts with< <prefix>\n");
    print("    run <file> ..... load an executable file and run it\n");
    print("    tokens ......... test command to show tokenizing result\n");
}



void main(int argc, char** argv)
{
    char buffer[BUFFERSIZE];
    char* tokens[MAXTOKENS];
    u16 numtokens;
    
    
    fprintf(stdout,"OS816 command line interface %d.%d (c) 2021 Reinhard Grafl\n", 0,1);

    
    assert(argc==0);
    assert(isalnum('A'));
    assert(sin(5.5) == sin(5.5));
    
    for (;;)
    {
        print(">");
        input(buffer, BUFFERSIZE);
        
        numtokens = tokenize(buffer, tokens, MAXTOKENS);        
        if (numtokens>0)
        {
            if (strcmp(tokens[0], "dir")==0) { cmd_dir(numtokens,tokens); }
            else if (strcmp(tokens[0], "run")==0) { cmd_run(numtokens,tokens); }
            else if (strcmp(tokens[0], "tokens")==0) { cmd_tokens(numtokens,tokens); }
            else { cmd_help(numtokens,tokens); }
        }
    }
}
