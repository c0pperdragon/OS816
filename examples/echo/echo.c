#include <os816.h>

int main(int argc, char** argv)
{
    sendstr ("Try to type keys...\n");
    for (;;)
    {
        int b;
        b = receive();
        if (b>=0) { send(b); }
    }
        
//    char* buffer = malloc(1000);
//    for (;;)
//    {
//        printf("What is your name?\n"); 
//        scanf("%s",buffer);   // DANGER! prone to buffer overrun
//        printf("Hi, '%s' (%d letters)!\n", buffer, strlen(buffer));
//    }
}
