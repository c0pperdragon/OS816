// IO and toolbox functions to be used from programs compiled with the WDC compiler
// All timing relevant functions assume that the machine is running with a 12Mhz clock

typedef unsigned int word;
typedef unsigned char byte;

void portset(byte bits);
void portclear(byte bits);
byte portin(void); 

void uartsend(byte data);
byte uartreceive(void);

void sleep(word milliseconds);

