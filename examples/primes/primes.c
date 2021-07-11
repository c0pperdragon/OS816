// example program to compute prime numbers
// this should give the result: number 5133  checksum 33852

#include <stdio.h>
#include <stdlib.h>

#define TOTAL 50000
#define TOTAL_SQRT 240

void sieve(char isprime[], int printall)
{
    unsigned int i,j;
    unsigned int count;
    unsigned int checksum;
        
    count = 0;
    checksum = 0;
    
    if (printall) { printf("Resetting\n"); }
    for (i=0; i<TOTAL; i++) { isprime[i]=1; }
    if (printall) { printf("Computing\n"); }
    
    for (i=2; i<TOTAL; i++) 
    { 
        if (isprime[i])
        {
            count++;
            checksum += i;
            
            if (printall) 
            {   printf("%u ",i);
            }   
            if (i<TOTAL_SQRT)
            {
                for (j=i+i; j<TOTAL; j=j+i)
                {
                    isprime[j]=0;
                }
            }
        }
    }
    if (printall) { printf("\n"); }
    printf("Number of primes: %u Checksum: %u\n", count,checksum);
}

int main(int arc, char** argv)
{
    char* buffer;
    printf("Compute prime numbers from %u to %u\n", 2, (TOTAL-1));
    buffer = (char*) 0x20000; // malloc((size_t) TOTAL);
    for (;;) { sieve(buffer,0); }
}
