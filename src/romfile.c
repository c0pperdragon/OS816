
#include "os816.h"
#include <string.h>
#include <fcntl.h>
#include <stdio.h>

typedef struct {
    unsigned long filestart;
    unsigned long filesize;
    unsigned long filecursor;
    unsigned char isopen;
    unsigned char dummy,dummy2,dummy3;
} RomFile;

#define CONCURRENTFILES 4
RomFile romfiles[CONCURRENTFILES];

int filenameidentical(const char* a, const char* b)
{
    int i;
    for (i=0; i<100; i++)
    {
        if (a[i]!=b[i]) { return 0; };
        if (a[i]==0) { return 1; }
    }
    return 0;
}


int romfile_open(const char * name)
{
    unsigned long* img = (unsigned long*) 0x00810000;   // hardcoded location of rom images
    unsigned long imagelength;
    int i;
    
    // try to find the rom image
    for (;;)
    {
        if (*img != 0x454C4946) { return -1; }  // magic code "FILE"
        imagelength = *(img+1);   // total size in bytes
        
        if (!filenameidentical((char*) (img+2), name))   // check for correct name
        {
            img = (unsigned long*) ( ((long int) img) + imagelength);  // skip current image
            continue;
        }

        // found the correct image, now need to find a free file descriptor
        for (i=0; i<CONCURRENTFILES; i++)
        {
            int namelen = strlen((char*) (img+2));
            RomFile* f = &(romfiles[i]);
            if (f->isopen) { continue; }
            // yes, found a descriptor, set up for operation
            f->filestart = ((unsigned long) img) + 9 + namelen; 
            f->filesize = imagelength - 9 - namelen;
            f->filecursor = 0;
            f->isopen = 1;
            return i;
        }    
        return -1; // no file descriptor free
    }
}

void romfile_close(int romfd) 
{
    if (romfd>=0 && romfd<CONCURRENTFILES) 
    {
        romfiles[romfd].isopen = 0;
    }
}

unsigned int romfile_read(int romfd, void * buffer, unsigned int len)
{
    RomFile* f;

    if (len<1) { return 0; }
    if (romfd<0 || romfd>=CONCURRENTFILES) { return 0; }

    f = &(romfiles[romfd]);
    if (!f->isopen) { return 0; };

    if (f->filecursor+len > f->filesize)
    {
        len = f->filesize - f->filecursor;
    }
    
    burstmemcpy(buffer, (void*)(f->filestart+f->filecursor), len);
    f->filecursor += len;
    return len;        
}

long romfile_lseek(int romfd, long offset, int whence)
{
    if (romfd>=0 && romfd<CONCURRENTFILES) 
    {
        RomFile* f = &(romfiles[romfd]);
        if (!f->isopen) { return -1; }

        switch (whence)
        {   case SEEK_SET: 
                f->filecursor =  offset;
                break;
            case SEEK_CUR:
                f->filecursor += offset;
                break;            
            case SEEK_END:
                f->filecursor = f->filesize + offset;
                break;
            default: 
                return -1;
        }        
        if (f->filecursor<0 || f->filecursor>=f->filesize)
        {
            f->filecursor = 0;
            return -1;
        }
        return f->filecursor;
    }
    return -1;
}
