#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>



void lsa(char*basepath)	{
	
	struct dirent *de; //Pointer for entry

	DIR * dr = opendir(basepath);

	if (dr == NULL) // opendir
	{
		printf("Could not open current directory");
	}

	while ((de = readdir(dr)) != NULL)		{
		
		printf("%s:%d\n",de->d_name,de->d_type==DT_DIR);
		
		if ((de->d_type==DT_DIR)&&!(strcmp(de->d_name,".")==0)&&!(strcmp(de->d_name,"..")==0))	{
			
			printf("Made it\n");

			//You actually need to concatenate the full path name from argv, rename it basepath

			lsa(de->d_name);
		}
	}

	closedir(dr);

}

int main(int argc,char**argv)	{
	
	lsa(argv[1]);
	
	
	return 0;
}
