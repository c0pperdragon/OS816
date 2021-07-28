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

/*

void memcpy(void* destination, void* source, size_t len)
{
    size_t daddress = ((size_t*)(&destination)) [0];
    size_t dbank = ((size_t*)(&destination)) [1];
    size_t saddress = ((size_t*)(&source)) [0];
    size_t sbank = ((size_t*)(&source)) [1];

    send('[');
    
    if (len==0) { return; }
    
    for (;;)
    {
        size_t span;
        size_t span_m_1 = len - 1;
        size_t dremain_m_1 = 65535-daddress;
        size_t sremain_m_1 = 65535-saddress;
        if (dremain_m_1 < span_m_1) { span_m_1 = dremain_m_1; }
        if (sremain_m_1 < span_m_1) { span_m_1 = sremain_m_1; }
        
        MVN (daddress, dbank, saddress, sbank, span_m_1);

        span = span_m_1 + 1;
        if (span==len) { break; }

        len -= span;
        daddress += span;
        if (daddress==0) { dbank++; }
        saddress += span;
        if (saddress==0) { sbank++; }
    }
}

*/