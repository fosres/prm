#include <stdio.h>
#include <dirent.h>

int main(int argc,char**argv)	{
	
	struct dirent *de; //Pointer for entry

	DIR * dr = opendir(argv[1]);

	if (dr == NULL) // opendir
	{
		printf("Could not open current directory");
	}

	while ((de = readdir(dr)) != NULL)	{
		printf("%s\n",de->d_name);
	}

	closedir(dr);

	return 0;

}
