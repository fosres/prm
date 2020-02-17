#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER	4096

int main(int argc,char**argv)	{
	

#if 0
	Open new or existing file for reading
	
	and writing, truncating to zero bytes;

	file permissions read+write for owner
	
	and nothing for all others.
#endif	
	int fd = open(argv[1],O_RDONLY);
	
	if (fd == -1)	{
		fprintf(stderr,"%s:Failed to open file\n",argv[1]);
		exit(1);
	
	}

	unsigned char buffer[BUFFER + 1];

	size_t read_size = 0;

	while ( ( (read_size = read(fd,buffer,BUFFER)) != 0 ) && (read_size != 0) ) 	{
		
		buffer[BUFFER] = 0;

		printf("%s",buffer);		

	}
	
	return 0;
}
