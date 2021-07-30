
#include <stdio.h>
#include <malloc.h>

int main(int argc, char** argv)
{
    char* buffer = malloc(1000);
    for (;;)
    {
        printf("What is your name?\n"); 
        scanf("%s",buffer);   // DANGER! prone to buffer overrun
        printf("Hi, '%s' (%d letters)!\n", buffer, strlen(buffer));
    }
}
