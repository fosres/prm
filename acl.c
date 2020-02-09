#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
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
		
		if (	(strcmp(de->d_name,".")==0) || (strcmp(de->d_name,"..")==0)	)	{
			continue;
		}

		else if ((de->d_type==DT_DIR))	{

			//You actually need to concatenate the full path name from argv, rename it basepath
			
			strncat(fullname,basepath,2048);

			strncat(fullname,"/",2048);

			strncat(fullname,de->d_name,2048);

			printf("%s:directory\n",fullname);

			lsa(fullname);

			memset(fullname,0x0,2048);
		}

		else if ((de->d_type==DT_REG))	{
			
			strncat(fullname,basepath,2048);

			strncat(fullname,"/",2048);

			strncat(fullname,de->d_name,2048);
			
			printf("%s:file\n",fullname);
			
			memset(fullname,0x0,2048);
		}

		else	{

			continue;
		}	

	}

	closedir(dr);

}

#if 0

To guratee do_chmod and do_chown do their job, simply run program as root user-owner

#endif

void do_chmod(const unsigned char*dest,const unsigned char*src)	{
	
	struct stat tmp;

	memset(&tmp,0x0,sizeof(stat));

	stat(src,&tmp);

	chmod(dest,tmp.st_mode);
}

void do_chown(const unsigned char * dest,const unsigned char * src)	{
	
	uid_t	uid = 0;
	
	gid_t	gid = 0;

	struct stat info;

	memset(&info,0x0,sizeof(stat));

	stat(src,&info);

	struct passwd * pwd = getpwuid(info.st_uid);

	struct group * grp = getgrgid(info.st_gid);

	if (pwd == NULL)	{
		
		fprintf(stderr,"%s:Failed to get uid of sourcefile\n",src);	
		
		exit(1);

	}	
	
	uid = pwd->pw_uid;

	if (grp==NULL)	{
		
		fprintf(stderr,"%s:Failed to get gid of destination file\n",src);
		
		exit(1);	
	}

	gid = grp->gr_gid;

	if (chown(dest,uid,gid) == -1)	{
		
		fprintf(stderr,"%s:Failed to change file and ownership permissions of destination file\n");
		
		exit(1);
	}

}

#if 0
int main(int argc,char**argv)	{
	
	lsa(argv[1]);

	return 0;
}
#endif

