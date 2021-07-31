
int romfile_openread(const char * name);
void romfile_closeread(int readfd) ;
unsigned int romfile_read(int readfd, void * buffer, unsigned int len);
long romfile_lseek(int readfd, long offset, int whence);

int romfile_openwrite(const char * name);
void romfile_closewrite(int writefd) ;
unsigned int romfile_write(int writefd, void * buffer, unsigned int len);

int romfile_delete(const char* name);
