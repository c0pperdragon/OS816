// IO and toolbox functions to be used from programs compiled with the WDC compiler.
// All timing relevant functions will auto-detect the machine and adjust timing loops accordingly.

int main(int argc, char** argv); 

void sleep(int milliseconds);

void portout(int data);
int portin(void); 

void send(int data);
int receive(void);

void MVN(unsigned int destinationaddress, unsigned int destinationbank, 
         unsigned int sourceaddress, unsigned int sourcebank,
         unsigned int length_minus_one);
         