// Test suite to check correct behaviour of compilde code and
// standard libraries 
// To have a minimum interface, when an error occurs in any of the
// test, a test number is presented in binary on the output port.
// When all tests pass, a 0 is set to the output port.


#include <os816.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


// test functions that will halt the program in case of an error

int checkcounter=0;

void test(int result, int expected)
{
    checkcounter++;
    if (result==expected) { return; }
    portout (checkcounter);
    for (;;) {}
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
    portout (checkcounter);
    for (;;) {}
}

// utility functions
void printhex(char* buffer)
{
    int i;
    char bo[4];
    for (i=0; i<10000; i++)
    {
        int c = buffer[i] & 0xff;
        if (c=='\0') { putchar('\n'); return; }
        send ('A' + (c>>4));
        send ('A' + (c & 0xf));
        send (' ');
    }
    send('\n');
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
    strcpy(buffer, "Hi folks");
    teststr(buffer, "Hi folks");              // 8
    test(strcmp(buffer, "Hi folks"), 0);      // 9
    test(strcmp(buffer, "And lower"), 1);     // 10
    test(strcmp(buffer, "Much higher"), -1);  // 11
}

void testsprintf(void)
{
    char buffer[100];
    strcpy(buffer, "dummy and very long string                  ");
    sprintf(buffer,"f*");
    teststr(buffer, "f*");                                           // 12
    sprintf(buffer, "Best prime is %d.", 17);
    teststr(buffer, "Best prime is 17.");                            // 13
    sprintf(buffer, "The answer is %d, which is %s!", 42,"unexpected");
    teststr(buffer, "The answer is 42, which is unexpected!");       // 14
}

// run suit of tests


int main(int argc, char** argv)
{     
    // memory access tests
    test16bitoverboundaries();
    testarrayaccrossboundaries();
    testlongarrayindex();
    
    // simple standard libraries tests
    teststrings();
    testsprintf();

    // all passed, indicate success
    portout(0);
    return 0;
}

