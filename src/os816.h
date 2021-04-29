// IO and toolbox functions to be used from programs compiled with the WDC compiler
// All timing relevant functions assume that the machine is running with a 10Mhz clock

typedef unsigned int word;
typedef unsigned char byte;

void portout(byte data);
byte portin(void); 

void uartsend(byte data);
byte uartreceive(void);

void sleep(word milliseconds);

