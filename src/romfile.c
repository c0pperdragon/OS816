
#include "os816.h"
#include "romfile.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ----------------- facility to read ROM files ----------------------------

typedef struct 
{
    unsigned char isopen;
    unsigned char pad0,pad1,pad2;
    unsigned long filestart;
    unsigned long fileend;
    unsigned long filecursor;
} 
RomReadFile;

#define CONCURRENTREADFILES 4
RomReadFile romreadfiles[CONCURRENTREADFILES];

int romfile_openread(const char * name)
{
    unsigned long img = 0x810000;   
    
    // try to find the rom image
    for (;;)
    {
        unsigned long imagelength;
        int namelength;
        int i;
        
        if (*((unsigned long*)img) != 0x454C4946) { return -1; }  // magic code "FILE"
        imagelength = *((unsigned long*)(img+4));   // total size in bytes
        
        if ( (namelength = strcmplen((char*) (img+8), name)) < 0)   // check for correct name
        {
            img += imagelength;  // skip current image
            continue;
        }

        // found the correct image, now need to find a free file descriptor
        for (i=0; i<CONCURRENTREADFILES; i++)
        {
            RomReadFile* f = &(romreadfiles[i]);
            if (f->isopen) { continue; }
            // yes, found a descriptor, set up for operation
            f->filestart = img + 9 + namelength;
            f->fileend = f->filestart + imagelength;
            f->filecursor = f->filestart; 
            f->isopen = 1;
            return i;
        }    
        return -1; // no file descriptor free
    }
}

int romfile_closeread(int readfd) 
{
    if (readfd>=0 && readfd<CONCURRENTREADFILES) 
    {
        romreadfiles[readfd].isopen = 0;
        return 0;
    }
    return -1;
}

unsigned int romfile_read(int readfd, void * buffer, unsigned int len)
{
    RomReadFile* f;

    if (len<1) { return 0; }
    
    if (readfd<0 || readfd>=CONCURRENTREADFILES) { return 0; }
    f = &(romreadfiles[readfd]);
    if (!f->isopen) { return 0; };

    if (f->filecursor+len > f->fileend)
    {
        len = (unsigned int) (f->fileend - f->filecursor);
    }
    
    memcpy(buffer, (void*)(f->filecursor), len);
    f->filecursor += len;
    return len;        
}

long romfile_lseek(int readfd, long offset, int whence)
{
    RomReadFile* f;
    long newcursor;

    if (readfd<0 && readfd>=CONCURRENTREADFILES) { return -1; }
    f = &(romreadfiles[readfd]);
    if (!f->isopen) { return -1; }

    switch (whence)
    {   
        case SEEK_SET: 
            newcursor =  f->filestart+offset;
            break;
        case SEEK_CUR:
            newcursor += f->filecursor + offset;
            break;            
        case SEEK_END:
            newcursor = f->fileend + offset;
            break;
        default: 
            return -1;
    }        
    if (newcursor<f->filestart || newcursor>f->fileend){ return -1; }
    f->filecursor = newcursor;
    return newcursor;
}

// ------------- facility to write ROM fies --------------------------

typedef struct WriteChunk
{
    struct WriteChunk* next;
    unsigned int datasize;
}
WriteChunk;

typedef struct 
{
    unsigned char isopen;
    unsigned char pad0,pad1,pad2;
    unsigned char* filename;
    WriteChunk *firstchunk;
    WriteChunk *lastchunk;
} 
RomWriteFile;

#define CONCURRENTWRITEFILES 4
RomWriteFile romwritefiles[CONCURRENTWRITEFILES];

int romfile_openwrite(const char *name)
{
    int i;

    
    // find a free file descriptor
    for (i=0; i<CONCURRENTWRITEFILES; i++)
    {
        RomWriteFile* f = &(romwritefiles[i]);
        if (f->isopen) { continue; }        
        // prepare file descriptor block
        f->isopen = 1;
        f->filename = longalloc( ((unsigned long)strlen(name))+1); 
        if (!f->filename) { return -1; }
        strcpy(f->filename, name);
        f->firstchunk = 0;
        f->lastchunk = 0;
        return i;
    }
    // no descriptors available
    return -1;
}

int romfile_closewrite(int writefd) 
{
    char sb[100];
    RomWriteFile* f;
    WriteChunk* chunk;
    WriteChunk *todelete;
    
    if (writefd<0 && writefd>=CONCURRENTWRITEFILES) { return -1; }
    f = &(romwritefiles[writefd]);
    if (!f->isopen) { return -1; }
    
    sprintf(sb, "Writing file: %s\n", f->filename);
    sendstr(sb);
    for (chunk=f->firstchunk; chunk; )
    {
        sprintf(sb, "%u bytes\n", chunk->datasize);
        sendstr(sb);
        
        todelete = chunk;
        chunk=chunk->next;
        free(todelete);
    }
    free(f->filename);
    f->isopen = 0;
    
    // not properly implemented
    return -1;
}

unsigned int romfile_write(int writefd, void * buffer, unsigned int len)
{
    RomWriteFile* f;
    WriteChunk* chunk;
    
    if (writefd<0 && writefd>=CONCURRENTWRITEFILES) { return -1; }
    f = &(romwritefiles[writefd]);
    if (!f->isopen) { return -1; }

    if (len<1) { return 0; }
    
    chunk = longalloc(sizeof(WriteChunk) + (unsigned long) len);
    if (!chunk) { return 0; }    
    
    chunk->next = 0;
    chunk->datasize = len;
    memcpy ( (void*) (((unsigned long) chunk) + sizeof(WriteChunk)), buffer, len);

    if (!f->firstchunk) { f->firstchunk = chunk; }
    if (f->lastchunk) { f->lastchunk->next = chunk; }
    f->lastchunk = chunk;
    
    return len;   
}

int romfile_delete(const char* name)
{
    // not implemented
    return -1; 
}

