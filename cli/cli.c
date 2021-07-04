// command line interface to work with files and to start programs

#include "os816.h"

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
    
    if (argc<2) 
    {
        print ("No program given to execute\n");
        return;
    }
    
    // start execution of code on location 0x10000
    p = (main_t) 0x10000;
    p(argc-1,&(argv[1]));
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


void main()
{
    char buffer[BUFFERSIZE];
    char* tokens[MAXTOKENS];
    u16 numtokens;
    
    print("OS816 command line interface 0.1  (c) 2021 Reinhard Grafl\n");
    
    for (;;)
    {
        print(">");
        input(buffer, BUFFERSIZE);
        
        numtokens = tokenize(buffer, tokens, MAXTOKENS);        
        if (numtokens>0)
        {
            if (strequal(tokens[0], "dir")) { cmd_dir(numtokens,tokens); }
            else if (strequal(tokens[0], "run")) { cmd_run(numtokens,tokens); }
            else if (strequal(tokens[0], "tokens")) { cmd_tokens(numtokens,tokens); }
            else { cmd_help(numtokens,tokens); }
        }
    }
}
