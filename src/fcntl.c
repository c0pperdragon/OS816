
#include "os816.h"
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int romfile_open(const char * name);
void romfile_close(int romfd) ;
unsigned int romfile_read(int romfd, void * buffer, unsigned int len);
long romfile_lseek(int romfd, long offset, int whence);



// file descriptor mapping
// 0x00 - 0x0f for standard streams
// 0 .. stdin
// 1 .. stdout
// 2 .. stderr
// 0x10 - 0x1F  for romfile system (maybe not all available)


int close(int fd) 
{
    if ((fd&0xF0)==0x10)    // romfs
    {
        romfile_close(fd&0x0F);
    }        
    return -1;  // dont care for this return value
}

int creat(const char *name, int mode)
{
    // not available yet
    return -1;
}

int isatty(int fd)
{
    return (fd&0xF0) == 0x00;
}

long lseek(int fd, long offset, int whence)
{
    if ((fd&0xF0)==0x10)    // romfs
    {
        return romfile_lseek(fd&0x0F, offset, whence);
    }        
    return -1;
}

int open(const char * name, int mode)
{    
    // for readonly try a romfile
    if (mode==O_RDONLY)
    {
        int romfd = romfile_open(name);
        if (romfd>=0) { return romfd | 0x10; }
    }
    return -1;
}

size_t read(int fd, void * buffer, size_t len)
{
    if (len<1) { return 0; }

    if ((fd&0xF0)==0x10)    // romfs
    {
        return romfile_read(fd&0x0F, buffer, len);
    }        
    if (fd==0) // stdin
    {
        ((unsigned char *)buffer) [0] = (unsigned char) receive();
        return 1;
    }   
    return 0;
}

int unlink(const char * name)
{
    // not available yet
    return -1;
}

size_t write(int fd, void * buffer, size_t len)
{
    if ((fd&0xF0)==0x10)    // romfs
    {
        return 0;
    }        

    if (fd==1 || fd==2) 
    {
        size_t i = 0;
        for (i=0; i<len; i++)
        {
            send ( ((unsigned char *)buffer) [i] );
        }
        return len;        
    }
    
    return 0;
}
