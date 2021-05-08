// example program to compute prime numbers
// this should give the result: number 5133  checksum 33852

#include "os816.h"

#define TOTAL 50000
#define TOTAL_SQRT 240
byte isprime[TOTAL];


void sieve(bool printall)
{
    u16 i,j;
    u16 count;
    u16 checksum;
        
    count = 0;
    checksum = 0;
    
    for (i=0; i<TOTAL; i++) { isprime[i]=1; }
    for (i=2; i<TOTAL; i++) 
    { 
        if (isprime[i])
        {
            count++;
            checksum += i;
            
            if (printall) 
            {   printu16(i);
                print(" ");
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
    if (printall) { print("\n"); }
    print("Number of primes: ");
    printu16(count);
    print(" Checksum: ");
    printu16(checksum);    
    print ("\n");
}

void main()
{
    print ("Compute prime numbers from 2 to ");
    printu16 (TOTAL-1);
    print ("\n");
    
    for (;;) { sieve(false); }
}
