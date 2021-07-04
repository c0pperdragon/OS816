// IO and toolbox functions to be used from programs compiled with the WDC compiler
// All timing relevant functions will auto-detect the machine and 
// adjust timing loops accordingly

typedef unsigned char byte;
typedef unsigned char u8;
typedef signed char i8;
typedef unsigned int u16;
typedef signed int i16;
typedef unsigned long int u32;
typedef signed long int i32;

typedef unsigned int bool;
#define true 1
#define false 0

void sleep(u16 milliseconds);

void portout(u16 data);
u16 portin(void); 

void send(u16 data);
u16 receive(void);

void print(char* text);
void printu16(u16 value);
void input(char* buffer, u16 bsize);

u16 tokenize(char* buffer, char** tokens, u16 maxtokens);
bool strequal(char* s1, char* s2);
