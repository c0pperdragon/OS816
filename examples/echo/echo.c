
#include <stdio.h>
#include <malloc.h>

int main(int argc, char** argv)
{
    char* buffer = malloc(1000);
    for (;;)
    {
        printf("What is your name?\n"); 
        gets(buffer);   // DANGER! prone to buffer overrun
        printf("Hi, '%s'!\n", buffer);
    }
}
