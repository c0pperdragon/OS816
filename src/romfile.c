#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "os816.h"
#include "romfile.h"

#define ROMFILETAG    0x454C4946           // magic code "FILE"
#define SECTOR(target) ((char*)(((unsigned long)(target))&0xFFFFF000))

// Structure of the rom file system:
// Beginning at romFileStart, all files are stored sequentially:
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

char *romFileStart(void)
{
    unsigned long top1;
    unsigned long top2;
    unsigned long top3;
    #asm
        XREF _END_CODE
        LDA #<_END_CODE
        STA %%top1
        LDA #^_END_CODE
        STA %%top1+2
        XREF _END_KDATA
        LDA #<_END_KDATA
        STA %%top2
        LDA #^_END_KDATA
        STA %%top2+2
        XREF _ROM_BEG_DATA
        XREF _BEG_DATA
        XREF _END_DATA
        LDA #<_ROM_BEG_DATA+(_END_DATA-_BEG_DATA)
        STA %%top3
        LDA #^_ROM_BEG_DATA+(_END_DATA-_BEG_DATA)
        STA %%top3+2
    #endasm
    if (top2>top1) { top1=top2; }
    if (top3>top1) { top1=top3; }
    return SECTOR(top1+0xFFF);
}


// ----------------- facility to read ROM files ----------------------------

