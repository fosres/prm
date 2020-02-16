#include <stdio.h>
#include <unistd.h>

int main(int argc,char ** argv)	{
	printf("%s\n",argv[argc-1]);

	symlink(argv[argc-2],argv[argc-1]);

}
