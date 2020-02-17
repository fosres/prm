#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER	4096

void copy_symlink(unsigned char * dest,unsigned char * src)	{

	unsigned char buffer[BUFFER + 1];

	readlink(src,buffer,BUFFER);

	symlink(buffer,dest);

}

int main(int argc,char**argv)	{
	
#if 0
	Open new or existing file for reading
	
	and writing, truncating to zero bytes;

	file permissions read+write for owner
	
	and nothing for all others.
#endif

#if 0
	int fd = open(argv[1],O_RDONLY | O_NOFOLLOW);
	
	if (fd == -1)	{
		fprintf(stderr,"%s:Failed to open file\n",argv[1]);
		exit(1);
	
	}
#endif

	copy_symlink(argv[2],argv[1]);

	return 0;
}