typedef struct 
{
    unsigned int isopen;
    unsigned char* img;
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

void relocateRomReadFile(char* imgbefore, char* imgafter)
{
    int i;
    RomReadFile *rf;
    for (i=0, rf=romreadfiles; i<CONCURRENTREADFILES; i++, rf++) 
    {
        if (rf->isopen && rf->img == imgbefore)
        {
            rf->img = imgafter;
            rf->data = imgafter + 8 + strcmplen(imgafter+8, imgafter+8) + 1;
        }
    }
}

int romfile_openread(const char * name)
{
    char* img = romFileStart();   
    RomReadFile* f;
    
    // try to find the rom image
    for (;;)
    {
        unsigned long imagelength;
        unsigned int namelength;
        unsigned int i;
        
        if (*((unsigned long*)img) != ROMFILETAG) { return -1; }  // no more valid romfiles
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
            f->img = img;
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

// check if there is any open file with this name (for reading or writing)
int haveActiveRomFile(const char* name)
{
    int i;
    RomReadFile *rf;
    RomWriteFile *wf;
    for (i=0, rf=romreadfiles; i<CONCURRENTREADFILES; i++, rf++) 
    {
        if (rf->isopen && strcmplen(name,rf->img+8)) { return 1; }
    }
    for (i=0, wf=romwritefiles; i<CONCURRENTWRITEFILES; i++, wf++) 
    {
        if (wf->isopen && strcmplen(name,wf->filename)) { return 1; }
    }
    return 0;
}

// scan through files to find first unused position 
// in case the storage is corrupted, return 0
char* findUnusedRomLocation(void)
{
    char* img = romFileStart();
    while ( *((unsigned long*)img) == ROMFILETAG)
    {
        unsigned long l = *((unsigned long*)(img+4));
        img += l;
    }
    if ( *((unsigned long*)img) != 0xFFFFFFFF) { return 0; }  // found unusable area
    return img;
}

int romfile_openwrite(const char *name)
{
    unsigned int namelength = strcmplen(name, name);
    RomWriteFile* f;
    int i;

    // prevent write-accessing an already opened file
    if (haveActiveRomFile(name)) { return -1; }

    // delete a possible previously existing file of the same name
    romfile_delete(name);

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
    int returncode = 0;
    unsigned int namelength;
    unsigned long totalsize;
    char* romfiletop;
    char* img;
    char* imgend;
    char* cursor;
    unsigned int i;

    if (!f) { return -1; }

    // calculate needed total size of rom file
    namelength = strcmplen(f->filename,f->filename);
    totalsize = 9 + namelength + f->size;

    // calculate where the file should be written to
    img = findUnusedRomLocation();
    imgend = img + totalsize;
    
    // first check if there is enough empty space to store the file
    if ((!img) || imgend>topaddress_flash() || !isflashempty(img, totalsize)) 
    { 
        romfile_compact();
        // check again after compacting
        // check if there is enough empty space to store the file
        img = findUnusedRomLocation();
        imgend = img + totalsize;        
        if ((!img) || imgend>topaddress_flash() || !isflashempty(img, totalsize)) 
        {
            returncode=-1; 
            goto release_all; 
        }
    }    

    cursor = img;
    // write first half of the header (when arborting now, the file is unusable)
    writeflash(cursor, "FI", 2);
    cursor += 4;    
    // write size information
    writeflash(cursor, &totalsize, 4);
    cursor += 4;
    writeflash(cursor, f->filename, namelength+1);
    cursor += namelength;
    cursor ++;    
    // write all the chunks 
    for (i=0; i<f->usedchunks; i++)
    {
        unsigned int len = cursor+CHUNKSIZE<imgend ? CHUNKSIZE : (unsigned int)(imgend-cursor);
        writeflash (cursor, f->chunks[i], len);
        cursor += len;
    }
    // write second half of the header to make the file valid
    writeflash(img+2, "LE", 2);
    
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
    char* img = romFileStart();   
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

// ---------------- compactification ---------------------------------

// allocate 4KB on the stack and call the worker function with a pointer
// to this temporary memory
void romfile_compact()
{
    #asm
        TSC
        SEC
        SBC #4096
        TCS
        PEA #0
        INA
        PHA
        JSL ~~compactRomFiles
        TSC
        CLC
        ADC #4096
        TCS
    #endasm    
}

// fill transfer buffer and write to FLASH when buffer is filled
// in case this is the first transfer that hits the buffer, must make sure the
// start part is filled with the orginal data from the same target location
void transferViaBuffer(char* target, char* img, unsigned long len, char* tmp4k, int previouslytransfered)
{
    char* targetsector = SECTOR(target);
    if (!previouslytransfered)
    {
        memcpy (tmp4k, targetsector, (unsigned int)(target-targetsector));
    }
    while (len>0)
    {
        unsigned int space = 0x1000 - (unsigned int)(target-targetsector);
        if (len<space)
        {
            memcpy (tmp4k+(target-targetsector), img, (unsigned int) len);
            return;
        }
        else
        {
            memcpy (tmp4k+(target-targetsector), img, space);
            eraseflash(targetsector);
            writeflash(targetsector, tmp4k, 0x1000);
            target += space;
            img += space;
            len -= space;
            targetsector = SECTOR(target);
        }
    }
}

// actual compactification function 
void compactRomFiles(char *tmp4k)
{
    char *img = romFileStart();   // source
    char *target = img;           // target cursor
    int previouslytransfered = 0;  
    char* targetsector;
    char* erasesector;
    
    // scan over all files and copy-compact them
    for (;;)   
    {
        unsigned long len = *((unsigned long*)(img+4));
        // check if encountered corrupt file or last file in store
        if ( *((unsigned long*)img) != ROMFILETAG) { break; }
        if (len<9 || len>10000000 || img+len>topaddress_flash()) { break; }
        // encountered non-deleted file 
        if (img[8]) 
        {
            // need to actually copy data if there was a hole previously
            if (img!=target) 
            {
                transferViaBuffer(target,img,len,tmp4k,previouslytransfered);
                relocateRomReadFile(img,target);
                previouslytransfered = 1;                
            }
            img += len;
            target += len;
        }
        // encountered deleted file which we just skip (so src and target diverges)
        else
        {
            img += len;
        }
    }
    // make sure the start of the buffer is correctly set in case no files were transfered yet
    transferViaBuffer(target, 0, 0L, tmp4k, previouslytransfered);    
    targetsector = SECTOR(target);
    // erase everything after the files up to the end of rom file area
    for (erasesector=targetsector; erasesector < topaddress_flash(); erasesector+=0x1000)
    {
        eraseflash(erasesector);        
    }
    // write back the first part of the buffer in the newly erased area (may even have zero length)
    writeflash (targetsector, tmp4k, (unsigned int)(target-targetsector));   
}

