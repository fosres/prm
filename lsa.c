#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>



void lsa(char*basepath)	{
	
	struct dirent *de = 0; //Pointer for entry

	DIR * dr = opendir(basepath);

	unsigned char fullname[2048];

	memset(fullname,0x0,2048);

	if (dr == NULL) // opendir
	{
		printf("Could not open current directory");
	}

	while ((de = readdir(dr)) != NULL)		{
		
		
		if ((de->d_type==DT_DIR)&&(strstr(de->d_name,".")==NULL)&&(strstr(de->d_name,"..")==NULL))	{
			
			printf("%s:%d\n",de->d_name,de->d_type==DT_DIR);

			printf("Made it\n");

			//You actually need to concatenate the full path name from argv, rename it basepath
			
			strncat(fullname,basepath,2048);

			strncat(fullname,"/",2048);

			strncat(fullname,de->d_name,2048);

			printf("%s\n",fullname);

			lsa(fullname);

			memset(fullname,0x0,2048);
		}
	}

	closedir(dr);

}

int main(int argc,char**argv)	{
	
	lsa(argv[1]);
	
	
	return 0;
}
