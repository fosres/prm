#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

int is_directory(const char*path)	{
	
	DIR * directory = opendir(path);
	
	if (errno==ENOTDIR)	{
		return 0;
	}

	if (directory!=NULL)	{
		closedir(directory);
		return 1;
	}

	return -1;

}


void lsa(char*argv)	{
	
	struct dirent *de; //Pointer for entry

	DIR * dr = opendir(argv);

	if (dr == NULL) // opendir
	{
		printf("Could not open current directory");
	}

	while ((de = readdir(dr)) != NULL)		{
		
		printf("%s\n",de->d_name);

		if(is_directory(de->d_name) && !strcmp(de->d_name,".") && !strcmp(de->d_name,".."))	{
			
			lsa(de->d_name);	
						
		}
	}

	closedir(dr);

}

int main(int argc,char**argv)	{
	
	lsa(argv[1]);

	return 0;
}
