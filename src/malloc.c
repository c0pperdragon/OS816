#include <stdlib.h>
#include <os816.h>

unsigned long heapcursor = 0x010000;
unsigned long heapend    = 0x080000;


void *longalloc(unsigned long _size)
{
    unsigned long s;
    if (heapcursor+_size > heapend) { return 0; }
    s = heapcursor;
    heapcursor += _size;
    return (void*) s;
}

void *calloc(size_t _nmemb, size_t _size)
{
    unsigned long total = 0;
    // multiply by adding
    if (_nmemb < _size)
    {
        size_t i;
        for (i=0; i<_nmemb; i++) { total += _size; }
    }
    else
    {
        size_t i;
        for (i=0; i<_size; i++) { total += _nmemb; }

    }
    return longalloc(total);
}

void *malloc(size_t _size)
{
    return longalloc( (unsigned long) _size);
}

void free(void *_ptr)
{
    // NOT SUPPORTED YET
}

void *realloc(void *_ptr, size_t _size)
{
    // NOT SUPPORTED YET
    return 0;
}


