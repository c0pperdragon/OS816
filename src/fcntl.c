
#include "os816.h"
#include "romfile.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>



// file descriptor mapping
// 0x00 - 0x0f for standard streams
// 0 .. stdin
// 1 .. stdout
// 2 .. stderr
// 0x10 - 0x1F descriptors for reading romfiles
// 0x20 - 0x1F descriptors for writing romfiles


int close(int fd) 
{
    if ((fd&0xF0)==0x10)    // reading descriptor
    {
        romfile_closeread(fd&0x0F);
    }        
    if ((fd&0xF0)==0x20)    // writing descriptor
    {
        romfile_closewrite(fd&0x0F);
    }        
    return -1;  // dont care for this return value
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
    // only supported when reading romfiles
    if ((fd&0xF0)==0x10) 
    {
        return romfile_lseek(fd&0x0F, offset, whence);
    }        
    return -1;
}

int open(const char * name, int mode)
{    
    if (mode==O_RDONLY)
    {
        int rdfd = romfile_openread(name);
        if (rdfd>=0) { return rdfd | 0x10; }
    }
    if (mode==O_WRONLY)
    {
        int wrfd = romfile_openwrite(name);
        if (wrfd>=0) { return wrfd | 0x20; }
    }
    return -1;
}

size_t read(int fd, void * buffer, size_t len)
{
    if (len<1) { return 0; }

    if (fd==0) // stdin
    {
        ((unsigned char *)buffer) [0] = (unsigned char) receive();
        return 1;
    }   
    if ((fd&0xF0)==0x10)    // romfile read
    {
        return romfile_read(fd&0x0F, buffer, len);
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
    if ((fd&0xF0)==0x20)    // romfile write
    {
        return romfile_write(fd&0x0F, buffer, len);
    }            
    return 0;
}
