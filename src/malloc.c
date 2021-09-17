#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "os816.h"

#define HEAPSTART 0x010000


typedef struct FreeBlock {
    unsigned long length;     // total size (for free or used blocks alike)
    struct FreeBlock *next;   // here also starts the payload for used blocks
} FreeBlock;

#define MINBLOCKSIZE (sizeof(FreeBlock))

char isheapinitialized = 0;
FreeBlock *firstfree = 0;


void initheap(void)
{
    firstfree = (FreeBlock*) HEAPSTART;
    firstfree->length = ((unsigned long)topaddress_ram())-HEAPSTART;
    firstfree->next = 0;
    isheapinitialized = 1;    
}    

void *longalloc(unsigned long payloadsize)
{
    unsigned long length = payloadsize<MINBLOCKSIZE-4 ? MINBLOCKSIZE : payloadsize+4; 
    FreeBlock** fowner;
    FreeBlock* f;

    if (!isheapinitialized) { initheap(); }

    // keep track who is referencing the current block
    // search for first free block that is large enough
    for (fowner=&firstfree,f=firstfree; f; f=f->next)
    {
        if (f->length >= length)
        {            
            unsigned long remaining = f->length-length; // remaining bytes
            if (remaining < MINBLOCKSIZE)  
            {    
                // can not split off another free block, so directly use the whole one
                *fowner = f->next;   // disconnect
            }
            else
            {
                // split block off either from front or back part, depending on size of allocation
                if (payloadsize<=1024) 
                {  // small blocks are taken from front
                    FreeBlock* fragment = (FreeBlock*) ( ((unsigned long) f) + length );
                    fragment->length = remaining; 
                    fragment->next = f->next;  // replace in list, part 1
                    *fowner = fragment;        // replace in list, part 2 
                }
                else                  
                {   // large parts are taken from the end
                    f->length = remaining;      // shrink existing free block
                    f = (FreeBlock*) ( ((unsigned long) f) + remaining ); 
                }
                // allocated block gets new length
                f->length = length;
            }
            return (void*) (((unsigned long)f) + 4);  
        }
        fowner = (FreeBlock**) (((unsigned long)f) + 4);
    }
    // nothing suitable found
    return 0;
}

void free(void *ptr)
{
    unsigned long address;
    unsigned long length; 
    FreeBlock** fowner;
    FreeBlock* f;

    if (!ptr) { return; }
    
    address = ((unsigned long) ptr) - 4;
    length = *((unsigned long *)address); 

    // search through free list to find a block to which this one can be joined
    fowner = &firstfree;
    for (f=firstfree; f && ((unsigned long) f) < address; f=f->next)
    {
        if ( ((unsigned long) f) + f->length == address)
        {
            f->length += length;
            goto postprocessing;
        }
        fowner = (FreeBlock**) (((unsigned long)f) + 4);
    }
    // no suitable block found - need to insert it as a new free block 
    // before the first one that had a too high address
    *((FreeBlock**)ptr) = f;    // insert to list part 1
    f = (FreeBlock*) address;   // this is the new free block
    *fowner = f;                // insert to list part 2
    
postprocessing:    
    // after freeing, the block f may actually have a following free sibling now
    // that could be joined to f as well.
    if (((unsigned long) f ) + f->length == ((unsigned long) f->next))
    {
        FreeBlock* sibling = f->next;
        f->length += sibling->length;
        f->next = sibling->next;
    }

//sendfreememory("F",length,address);
}



// standard convenience wrapper functions 

void *calloc(size_t nmemb, size_t size)
{    
    unsigned long total;
    char* ptr;
    unsigned int i;
    
    // for optimization adjust the parameters, so that
    // size is always the larger of both values (at least not smaller)
    if (size < nmemb)
    {
        i = nmemb;
        nmemb = size;
        size = i;
    }
    // determine total size by adding (avoid need for clib multiplication)
    total = 0;
    for (i=0; i<nmemb; i++) { total += size; }
    // attempt allocation
    if (!(ptr = longalloc(total))) { return 0; }
    // initialize the buffer
    if (total>0)
    {
        long int iptr = ((long int) ptr) + size;
        for (i=0; i<size; i++) { ptr[i]=0; }
        for (i=1; i<nmemb; i++) 
        { 
            memcpy ((void*)iptr, ptr, size);
            iptr += size;
        }
    }
    // all done
    return (void*) ptr;
}

void *malloc(size_t size)
{
    return longalloc( (unsigned long) size);
}

void *realloc(void *ptr, size_t size)
{
    unsigned long prevsize;
    void* n;
    
    if (!ptr) { return longalloc((unsigned long)size); }
    if (!(n = longalloc((unsigned long)size) )) { return 0; }

    prevsize = *((unsigned long*)(((unsigned long)ptr)-4));
    if (prevsize<size) { size = (size_t) prevsize; }
    memcpy (n,ptr,size);  

    free (ptr);
    return n;
}

