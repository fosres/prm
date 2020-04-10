#if 0
https://stackoverflow.com/questions/42890587/retrieving-owner-name-of-file-using-getpwuid-in-c-always-throw-root

https://stackoverflow.com/questions/7624127/finding-the-owner-and-group-of-a-file-as-a-string
#endif

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
#include "acl.h"


void copy_symlink(unsigned char * dest,unsigned char * src)	{

	unsigned char buffer[BUFFER + 1];

	memset(buffer,0x0,sizeof(BUFFER+1));

	if ( readlink(src,buffer,BUFFER) <= 0 )	{
		
		return;

	}

	symlink(buffer,dest);


}

bool file_exists(const unsigned char*file)	{
	
	struct stat buffer;

	memset(&buffer,0x0,sizeof(stat));

	return(stat(file,&buffer) == 0);

}

DIR * dir_exists(const unsigned char*directory)	{
	
	return opendir(directory);

}

//unmark is potentially as powerful as the BASH "rm -rf" when run as root-owner user. Use with caution! SERIOUSLY!!!!

void unmark(const unsigned char*srcpath)	{
	
	struct dirent *de = 0; //Pointer for entry

	DIR * dr = opendir(srcpath);

	unsigned char src_fullname[MAXSIZE];

	memset(src_fullname,0x0,MAXSIZE);

	if (dr == NULL) // opendir
	{
		remove(srcpath);

		return;
	}

	while ((de = readdir(dr)) != NULL)	{
		
		if (	(strcmp(de->d_name,".")==0) || (strcmp(de->d_name,"..")==0)	)	{
			
			continue;
		}

		else if ((de->d_type==DT_DIR))	{

			//You actually need to concatenate the full path name from argv, rename it srcpath
			
			strncat(src_fullname,srcpath,MAXSIZE);

			strncat(src_fullname,"/",MAXSIZE);

			strncat(src_fullname,de->d_name,MAXSIZE);

			unmark(src_fullname);

			memset(src_fullname,0x0,MAXSIZE);
		}

		else if ((de->d_type==DT_REG))	{
			
			strncat(src_fullname,srcpath,MAXSIZE);

			strncat(src_fullname,"/",MAXSIZE);
			
			strncat(src_fullname,de->d_name,MAXSIZE);
				
			remove(src_fullname);
			
			memset(src_fullname,0x0,MAXSIZE);
		
		}
	}

	closedir(dr);
	
		rmdir(srcpath);

}

//delete will only delete the files and directories in the destpath directory that are NOT

//present in the srcpath directory. This function is inspired by the --delete flag in rsync

void delete(const unsigned char*destpath,const unsigned char*srcpath)	{
	
	if (!dir_exists(destpath))	{
		
		return;
	}	
	
	struct dirent *de = 0; //Pointer for entry

	DIR * dr = opendir(destpath);

	unsigned char src_fullname[MAXSIZE];

	memset(src_fullname,0x0,MAXSIZE);

	unsigned char dest_fullname[MAXSIZE];
	
	memset(dest_fullname,0x0,MAXSIZE);

	unsigned char leaf[MAXSIZE];

	memset(leaf,0x0,MAXSIZE);

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
			
			strncat(src_fullname,srcpath,MAXSIZE);

			strncat(src_fullname,"/",MAXSIZE);

			strncat(src_fullname,de->d_name,MAXSIZE);
			
			strncat(dest_fullname,destpath,MAXSIZE);

			strncat(dest_fullname,"/",MAXSIZE);

			strncat(dest_fullname,de->d_name,MAXSIZE);
			
			if ( dir_exists(dest_fullname) && !dir_exists(src_fullname) )	{ 
				
				unmark(dest_fullname);	
			}

			else if ( dir_exists(dest_fullname) && dir_exists(src_fullname) ) {

				delete(dest_fullname,src_fullname);

			}

			memset(dest_fullname,0x0,MAXSIZE);
			
			memset(src_fullname,0x0,MAXSIZE);
		}

		else if ((de->d_type==DT_REG))	{
			
			strncat(src_fullname,srcpath,MAXSIZE);

			strncat(src_fullname,"/",MAXSIZE);
			
			strncat(src_fullname,de->d_name,MAXSIZE);
			
			strncat(dest_fullname,destpath,MAXSIZE);

			strncat(dest_fullname,"/",MAXSIZE);
			
			strncat(dest_fullname,de->d_name,MAXSIZE);

			printf("src_fullname:%s\ndest_fullname:%s\n",src_fullname,dest_fullname);
			
			if ( file_exists(dest_fullname) && !file_exists(src_fullname) )	{	
				
				remove(dest_fullname);

			}
			
			memset(src_fullname,0x0,MAXSIZE);
			
			memset(dest_fullname,0x0,MAXSIZE);
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

	if (stat(src,&tmp)) {
		
		perror("stat");	

	}

	if (chmod(dest,tmp.st_mode & 07777))	{
		
		perror("chmod");
	}
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
		
				
		uid = info.st_uid;
	}

	else			{	
	
		uid = pwd->pw_uid;

	}
	
	if (grp==NULL)	{
		

		gid = info.st_gid;
		
	}

	else		{
	
		gid = grp->gr_gid;

	}
	
	
	if (chown(dest,uid,gid) == -1)	{
		
		fprintf(stderr,"%s:Failed to change file and ownership permissions of destination file\n");
		
	}

}
#if 0
int main(int argc,char**argv)	{
	
	delete("/home/tsalim/git/prm/swiss_bkup","/home/tsalim/git/prm/swiss");

	return 0;
}
#endif
