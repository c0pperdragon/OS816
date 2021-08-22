#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "os816.h"
#include "romfile.h"

#define ROMFILESTART  ((char*)0x00810000)  // second bank of ROM
#define ROMFILETAG    0x454C4946           // magic code "FILE"

// Structure of the rom file system:
// Beginning at ROMFILESTART, all files are stored sequentially:
//     4 bytes          "FILE"
//     4 bytes          total size of the rom file (including name and header)
//     1 or more bytes  file name including an ending '\0' (empty string means deleted files)
//     x bytes          content of the file
// The space available for the file system extends up to the start of the CODE segment

// -------------- tools ---------------------------------

unsigned int strcmplen(const char* a, const char* b)
{
    unsigned int i;
    for (i=0; i<1000; i++)
    {
        if (a[i]!=b[i]) { return 0; };
        if (a[i]==0) { return i; }
    }
    return 0;
}

int isflashempty(char* area, unsigned long size)
{
    while (size) 
    {
        unsigned int testnow = (size>65535) ? 65535 : (unsigned int) size;
        unsigned int i;
        for (i=0; i<testnow; i++)
        {
            if (area[i]!=0xff) { return 0; }
        }
        area+=testnow;
        size-=testnow;
    }
    return 1;
}


// ----------------- facility to read ROM files ----------------------------

typedef struct 
{
    unsigned int isopen;
    unsigned char* data;
    unsigned long size;
    unsigned long cursor;
} 
RomReadFile;

#define CONCURRENTREADFILES 4
RomReadFile romreadfiles[CONCURRENTREADFILES];

RomReadFile *getOpenRomReadFile(int readfd)
{
    RomReadFile* f;
    if (readfd<0 || readfd>=CONCURRENTREADFILES) { return 0; }
    f = romreadfiles+readfd;
    if (!f->isopen) { return 0l; }
    return f;
}

int romfile_openread(const char * name)
{
    char* img = ROMFILESTART;   
    RomReadFile* f;
    
    // try to find the rom image
    for (;;)
    {
        unsigned long imagelength;
        unsigned int namelength;
        unsigned int i;
        
        if (*((unsigned long*)img) != ROMFILETAG) { return -1; }  
        imagelength = *((unsigned long*)(img+4));   // total size in bytes
        
        if (! (namelength = strcmplen(img+8, name)) )   // check for correct name
        {
            img += imagelength;  // skip current image
            continue;
        }

        // found the correct image, now need to find a free file descriptor
        for (i=0, f=romreadfiles; i<CONCURRENTREADFILES; i++, f++)
        {
            if (f->isopen) { continue; }
            // yes, found a descriptor, set up for operation
            f->isopen = 1;
            f->data = img + 9 + namelength;
            f->size = imagelength - namelength - 9;
            f->cursor = 0; 
            return i;
        }    
        return -1; // no file descriptor free
    }
}

int romfile_closeread(int readfd) 
{
    RomReadFile* f = getOpenRomReadFile(readfd);
    if (!f) { return -1; } 
    f->isopen = 0;
    return 0;
}

unsigned int romfile_read(int readfd, void * buffer, unsigned int len)
{
    RomReadFile* f = getOpenRomReadFile(readfd);
    if (!f) { return -1; }

    if (f->cursor+len > f->size)
    {
        if (f->cursor < f->size) { len = f->size - f->cursor; }
        else { len=0; }
    }
    
    memcpy(buffer, f->data + f->cursor, len);
    f->cursor += len;
    return len;        
}

long romfile_lseekread(int readfd, long offset, int whence)
{
    long newcursor;
    RomReadFile* f = getOpenRomReadFile(readfd);
    if (!f) { return -1; }
     
    switch (whence)
    {   
        case SEEK_SET: 
            newcursor = offset;
            break;
        case SEEK_CUR:
            newcursor = f->cursor + offset;
            break;            
        case SEEK_END:
            newcursor = f->size + offset;
            break;
        default: 
            return -1;
    }        
    if (newcursor<0){ return -1; }
    f->cursor = newcursor;
    return newcursor;
}

// ------------- facility to write ROM fies --------------------------

typedef struct 
{
    unsigned int isopen;
    unsigned char* filename;
    unsigned char** chunks;
    unsigned int usedchunks;
    unsigned long size;
    unsigned long cursor;
} 
RomWriteFile;

#define MAXCHUNKS 256
#define CHUNKSIZE 1024
#define CONCURRENTWRITEFILES 4
RomWriteFile romwritefiles[CONCURRENTWRITEFILES];

RomWriteFile *getOpenRomWriteFile(int writefd)
{
    RomWriteFile* f;
    if (writefd<0 || writefd>=CONCURRENTWRITEFILES) { return 0; }
    f = romwritefiles+writefd;
    if (!f->isopen) { return 0; }
    return f;
}

