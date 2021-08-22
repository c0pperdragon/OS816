#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include "romfile.h"
#include "os816.h"

// file descriptor mapping
// 0x00 - 0x0f for standard streams
// 0 .. stdin
// 1 .. stdout
// 2 .. stderr
// 0x04 - 0x07 descriptors for reading romfiles
// 0x08 - 0x0B descriptors for writing romfiles


int close(int fd) 
{
    if ((fd&0x0C)==0x04)    // reading descriptor
    {
        return romfile_closeread(fd&0x03);
    }        
    if ((fd&0x0C)==0x08)    // writing descriptor
    {
        return romfile_closewrite(fd&0x03);
    }        
    return -1; 
}

int creat(const char *name, int mode)
{
    return open(name,mode);
}

int isatty(int fd)
{
    return (fd>=0 && fd<=2) ? 1:0;
}

long lseek(int fd, long offset, int whence)
{
    if ((fd&0x0C)==0x04)     // reading descriptor
    {
        return romfile_lseekread(fd&0x03, offset, whence);
    }        
    if ((fd&0x0C)==0x08)     // writing descriptor
    {
        return romfile_lseekwrite(fd&0x03, offset, whence);
    }
    return -1;
}

int open(const char * name, int mode)
{    
    if (mode&O_APPEND) { return -1; } // append not supported
    
    if ((mode&0x000F)==O_RDONLY)
    {
        int rdfd = romfile_openread(name);
        if (rdfd>=0) { return rdfd | 0x04; }
    }
    if ((mode&0x000F)==O_WRONLY)
    {
        int wrfd = romfile_openwrite(name);
        if (wrfd>=0) { return wrfd | 0x08; }
    }
    return -1;
}

size_t read(int fd, void * buffer, size_t len)
{
    if (len<1) { return 0; }

    if (fd==0) // stdin
    {
        ((unsigned char *)buffer) [0] = (unsigned char) receive(0);
        return 1;
    }   
    if ((fd&0x0C)==0x04)    // romfile read
    {
        return romfile_read(fd&0x03, buffer, len);
    }        
    return 0;
}

int unlink(const char * name)
{
    return romfile_delete(name);
}

size_t write(int fd, void * buffer, size_t len)
{
    if (fd==1 || fd==2) 
    {
        size_t i = 0;
        for (i=0; i<len; i++)
        {
            send ( ((unsigned char *)buffer) [i] );
        }
        return len;        
    }
    if ((fd&0x0C)==0x08)    // romfile write
    {
        return romfile_write(fd&0x03, buffer, len);
    }            
    return 0;
}
