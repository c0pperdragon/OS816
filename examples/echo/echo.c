
#include <stdio.h>

int main(int argc, char** argv)
{
    char buffer[200];
    for (;;)
    {
        printf("What is your name?\n"); 
        gets(buffer);   // DANGER! prone to buffer overrun
        printf("Hi, '%s'!\n", buffer);
    }
}
