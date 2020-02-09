#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>

bool file_exists(const unsigned char*file)	{
	
	struct stat buffer;

	memset(&buffer,0x0,sizeof(stat));

	return(stat(file,&buffer) == 0);

}

bool dir_exists(const unsigned char*directory)	{
	
	return opendir(directory);

}

int unmark(const unsigned char*srcpath)	{
	
	struct dirent *de = 0; //Pointer for entry

	DIR * dr = opendir(srcpath);

	unsigned char src_fullname[2048];

	memset(src_fullname,0x0,2048);

	if (dr == NULL) // opendir
	{
		printf("Could not open current directory");
	}

	while ((de = readdir(dr)) != NULL)	{
		
		if (	(strcmp(de->d_name,".")==0) || (strcmp(de->d_name,"..")==0)	)	{
			
			continue;
		}

		else if ((de->d_type==DT_DIR))	{

			//You actually need to concatenate the full path name from argv, rename it srcpath
			
			strncat(src_fullname,srcpath,2048);

			strncat(src_fullname,"/",2048);

			strncat(src_fullname,de->d_name,2048);

			unmark(src_fullname);

			memset(src_fullname,0x0,2048);
		}

		else if ((de->d_type==DT_REG))	{
			
			strncat(src_fullname,srcpath,2048);

			strncat(src_fullname,"/",2048);
			
			strncat(src_fullname,de->d_name,2048);
				
			remove(src_fullname);
			
			memset(src_fullname,0x0,2048);
		
		}
	}

	closedir(dr);
	
		rmdir(srcpath);
	
	return r;

}

int delete(const unsigned char*destpath,const unsigned char*srcpath)	{
	
	struct dirent *de = 0; //Pointer for entry

	DIR * dr = opendir(srcpath);

	unsigned char src_fullname[2048];

	memset(src_fullname,0x0,2048);

	unsigned char dest_fullname[2048];
	
	memset(dest_fullname,0x0,2048);

	unsigned char leaf[2048];

	memset(leaf,0x0,2048);

	int r2 = -1;

	int r = -1;

	if (dr == NULL) // opendir
	{
		printf("Could not open current directory");
	}

	while ((de = readdir(dr)) != NULL)		{
		
		if (	(strcmp(de->d_name,".")==0) || (strcmp(de->d_name,"..")==0)	)	{
			
			continue;
		}

		else if ((de->d_type==DT_DIR))	{

			//You actually need to concatenate the full path name from argv, rename it srcpath
			
			strncat(src_fullname,srcpath,2048);

			strncat(src_fullname,"/",2048);

			strncat(src_fullname,de->d_name,2048);
			
			strncat(dest_fullname,destpath,2048);

			strncat(dest_fullname,"/",2048);

			strncat(dest_fullname,de->d_name,2048);

//			printf("%s:directory\n",src_fullname);

			r2 = delete(dest_fullname,src_fullname);

			memset(dest_fullname,0x0,2048);
			
			memset(src_fullname,0x0,2048);
		}

		else if ((de->d_type==DT_REG))	{
			
			strncat(src_fullname,srcpath,2048);

			strncat(src_fullname,"/",2048);
			
			strncat(src_fullname,de->d_name,2048);
			
			strncat(dest_fullname,srcpath,2048);

			strncat(dest_fullname,"/",2048);
			
			strncat(dest_fullname,de->d_name,2048);
			
				
//			printf("%s:file\n",src_fullname);
			
			if ( file_exists(dest_fullname) && !file_exists(src_fullname) )	{	
				
				r2 = remove(dest_fullname);

			}
			
			memset(src_fullname,0x0,2048);
		}

		else	{

			continue;
		}	

	}

	closedir(dr);

}

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

int main(int argc,char**argv)	{
	
	unmark(argv[1]);

	return 0;
}

