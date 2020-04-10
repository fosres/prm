#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include "acl.h"
#define BUFSIZE	4096

int symlink_exists(const char*path)	{

	struct stat buf = {0};

	int result = lstat(path,&buf);

	return result;	
}

int main(int argc,char**argv)		{

//	copy_symlink(argv[2],argv[1]);
	
	unsigned char buf[4096];

	memset(buf,0x0,sizeof(unsigned char)*4096);

	readlink(argv[1],buf,BUFSIZE);

	unsigned char * dest = "source_dest_link\0";

	symlink(buf,dest);	
	
	return 0;
}
