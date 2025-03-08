#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "os816.h"
#include "devices.h"

#define MAXDESCRIPTORS 20


// built-in device and descriptors for accessing the serial interface
// (descriptor is not used here)
int tty_close (IODescriptor* descriptor) 
{ 
	return 0;  // nothing to do, really
}
size_t tty_read(IODescriptor* descriptor, void * buffer, size_t len)
{
    int b;
    do { b=receive(); } while (b<0);
    ((unsigned char *)buffer) [0] = (unsigned char) b;
    return 1;	
}
size_t tty_write(IODescriptor* descriptor, void * buffer, size_t len)
{
    size_t i = 0;
    for (i=0; i<len; i++)
    {
        unsigned char b = ((unsigned char *)buffer) [i];
        if (b=='\n') { send('\r'); }
        send (b);
    }
    return len;        
}
long tty_lseek(IODescriptor* descriptor, long offset, int whence)
{
	return 0;
}
const Device ttydevice = 
{
	NULL,      // unlink (unused)	
	NULL,      // open (unused)
	tty_close,
	tty_read,
	tty_write,
	tty_lseek
};
IODescriptor ttydescriptor = 
{
	&ttydevice
};

IODescriptor* descriptors[MAXDESCRIPTORS] = 
{
	&ttydescriptor, &ttydescriptor, &ttydescriptor, NULL,NULL,NULL,NULL,NULL,NULL,NULL,
	NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL
}; 


// implement standard fcntl functions using the devices framework
int open(const char * name, int mode)
{    
	int fd;
	IODescriptor* descriptor;
	int prefixlength;
	const Device* device = finddevice(name, &prefixlength);
	if (!device) { return -1; }
	for (fd=0; fd<MAXDESCRIPTORS; fd++)
	{
		if (descriptors[fd]) { continue; }
		descriptor = (device->open)(name+prefixlength, mode);
		if (!descriptor) { return -1; }				
		descriptors[fd] = descriptor;
		return fd;
	}
	return -1;
}

int close(int fd) 
{
	IODescriptor* descriptor;
	if (fd<0 || fd>=MAXDESCRIPTORS) { return -1; }
	descriptor = descriptors[fd];
	if (!descriptor) { return -1; }
	descriptors[fd] = NULL;
	return (descriptor->device->close)(descriptor);
}

size_t read(int fd, void * buffer, size_t len)
{
	IODescriptor* descriptor;
	if (fd<0 || fd>=MAXDESCRIPTORS) { return 0; }
	descriptor = descriptors[fd];
	if (!descriptor) { return 0; }
	return (descriptor->device->read) (descriptor,buffer,len);
}

size_t write(int fd, void * buffer, size_t len)
{
	IODescriptor* descriptor;
	if (fd<0 || fd>=MAXDESCRIPTORS) { return 0; }
	descriptor = descriptors[fd];
	if (!descriptor) { return 0; }
	return (descriptor->device->write) (descriptor,buffer,len);
}

long lseek(int fd, long offset, int whence)
{
	IODescriptor* descriptor;
	if (fd<0 || fd>=MAXDESCRIPTORS) { return 0; }
	descriptor = descriptors[fd];
	if (!descriptor) { return 0; }
	return (descriptor->device->lseek) (descriptor,offset,whence);
}

int unlink(const char * name)
{
	int prefixlength;
	const Device* device = finddevice(name, &prefixlength);
	if (!device) { return -1; }
	return (device->unlink)(name+prefixlength);
}

int creat(const char *name, int mode)
{
    return open(name,mode);
}

int isatty(int fd)
{
	if (fd<0 || fd>=MAXDESCRIPTORS) { return 0; }
	return (descriptors[fd]==&ttydescriptor) ? 1:0;
}


		









/*


int close(int fd) 
{
    if ((fd&0x0C)==0x04)    // reading descriptor
    {
        return romfile_closeread(fd&0x03);
    }        
    if ((fd&0x0C)==0x08)    // writing descriptor
    {
        return romfile_closewrite(fd&0x03);
    }        
    return -1; 
}

int creat(const char *name, int mode)
{
    return open(name,mode);
}

int isatty(int fd)
{
    return (fd>=0 && fd<=2) ? 1:0;
}

long lseek(int fd, long offset, int whence)
{
    if ((fd&0x0C)==0x04)     // reading descriptor
    {
        return romfile_lseekread(fd&0x03, offset, whence);
    }        
    if ((fd&0x0C)==0x08)     // writing descriptor
    {
        return romfile_lseekwrite(fd&0x03, offset, whence);
    }
    return -1;
}

int open(const char * name, int mode)
{    
    if (mode&O_APPEND) { return -1; } // append not supported
    
    if ((mode&0x000F)==O_RDONLY)
    {
        int rdfd = romfile_openread(name);
        if (rdfd>=0) { return rdfd | 0x04; }
    }
    if ((mode&0x000F)==O_WRONLY)
    {
        int wrfd = romfile_openwrite(name);
        if (wrfd>=0) { return wrfd | 0x08; }
    }
    return -1;
}

size_t read(int fd, void * buffer, size_t len)
{
    if (len<1) { return 0; }

    if (fd==0)             // serial read
    {
        int b;
        do { b=receive(); } while (b<0);
        ((unsigned char *)buffer) [0] = (unsigned char) b;
        return 1;
    }   
    if ((fd&0x0C)==0x04)    // romfile read
    {
        return romfile_read(fd&0x03, buffer, len);
    }        
    return 0;
}

int unlink(const char * name)
{
    return romfile_delete(name);
}

size_t write(int fd, void * buffer, size_t len)
{
    if (fd==1 || fd==2) 
    {
        size_t i = 0;
        for (i=0; i<len; i++)
        {
            unsigned char b = ((unsigned char *)buffer) [i];
            if (b=='\n') { send('\r'); }
            send (b);
        }
        return len;        
    }
    if ((fd&0x0C)==0x08)    // romfile write
    {
        return romfile_write(fd&0x03, buffer, len);
    }            
    return 0;
}
*/
