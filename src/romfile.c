#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "os816.h"
#include "romfile.h"

#define ROMFILESTART  0x00810000     // second bank of ROM
#define ROMFILETAG    0x454C4946     // magic code "FILE"

// Structure of the rom file system:
// Beginning at ROMFILESTART, all files are stored sequentially:
//     4 bytes          "FILE"
//     4 bytes          total size of the rom file (including name and header)
//     1 or more bytes  file name including an ending '\0' (empty string means deleted files)
//     x bytes          content of the file


// -------------- tools ---------------------------------

int strcmplen(const char* a, const char* b)
{
    int i;
    for (i=0; i<1000; i++)
    {
        if (a[i]!=b[i]) { return -1; };
        if (a[i]==0) { return i; }
    }
    return -1;
}

void blocktransfer(char* destination, const char* source, unsigned int len)
{
    #asm
        LDX %%len
        BEQ noblocktransfer
        LDY #0
        SEP #$20    
        longa off
    blocktransferloop:    
        LDA [%%source],Y
        STA [%%destination],Y
        INY
        DEX
        BNE blocktransferloop
        REP #$20    
        longa on
    noblocktransfer:
    #endasm      
}


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
    unsigned long img = ROMFILESTART;   
    
    // try to find the rom image
    for (;;)
    {
        unsigned long imagelength;
        int namelength;
        int i;
        
        if (*((unsigned long*)img) != ROMFILETAG) { return -1; }  
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
    
    blocktransfer(buffer, (char*)(f->filecursor), len);
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

typedef struct 
{
    unsigned char isopen;
    unsigned char pad0;
    unsigned int usedchunks;
    unsigned char* filename;
    unsigned char** chunks;
    unsigned long filesize;
} 
RomWriteFile;

#define MAXCHUNKS 256
#define CHUNKSIZE 1024
#define CONCURRENTWRITEFILES 4
RomWriteFile romwritefiles[CONCURRENTWRITEFILES];


int romfile_openwrite(const char *name)
{
    int namelength = strcmplen(name, name);
    int i;

    // find a free file descriptor
    for (i=0; namelength>0 && i<CONCURRENTWRITEFILES; i++)
    {
        RomWriteFile* f = &(romwritefiles[i]);
        char* filename;
        char** chunks;
        if (f->isopen) { continue; }
        // try to allocate more extra data
        filename = malloc(namelength+1); 
        if (!filename) { return -1; }
        blocktransfer(filename, name, ((unsigned int)namelength)+1);
        chunks = malloc(MAXCHUNKS*4);
        if (!chunks) { free(filename); return -1; }        
        // prepare file descriptor block
        f->isopen = 1;
        f->usedchunks = 0;
        f->filename = filename; 
        f->chunks = chunks;
        f->filesize = 0;
        return i;
    }
    // no descriptors available
    return -1;
}

int romfile_closewrite(int writefd) 
{
    RomWriteFile* f;
    unsigned int namelength;
    unsigned long romfiletop;
    unsigned long totalsize;
    unsigned long writepos;
    unsigned long img;
    unsigned int i;
    int returncode = 0;
    char sb[100];
    
    if (writefd<0 && writefd>=CONCURRENTWRITEFILES) { return -1; }
    f = &(romwritefiles[writefd]);
    if (!f->isopen) { return -1; }
    
    // calculate needed size of rom file
    namelength = (unsigned int) strcmplen(f->filename,f->filename);
    totalsize = 9 + namelength + f->filesize;

    // scan through files to find first unused position
    for (img = ROMFILESTART; *((unsigned long*)img) == ROMFILETAG; img+=*((unsigned long*)(img+4)) );

    // check if there is enough space to store the file
    #asm
        XREF _BEG_CODE
        LDA #<_BEG_CODE
        STA %%romfiletop
        LDA #^_BEG_CODE
        STA %%romfiletop+2
    #endasm
    if (img<ROMFILESTART || img+totalsize>romfiletop) { returncode=-1; goto release_all; }

    // delete a possible previously existing file of the same name
    romfile_delete(f->filename);
    
    // write all the chunks 
    writepos = img + 9 + namelength;
    
    
sendstr("writing header\n");    
    // write the file header 
    if ( !writeflash( img+4, &totalsize, 4)
      || !writeflash( img+8, f->filename, namelength+1)
      || !writeflash( img, "FILE", 4)
       )
    {
        returncode=-1;
        goto release_all;
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
    return len;   
}

int romfile_delete(const char* name)
{
    unsigned long img = ROMFILESTART;   
    unsigned char terminator = '\0';
    
    for (;;)
    {
        unsigned long imagelength;
        
        if (*((unsigned long*)img) != ROMFILETAG) { return -1; }  
        imagelength = *((unsigned long*)(img+4));   // total size in bytes
        
        if ( strcmplen((char*) (img+8), name) < 0)   // check for correct name
        {
            img += imagelength;  // skip current image
            continue;
        }

        // null out the file name
        return writeflash(img+8, &terminator, 1) ? 0 : -1;
    }    
    // not found
    return -1; 
}
