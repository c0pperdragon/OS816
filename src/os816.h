// IO and toolbox functions to be used from programs compiled with the WDC compiler
// All timing relevant functions assume that the machine is running with a 10Mhz clock

typedef unsigned char byte;
typedef unsigned int u16;

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
