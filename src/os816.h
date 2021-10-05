// IO and toolbox functions to be used from programs compiled with the WDC compiler.
// All timing relevant functions will auto-detect the machine and adjust timing loops accordingly.

// Program entry point. Needs to be provided by the user program.
int main(int argc, char** argv); 

// Restart the system
void softreset(void);

// Tuned delay loop
void sleep(unsigned int milliseconds);

// Communication via the serial interface (with hardware handshake)
void send(unsigned int data);
int receive(void);   // returns either the next byte or -1 if nothing available
void sendstr(const char* str);
void sendnum(unsigned int value);

// access to IO port
void portout(unsigned int data);
unsigned int portin(void);

// Facility to write to the flash area
unsigned int writeflash(char* target, void* data, unsigned int size);
void eraseflash(char* sectoraddress);

// Memory configuration queries
void* topaddress_flash(void);
void* topaddress_ram(void);
