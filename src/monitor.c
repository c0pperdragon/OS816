#include "os816.h"

// Small monitor program to read/modify the memory, reprogram the flash and 
// execute code.

void receiveline(char* buffer, int buffersize)
{
    int len = 0;
    buffer[0] = '\0';
    send ('>');
    for (;;)
    {
        int c = receive();
        if (c>=32 && c<127 && len+1<buffersize)
        {
            if (c>='a' && c<='z') { c = c - 'a' + 'A'; }
            buffer[len] = (char) c;
            len++;
            buffer[len] = '\0';
            if (buffer[0]!=':') { send(c); }
        }
        else if (c=='\n' || c=='\r')
        {            
            if (buffer[0]!=':') { sendstr("\r\n"); }            
            return; 
        }
        else if (c=='\b' && len>0)
        {
            if (buffer[0]!=':') { sendstr("\b\033[0K"); }
            len--;
            buffer[len] = '\0';
        }
    }
}

void skiptospace(char* buffer, unsigned int* cursor)
{
    unsigned int c = *cursor;
    while (buffer[c]!=' ' && buffer[c]!='\0') { c++; }
    *cursor = c;
}

void skipoverspace(char* buffer, unsigned int* cursor)
{
    unsigned int c = *cursor;
    while (buffer[c]==' ' && buffer[c]!='\0') { c++; }
    *cursor = c;
}

unsigned long parsenumber(char* buffer, unsigned int* cursor, unsigned int maxdigits)
{
    unsigned int c = *cursor;
    unsigned long n = 0;
    unsigned int numdigits = 0;
    while (buffer[c]!=' ' && buffer[c]!='\0') 
    {
        unsigned int ch = buffer[c];
        unsigned int digit = 0;
        if (ch>='0' && ch<='9') { digit = ch-'0'; }
        if (ch>='A' && ch<='F') { digit = ch-'A'+10; }
        if (ch>='a' && ch<='f') { digit = ch-'a'+10; }
        c++;
        n += n;
        n += n;
        n += n;
        n += n;
        n += digit;
        numdigits++;
        if (numdigits>=maxdigits) { break; }
    }
    *cursor = c;    
    return n;
}

void printhex(unsigned int value)
{
    char* hexdigits = "0123456789ABCDEF";
    unsigned int digit = value>>4;
    send(hexdigits[digit]);
    digit = value&0xf;
    send(hexdigits[digit]);
}

void printdump(unsigned long address)
{
    unsigned int i;
    unsigned char* a = (void*)(&address);
    printhex(a[2]);
    printhex(a[1]);
    printhex(a[0]);
    send(' ');
    for (i=0; i<16; i++)
    {
        unsigned int c = *((unsigned char*)(address+i));
        printhex (c);
    }
    send(' ');
    for (i=0; i<16; i++)
    {
        unsigned int c = *((unsigned char*)(address+i));
        if (c>=32 && c<127) { send(c); }
        else { send('.'); }
    }
    sendstr("\r\n");
}