int romfile_openwrite(const char *name)
{
    unsigned int namelength = strcmplen(name, name);
    RomWriteFile* f;
    int i;

    // find a free file descriptor
    for (i=0, f=romwritefiles; namelength && i<CONCURRENTWRITEFILES; i++, f++) 
    {
        char* filename;
        char** chunks;
        if (f->isopen) { continue; }
        // try to allocate extra data
        if (! (f->filename = malloc(namelength+1)) ) { return -1; } 
        memcpy(f->filename, name, namelength+1);
        if (! (f->chunks = malloc(MAXCHUNKS*sizeof(char*))) ) { free(f->filename); return -1; };
        // complete file descriptor block
        f->isopen = 1;
        f->usedchunks = 0;
        f->size = 0;
        f->cursor = 0;
        return i;
    }
    // no descriptors available
    return -1;
}

int romfile_closewrite(int writefd) 
{
    RomWriteFile* f = getOpenRomWriteFile(writefd);
    unsigned long tag = ROMFILETAG;
    int returncode = 0;
    unsigned int namelength;
    unsigned long totalsize;
    char* romfiletop;
    char* img;
    char* imgend;
    unsigned int i;
    unsigned long dummy;
    
    if (!f) { return -1; }

    // delete a possible previously existing file of the same name
    romfile_delete(f->filename);

    // calculate needed total size of rom file
    namelength = strcmplen(f->filename,f->filename);
    totalsize = 9 + namelength + f->size;

    // scan through files to find first unused position
    for (img = ROMFILESTART; *((unsigned long*)img) == ROMFILETAG; )
    {
        unsigned long l = *((unsigned long*)(img+4));
        dummy=(unsigned long) img;
        img += l;
    }
    
    // extract information about code segment start (which is end of file system area)
    #asm
        XREF _BEG_CODE
        LDA #<_BEG_CODE
        STA %%romfiletop
        LDA #^_BEG_CODE
        STA %%romfiletop+2
    #endasm
    // check if there is enough empty space to store the file
    if (img<ROMFILESTART || img+totalsize>romfiletop) { returncode=-1; goto release_all; }
    // check if flash area can be written to
    if (!isflashempty(img, totalsize)) { returncode=-1; goto release_all; }
    
    imgend = img + totalsize;

    // write the header
    writeflash(img, &tag, 4);
    img += 4;    
    writeflash(img, &totalsize, 4);
    img += 4;
    writeflash(img, f->filename, namelength+1);
    img += namelength;
    img ++;
    
    // write all the chunks 
    for (i=0; i<f->usedchunks; i++)
    {
        unsigned int len = img+CHUNKSIZE<imgend ? CHUNKSIZE : (unsigned int)(imgend-img);
        writeflash (img, f->chunks[i], len);
        img += CHUNKSIZE;
    }
        
release_all:
    for (i=0; i<f->usedchunks; i++) { free(f->chunks[i]); }
    free(f->chunks);
    free(f->filename);
    f->isopen = 0;
    
    return returncode;
}

unsigned int romfile_write(int writefd, void * buffer, unsigned int len)
{    
    RomWriteFile* f = getOpenRomWriteFile(writefd);
    unsigned int neededchunks;
    unsigned long newsize;
    unsigned int written;

    if (!f) { return -1; }

    newsize = f->cursor + len;
    if (f->size > newsize) { newsize = f->size; }
    
    neededchunks = (newsize + CHUNKSIZE - 1) / CHUNKSIZE;     
    if (neededchunks>MAXCHUNKS) { return -1; }
    while (f->usedchunks < neededchunks)
    {
        char* chunk = malloc(CHUNKSIZE);
        if (!chunk) { return -1; }
        chunk[0] = 0;
        memcpy (chunk+1, chunk, CHUNKSIZE-1);        
        f->chunks[f->usedchunks] = chunk;
        f->usedchunks++;
    }
    f->size = newsize;

    written = 0;
    while (written<len)
    {
        unsigned int targetchunk = (unsigned int) (f->cursor / CHUNKSIZE); 
        unsigned int targetoffset = (unsigned int) (f->cursor % CHUNKSIZE);
        unsigned int canwrite = CHUNKSIZE-targetoffset;
        if (len-written < canwrite) { canwrite = len-written; }
        memcpy (f->chunks[targetchunk]+targetoffset, ((char*)buffer)+written, canwrite);
        written += canwrite;
        f->cursor += canwrite;
    }
    
    return len;
}

int romfile_delete(const char* name)
{
    char* img = ROMFILESTART;   
    char terminator = '\0';
    
    for (;;)
    {
        unsigned long imagelength = *((unsigned long*)(img+4));   // total size in bytes
        if (*((unsigned long*)img) != ROMFILETAG) { return -1; }  
        
        if (! strcmplen((char*) (img+8), name) )   // check for correct name
        {
            img += imagelength;  // skip current image
            continue;
        }

        // null out the file name
        writeflash(img+8, &terminator, 1);
        return 0;
    }    
    // not found
    return -1; 
}

long romfile_lseekwrite(int writefd, long offset, int whence)
{
    long newcursor;
    RomWriteFile* f = getOpenRomWriteFile(writefd);
    if (!f) { return -1; }
   
    switch (whence)
    {   
        case SEEK_SET: 
            newcursor = offset;
            break;
        case SEEK_CUR:
            newcursor = f->cursor + offset;
            break;            
        case SEEK_END:
            newcursor = f->size + offset;
            break;
        default: 
            return -1;
    }        
    if (newcursor<0){ return -1; }
    f->cursor = newcursor;
    return newcursor;
}
