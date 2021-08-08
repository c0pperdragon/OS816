// IO and toolbox functions to be used from programs compiled with the WDC compiler.
// All timing relevant functions will auto-detect the machine and adjust timing loops accordingly.

// Program entry point. Needs to be provided by the user program.
int main(int argc, char** argv); 

// Delay loop with reasonable precision
void sleep(int milliseconds);

// RAW port IO
void portout(int data);
int portin(void); 

// Communication via the serial interface (with hardware handshake)
void send(int data);
int receive(void);
void sendstr(const char* str);

// Compare strings and calculate length if identical.
// If not identical, return -1
int strcmplen(const char* a, const char* b);

// Generic heap allocation with sizes > 64K
void *longalloc(unsigned long payloadsize);
