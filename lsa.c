#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int is_regular_file(const char*path)	{
	struct stat * path_stat = 0;

	stat(path,path_stat);

	return S_ISREG((*path_stat).st_mode);
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

		if(!is_regular_file(de->d_name) && !strcmp(de->d_name,".") && !strcmp(de->d_name,".."))	{
			
			lsa(de->d_name);	
						
		}
	}

	closedir(dr);

}

int main(int argc,char**argv)	{
	
	lsa(argv[1]);

	return 0;
}