void processline(char* line, unsigned int* hexoffset)
{
    unsigned int cursor=0;
    unsigned long address;
    unsigned int numbytes;
    unsigned int numlines;
    unsigned int i;
    unsigned char cmd = line[0];   
    if (cmd=='H') 
    {              // HELP 
        sendstr
        ("\
Monitor commands. All numbers are HEX\r\n\
H                  Help (this message)\r\n\
M <addr> [bytes]   Memory dump\r\n\
[W] <addr> <data>  Write to memory\r\n\
R <addr>           Run program\r\n\
C                  Clear RAM bank 0\r\n\
E <sectoraddress>  Erase 4K flash sector\r\n\
!                  Erase all user flash\r\n\
:<intelhex>        Reprogram flash\r\n\
X                  Exit monitor\r\n"
        );
    }
    else if (cmd=='M') 
    {         // MEMORY DUMP
        skiptospace(line, &cursor);
        skipoverspace(line, &cursor);
        address = parsenumber(line, &cursor, 6);
        skipoverspace(line, &cursor);
        numbytes = parsenumber(line, &cursor, 4);
        if (!numbytes) { numbytes=256; }
        printdump (address);
        for (i=16; i!=0 && i<numbytes; i+=16) { printdump (address+i); }   
    }
    else if (cmd=='W' ||cmd=='0') 
    {  // WRITE TO MEMORY
        if (cmd!='0') {
            skiptospace(line, &cursor);
            skipoverspace(line, &cursor);
        }
        address = parsenumber(line, &cursor, 6);
        skipoverspace(line, &cursor);
        for (i=0; line[cursor]; i++)
        {
            unsigned char* a = (unsigned char*) (address + i);
            *a = (unsigned char) parsenumber(line, &cursor, 2);
            skipoverspace(line, &cursor);
        }
    }
    else if (cmd=='R') 
    {         // RUN
        skiptospace(line, &cursor);
        skipoverspace(line, &cursor);
        address = parsenumber(line, &cursor, 6);
    #asm
        PHD
        BRA pushreturnaddress
    docall:
        SEP #$20
        LDA %%address+2
        PHA
        REP #$20    
        LDA %%address
        DEC
        PHA
        RTL
    pushreturnaddress:
        JSL docall
        PLD
    #endasm      
    }
    else if (cmd=='C') 
    {         // CLEAR RAM BANK 0 (up to the stack pointer)
    #asm
        LDA #0
        STA >$000000
        LDX #0
        LDY #1
        TSC
        DEA
        MVN #0,#0
    #endasm    
    }
    else if (cmd=='E') 
    {         // ERASE FLASH SECTOR
        skiptospace(line, &cursor);
        skipoverspace(line, &cursor);
        address = parsenumber(line, &cursor, 6) & 0xFFFFF000;
        if (address<0xC00000 || address>=0xC7F000) {
            sendstr("OUT OF RANGE\r\n");
            return;
        }
        eraseflash((void*)address);
    }
    else if (cmd=='!') 
    {         // ERASE ALL USER FLASH
        for (address=0xC00000; address<0xC7F000; address += 0x1000) {
            eraseflash((void*)address);
        }
    }
    else if (cmd==':') 
    {         // Intel hex to write to flash
        unsigned char buffer[101];
        unsigned int checksum = 0;
        cursor++;
        skipoverspace(line, &cursor);
        numbytes = (unsigned int) parsenumber(line, &cursor, 2);
        if (numbytes>100) 
        {
            sendstr("\r\nTOO LONG\r\n");
            return;
        }
        checksum += numbytes;
        address = parsenumber(line, &cursor, 4);
        checksum += (unsigned int)address;
        checksum += ((unsigned int)address)>>8;
        cmd = (unsigned int) parsenumber(line, &cursor, 2);
        checksum += cmd;
        for (i=0; i<=numbytes; i++) 
        { 
            unsigned int b = parsenumber(line, &cursor, 2);
            buffer[i] = b;
            checksum += b;
        }
        if ((checksum & 0xFF)!=0) 
        {
            sendstr("\r\nCHECKSUM\r\n");
            return;
        }
        if (cmd==0) 
        { // write actual data
            unsigned long target = *hexoffset;
            target += target;
            target += target;
            target += target;
            target += target;
            target = 0xC00000 + target + address;
            if (target<0xC00000 || target+numbytes>0xC7F000) {
                sendstr("\r\nOUT OF RANGE\r\n");
                return;
            }
            if (writeflash((char*)target, buffer, numbytes)!=numbytes)
            {
                sendstr("\r\nVERIFY ERROR\r\n");
            }
        }
        else if (cmd==1)           
        {       // end of HEX file
            sendstr("\r\nEND\r\n");
        }
        else if (cmd==2 && numbytes==2) 
        {    // set the offset
            *hexoffset = (((unsigned int) buffer[0])<<8) + buffer[1];
        }
        else 
        {
            sendstr("\r\nUNSUPPORTED HEX COMMAND\r\n");
        }
    }
    else 
    {
        sendstr("UNKNOWN COMMAND\r\n");
    }
}

void monitor(void)
{
    char line[200];
    unsigned int hexoffset = 0;
    
    sendstr("OS816 boot monitor - type H for help.\r\n");
    for (;;)
    {
        receiveline(line,200);
        if (line[0]=='X') { return; }
        if (line[0]) { processline(line, &hexoffset); }
    }    
}
