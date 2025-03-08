#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "os816.h"
#include "devices.h"

typedef struct DeviceList DeviceList;
struct DeviceList {
    const char* fileprefix;	
    const Device* device;
	DeviceList* next;
};

DeviceList* alldevices = NULL; 


void attachdevice(const char* fileprefix, const Device* device)
{
	DeviceList* n;
	// do not attach twice
	for (n=alldevices; n!=NULL; n=n->next)
	{
		if (strcmp(fileprefix, n->fileprefix)==0) { return; }
	}	
	// create new list node
	n = malloc(sizeof(DeviceList));
	n->fileprefix = fileprefix;
	n->device = device;
	n->next = alldevices;
	alldevices = n;
}

const Device* finddevice(const char* filename, int * out_prefixlength)
{
	DeviceList* n;
	for (n=alldevices; n!=NULL; n=n->next)
	{
		const char* prefix = n->fileprefix;
		int i;
		for (i=0; prefix[i]==filename[i]; i++)
		{
			if (prefix[i+1]==0) 
			{ 
				*out_prefixlength = i+1;
				return n->device; 
			}
		}
	}
	return NULL;
}

