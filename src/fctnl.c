
#include "os816.h"
#include <fcntl.h>

//void _abort(void)
//{
//}
//
//void _exit(int _code)
//{
//}

int close(int fd) 
{
    // TODO
    return -1;
}

int creat(const char *name, int mode)
{
    // TODO
    return -1;
}

int isatty(int fd)
{
    return 1;
}

long lseek(int fd, long offset, int whence)
{
    // TODO
    return -1;
}

int open(const char * name, int mode)
{
    // TODO
    return -1;
}

size_t read(int fd, void * buffer, size_t len)
{
    size_t i;
    if (len<1) { return 0; }
    // tty input
    for (i=0; i<len; i++)
    {   
        ((unsigned char *)buffer) [0] = (unsigned char) receive();
    }
    return i;
}

int unlink(const char * name)
{
    // TODO
    return -1;
}


size_t write(int fd, void * buffer, size_t len)
{
    size_t i;
    // tty output
    for (i=0; i<len; i++)
    {
        send ( ((unsigned char *)buffer) [i] );
    }    
    return i;
}
