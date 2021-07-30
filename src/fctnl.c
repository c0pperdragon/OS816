
#include "os816.h"
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


int fileisopen=0;
long filestart;
long filesize;    
long filecursor;


//void _abort(void)
//{
//}
//
//void _exit(int _code)
//{
//}

int close(int fd) 
{
    if (fd==3) { fileisopen=0; }
    return -1;
}

int creat(const char *name, int mode)
{
    // TODO
    send ( (int) '!');
    send ( (int) '1');
    return -1;
}

int isatty(int fd)
{
    if (fd<=2) return 1;
    return 0;
}

long lseek(int fd, long offset, int whence)
{
    if (fd==3 && fileisopen)
    {
        switch (whence)
        {   case SEEK_SET: 
                filecursor =  offset;
                break;
            case SEEK_CUR:
                filecursor += offset;
                break;            
            case SEEK_END:
                filecursor = filesize + offset;
                break;
            default: 
                return -1;
        }        
        if (filecursor<0 || filecursor>=filesize)
        {
            filecursor = 0;
            return -1;
        }
        return filecursor;
    }
    return -1;
}

int open(const char * name, int mode)
{
    if (fileisopen) { return -1; }
    filestart = 0x840004;
    filesize = * ((long*)0x840000) ;
    filecursor = 0;
    fileisopen = 1;
    return 3;
}

size_t read(int fd, void * buffer, size_t len)
{
    size_t i;

    if (len<1) { return 0; }
    if (fd==0) 
    {
        ((unsigned char *)buffer) [0] = (unsigned char) receive();
        return 1;
    }   
    if (fd==3 && fileisopen)
    {
        if (filecursor+len > filesize)
        {
            len = filesize-filecursor;
        }
        burstmemcpy(buffer, (void*)(filestart+filecursor), len);
        filecursor += len;
        return len;        
    }
    
    return 0;
}

int unlink(const char * name)
{
    send ( (int) '!');
    send ( (int) '4');
   // TODO
    return -1;
}


size_t write(int fd, void * buffer, size_t len)
{
    size_t i = 0;

    if (fd==1 || fd==2) 
    {
        for (i=0; i<len; i++)
        {
            send ( ((unsigned char *)buffer) [i] );
        }    
    }
    
    return i;    
}
