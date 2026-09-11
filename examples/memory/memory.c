// example program to allocate some memory
#include <stdio.h>
#include <stdlib.h>

void testmem(unsigned char* testpos1, unsigned char* testpos2)
{
	int i;	
	for (i=0; i<256; i++)
	{
		*testpos1 = (unsigned char) i;
		*(testpos1+1) = 0;
		printf ("Read from more memory: %d\n", (int) (*testpos2));
	}
}


int main(int arc, char** argv)
{
    int total;
	unsigned char* banks;

	unsigned char* tp = (unsigned char*) 0x120f00;
	testmem(tp,tp); 
	
	banks = (unsigned char*) 0xffff;
	printf("Total memory banks available: %d\n", (int) (*banks) );
	
    printf("Trying to allocate as many 1000 byte blocks as possible...\n");
	total = 0;
	while (malloc(1000)) { total++;	}
    printf("Got a total of %d blocks.\n",total);
	
	return 0;
}
