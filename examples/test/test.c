// Test suite to check correct behaviour of compiled code and
// standard libraries 

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <os816.h>

// test functions that will halt the program in case of an error

int checkcounter=0;

void test(int result, int expected)
{
    checkcounter++;
    if (result==expected) { return; }
    printf ("Failed test %d: Expected %d but received %d\n", checkcounter, expected, result);
}

void teststr(char* result, char* expected)
{
    int i;
    checkcounter++;    
    for (i=0; i<10000; i++)
    {
        if (result[i]!=expected[i]) { break; }
        if (result[i]=='\0') { return; }
    }
    printf ("Failed test %d: Expected '%s' but received '%s'\n", checkcounter, expected, result);
}


// tests for memory access

void test16bitoverboundaries(void)
{
    unsigned char* b1 = (unsigned char*) 0x0003FFFF;
    unsigned char* b2 = (unsigned char*) 0x00040000;
    int *bt = (int*) 0x0003FFFF;   
    *b1 = 0x47;
    *b2 = 0x11;
    test (*bt, 0x1147);    // 1
    *bt = 0x4711;
    test (*b1, 0x11);      // 2
    test (*b2, 0x47);      // 3
}

void testarrayaccrossboundaries(void)
{
    unsigned char* b1 = (unsigned char*) 0x0003FF00;
    unsigned char* b2 = (unsigned char*) 0x0003FFF0;
    unsigned char* b3 = (unsigned char*) 0x00040000;    
    b1[0x100] = 99;
    b2[0x011] = 88;
    test (b3[0], 99);      // 4
    test (b3[1], 88);      // 5
}

void putbyte(unsigned char* buffer, unsigned long pos, unsigned char value)
{
    unsigned long total = ((unsigned long) buffer) + pos;
    *((unsigned char*) total) = value;
}
unsigned char getbyte(unsigned char* buffer, unsigned long pos)
{
    unsigned long total = ((unsigned long) buffer) + pos;
    return *((unsigned char*) total);
}

void testlongarrayindex(void)
{
    unsigned char* b1 = (unsigned char*) 0x00030000;
    unsigned char* b2 = (unsigned char*) 0x0007FF00;
    putbyte(b1, 0x4FF00, (unsigned char) 65);
    test(*b2, 65);                                      // 6
    test(getbyte(b1, 0x4FF00), (unsigned char) 65);     // 7
    // test (b1[0x4FF00], 65);   // does not work - index is truncated to 16 bit
}

// standard libraries tests

void teststrings(void)
{
    char buffer[100];
    char *a;
    strcpy(buffer, "Hi folks");
    teststr(buffer, "Hi folks");              // 8
    test(strcmp(buffer, "Hi folks"), 0);      // 9
    test(strcmp(buffer, "And lower"), 1);     // 10
    test(strcmp(buffer, "Much higher"), -1);  // 11

    a = strdup(buffer);
    test(strcmp(a, buffer), 0);               // 12
    buffer[0] = 'W';
    test(strcmp(a, buffer), -1);              // 13
    free(a);
}

void testsprintf(void)
{
    char buffer[100];
    sprintf(buffer,"f*");
    teststr(buffer, "f*");                                           // 14
    sprintf(buffer, "Best prime is %d.", 17);
    teststr(buffer, "Best prime is 17.");                            // 15
    sprintf(buffer, "The answer is %d, which is %s!", 42,"unexpected");
    teststr(buffer, "The answer is 42, which is unexpected!");       // 16
}

// memory allocation

void printptr(void* ptr)
{
    long l = (long) ptr;
    int hi = (int) (l>>16);
    int lo = (int) l;
//    printf ("%04x%04x\n", hi,lo);
}

void testmalloc(void)
{
    void *a,*b,*c,*d,*e,*f,*g,*h;

    a = malloc(1000);
    printptr(a);
    b = malloc(2000);
    printptr(b);
    c = calloc(1000,200);
    printptr(c);
    d = malloc(2000);
    printptr(d);
    e = malloc(3000);
    printptr(e);
    if (b) { free(b); }
    if (d) { free(d); }
    if (c) { free(c); }
    f = malloc(20000);
    printptr(f);    
    g = malloc((unsigned int)60000);
    printptr(g);    
    h = malloc((unsigned int)50000);
    printptr(h);    
    free(h);
    free(g);
}

void testfileinput(void)
{
    FILE* f = fopen("romfile:x.txt","rb");
    char buffer[11];
    int len;
    if (!f) { printf("Can not open x.txt\n"); return; }
    len =  fread(buffer, 1,10, f);
    test(len, 2);               // 17
    buffer[len]='\0';
    teststr(buffer, "ab");      // 18
    fclose(f);
}

void testfileoutput(void)
{
    FILE* f;
    unsigned int i;
    unsigned long sum;
    
    if (! (f = fopen("romfile:y.txt","wb")) ) 
    { 
        printf("Can not open for write\n"); 
        return; 
    }
    for (i=1; i<=11000; i++)
    {
        if (fwrite(&i, 2, 1, f)!=1)        
        {
            printf("Error on writing\n"); 
            return; 
        }
    }
    if (fclose(f) ) 
    {
        printf("Error on closing write file\n"); 
        return; 
    }
    
    if (! (f = fopen("romfile:y.txt", "rb")) )
    { 
        printf("Can not open for read\n"); 
        return; 
    }
    if (fseek(f,(long)0,SEEK_END)) 
    {
        printf("Error on fseek\n"); 
        return; 
    }
    test ((unsigned int) ftell(f), 22000);                                    // 19
    fseek(f,(long)0,SEEK_SET);
    sum=0;
    while (fread (&i, 2, 1, f) == 1) 
    {   
        sum+=i;
    }    
    fclose(f);
    test ((unsigned int) sum, (unsigned int) (11001*5500));      // 20
}
    
void testfseek(void)
{
    FILE* f;
    char buffer[100];
    
    if (! (f = fopen("romfile:z","wb")) )                  { printf("fseek 1\n"); return; }
    if (fwrite("Fill in test string >____<.", 1,28,f)!=28) { printf("fseek 2\n"); return; }
    if (fseek(f, 21L, SEEK_SET)!=0)                        { printf("fseek 3\n"); return; }
    if (fwrite("GOOD", 1,4,f)!=4)                          { printf("fseek 4\n"); return; }
    if (fclose(f)!=0)                                      { printf("fseek 5\n"); return; }

    if (! (f = fopen("romfile:z","rb")) )                  { printf("fseek 6\n"); return; }
    if (fread(buffer, 1, 28, f)!=28)                       { printf("fseek 7\n"); return; }
    teststr(buffer, "Fill in test string >GOOD<.");       // 21
    fseek(f, -7L, SEEK_END);
    if (fread(buffer+8, 1, 4, f)!=4)                         { printf("fseek 8\n"); return; }
    teststr(buffer, "Fill in GOOD string >GOOD<.");       // 22
    if (fclose(f)!=0)                                      { printf("fseek 9\n"); return; }
}

// run suit of tests
int main(int argc, char** argv)
{
    printf("Running test suite...\n");
    attachdevice_romfile();
	
    // test memory allocation
    testmalloc();

    // memory access tests
    test16bitoverboundaries();
    testarrayaccrossboundaries();
    testlongarrayindex();
    
    // simple standard libraries tests
    teststrings();
    testsprintf();

    // test file io
    testfileinput();
    testfileoutput();
    testfseek();

    printf ("Tests completed\n");
    return (0); 
}
