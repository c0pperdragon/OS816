
#include "os816.h"
#include "romfile.h"
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

int romfile_openread(const char * name)
{
    unsigned long* img = (unsigned long*) 0x00810000;   // hardcoded location of rom images
    
    // try to find the rom image
    for (;;)
    {
        unsigned long imagelength;
        int namelength;
        int i;
        
        if (*img != 0x454C4946) { return -1; }  // magic code "FILE"
        imagelength = *(img+1);   // total size in bytes
        
        if ( (namelength = strcmplen((char*) (img+2), name)) < 0)   // check for correct name
        {
            img = (unsigned long*) ( ((long int) img) + imagelength);  // skip current image
            continue;
        }

        // found the correct image, now need to find a free file descriptor
        for (i=0; i<CONCURRENTFILES; i++)
        {
            RomFile* f = &(romfiles[i]);
            if (f->isopen) { continue; }
            // yes, found a descriptor, set up for operation
            f->filestart = ((unsigned long) img) + 9 + namelength; 
            f->filesize = imagelength - 9 - namelength;
            f->filecursor = 0;
            f->isopen = 1;
            return i;
        }    
        return -1; // no file descriptor free
    }
}

void romfile_closeread(int romfd) 
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
        long newcursor;
        
        if (!f->isopen) { return -1; }

        switch (whence)
        {   case SEEK_SET: 
                newcursor =  offset;
                break;
            case SEEK_CUR:
                newcursor += f->filecursor + offset;
                break;            
            case SEEK_END:
                newcursor = f->filesize + offset;
                break;
            default: 
                return -1;
        }        
        if (newcursor<0 || newcursor>f->filesize){ return -1; }
        f->filecursor = newcursor;
        return newcursor;
    }
    return -1;
}


int romfile_openwrite(const char * name)
{
    // not implemented
    return 0; // return -1;
}

void romfile_closewrite(int writefd) 
{
    // not implemented
}

unsigned int romfile_write(int writefd, void * buffer, unsigned int len)
{
    // not implemented
    return len;   
}

int romfile_delete(const char* name)
{
    // not implemented
    return -1; 
}

