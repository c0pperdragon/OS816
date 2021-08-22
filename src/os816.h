// IO and toolbox functions to be used from programs compiled with the WDC compiler.
// All timing relevant functions will auto-detect the machine and adjust timing loops accordingly.

// Program entry point. Needs to be provided by the user program.
int main(int argc, char** argv); 

// Communication via the serial interface (with hardware handshake)
void send(int data);
int receive(unsigned int timeout);
void sendstr(const char* str);

// Facility to write to the flash area
void writeflash(char* target, void* data, unsigned int size);

// raw access to IO port
#define IOPORT (*((unsigned char*)0x400000))
