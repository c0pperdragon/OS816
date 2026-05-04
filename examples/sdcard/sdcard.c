
#include <os816.h>

#define SERHIGH 0xE3
#define CS      0x04
#define MOSI    0x08
#define SCLK    0x10


const unsigned char CMD0[6]           = { 0x40|0, 0,0,0,0,       0x94|1 };
const unsigned char CMD8_000001AA[6]  = { 0x40|8, 0,0,0x01,0xAA, 0x86|1 };
const unsigned char CMD16_00000200[6] = { 0x40|16, 0,0,2,0,      1 };
const unsigned char CMD41_00000000[6] = { 0x40|41, 0,0,0,0,      1 };
const unsigned char CMD41_40000000[6] = { 0x40|41, 0x40,0,0,0,   1 };
const unsigned char CMD55[6]          = { 0x40|55, 0,0,0,0,      1 };

void sdcard_start(void)
{
	portout(SERHIGH | MOSI);
}
void sdcard_stop(void)
{
	portout(SERHIGH | CS | MOSI);
}
void sdcard_writebyte(unsigned int data)
{
	unsigned int i;
	unsigned int x = data;
	for (i=0; i<8; i++)
	{
		if (x & 0x80) 
		{
			portout(SERHIGH | MOSI);
			portout(SERHIGH | MOSI | SCLK);
		}
		else
		{
			portout(SERHIGH );
			portout(SERHIGH | SCLK);
		}
		x = x<<1;
	}
	portout(SERHIGH | MOSI);
}
unsigned int sdcard_readbyte(void)
{
	unsigned int i;
	unsigned int x = 0;
	for (i=0; i<8; i++) 
	{
		x = (x<<1) | (portin()&0x01); 
		portout(SERHIGH | MOSI | SCLK);
		portout(SERHIGH | MOSI );
	}
	return x;
}

unsigned int cmd_R1(const unsigned char* cmd)
{
	unsigned int i;
	sdcard_start();
	sdcard_writebyte(0xff);
	for (i=0; i<6; i++)
	{
		sdcard_writebyte(cmd[i]);
	}
	for (i=0; i<10; i++)  // reply can take some time
	{
		unsigned int r = sdcard_readbyte();
		if ((r&0x80)!=0) { continue; }
		sdcard_stop();
		return r;
	}
	sdcard_stop();
	return 0xff;
}

unsigned int cmd_R7(const unsigned char* cmd, unsigned char* response)
{
	unsigned int i;
	sdcard_start();
	sdcard_writebyte(0xff);
	for (i=0; i<6; i++)
	{
		sdcard_writebyte(cmd[i]);
	}
	for (i=0; i<10; i++) // reply can take some time
	{
		unsigned int r = sdcard_readbyte();
		if ((r&0x80)!=0) { continue; }
		for (i=0; i<4; i++)
		{
			response[i] = sdcard_readbyte();
		}
		sdcard_stop();
		return r;
	}
	sdcard_stop();
	return 0xff;
}


unsigned int init(void)
{
	unsigned int r,i;
	unsigned char x[4];
	
    sendstr("Start initializing sequence\r\n");
	// wait a bit
	sleep(1);
	// toggle the clock
	for (i=0; i<80; i++)
	{
		portout(SERHIGH | CS | MOSI | SCLK);
		portout(SERHIGH | CS | MOSI);
	}
	// try to set card to idle
	for (i=0; ; i++)
	{
		r = cmd_R1(CMD0);
		if (r==0x01) 
		{
			sendstr("Idle command successful\r\n");
			break;
		}
		else
		{
			sendstr("No correct response to idle command\r\n");
		}
		if (i==5) 
		{
			sendstr("Giving up on idle commands\r\n");
			return 0;
		}
	}
	// set card interface mode
	r = cmd_R7(CMD8_000001AA,x);
	sendstr("CMD8 response:"); sendnum(r); sendstr("\r\n");
	
	if ((r!=0x00) && (r!=0x01))   // error or no response 
	{  
		for (;;) 
		{
			cmd_R1(CMD55);
			r = cmd_R1(CMD41_00000000);
			if (r==0) 
			{ 
				sendstr("SD Ver. 1\r\n");
				break; 
			}
			if (r!=0x01) 
			{	
				sendstr ("No response to CMD41_00000000\r\n");
				return 0;
			}
		}
	}
	else if ((x[2]&0x0f)!=0x01 || x[3]!=0xAA) 
	{
		sendstr ("Mismatched configuration\r\n");
		return 0;
	}
	else
	{
		for (;;)
		{
			cmd_R1(CMD55);
			r = cmd_R1(CMD41_40000000);			
			if (r==0) 
			{ 
				sendstr("SD Ver. 2+\r\n");
				break; 
			}
			if (r!=0x01) 
			{	
				sendstr ("No response to CMD41_40000000\r\n");
				return 0;
			}
		}
	}
	// force block size to 512 bytes
	r=cmd_R1(CMD16_00000200);
	if (r!=0)
	{
		sendstr("Can not set block size\r\n");
		return 0;
	}
	return 1;
}

int main(int argc, char** argv)
{
	if (init()) { sendstr ("Initialization suceeded\r\n"); }
    return (0); 
}

