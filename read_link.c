#include <unistd.h>
#include <stdio.h>
#include "acl.h"
#define BUFSIZE	4096
int main(int argc,char**argv)		{

	copy_symlink(argv[2],argv[1]);
	
	return 0;
}
