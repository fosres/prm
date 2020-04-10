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

	copy_symlink(argv[2],argv[1]);

	printf("%d\n",symlink_exists(argv[1]));
	
	return 0;
}
