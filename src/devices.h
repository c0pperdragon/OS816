
typedef struct IODescriptor IODescriptor;
typedef struct Device Device;
struct IODescriptor {
	const Device* device;
	// more device specific data
};
struct Device {
	int (*unlink)(const char * name);	
	IODescriptor* (*open)(const char* name, int mode);
	int (*close) (IODescriptor* descriptor);
	size_t (*read)(IODescriptor* descriptor, void * buffer, size_t len);
	size_t (*write)(IODescriptor* descriptor, void * buffer, size_t len);
	long (*lseek)(IODescriptor* descriptor, long offset, int whence);
};


void attachdevice(const char* fileprefix, const Device* device);
const Device* finddevice(const char* filename, int * out_prefixlength);

void attachdevice_romfile();
void attachdevice_sdcard();
