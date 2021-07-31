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

// Compare strings and calculate length if identical.
// If not identical returns -1
int strcmplen(const char* a, const char* b);

// Memory copy operation that is optimized for larger blocks, utilizing the
// MVN processor instruction. For shorter blocks, the one-time overhead will
// dominate the operation. 
void burstmemcpy(void* destination, void* source, unsigned int len);
