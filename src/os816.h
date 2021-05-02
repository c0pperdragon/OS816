// IO and toolbox functions to be used from programs compiled with the WDC compiler
// All timing relevant functions assume that the machine is running with a 10Mhz clock

typedef unsigned int word;
typedef unsigned char byte;

void sleep(word milliseconds);

void portout(byte data);
byte portin(void); 

void send(byte data);
byte receive(void);

void print(char* text);
