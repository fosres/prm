#if 0
https://stackoverflow.com/questions/6856635/hide-password-input-on-terminal

https://security.stackexchange.com/questions/176974/why-is-accessing-kernel-memory-a-security-risk

https://security.stackexchange.com/questions/31390/whats-a-good-secure-file-deleter?rq=1

https://stackoverflow.com/questions/8778834/change-owner-and-group-in-c

https://stackoverflow.com/questions/43627117/unix-copy-a-file-with-original-permission-in-c?noredirect=1&lq=1

https://stackoverflow.com/questions/7624127/finding-the-owner-and-group-of-a-file-as-a-string

https://stackoverflow.com/questions/7328327/how-to-get-files-owner-name-in-linux-using-c

https://stackoverflow.com/questions/2256945/removing-a-non-empty-directory-programmatically-in-c-or-c
#endif
#include <termios.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>
#include <unistd.h>
#include <sodium.h>
#include "acl.h"

#define CHUNK_SIZE	4096

#define MAXSIZE		2048

//my_getpass accepts a password no larger than MAXSIZE bytes

int get_pass(unsigned char * pwd,unsigned char * repwd,const size_t MAX_SIZE, FILE * stream)	{
	
	unsigned char * p = pwd;

	unsigned char * r = repwd;

	struct termios old = {0},new = {0};

	size_t n = 0;

	//Turn echoing into stdout off and exit if my_getpass cannot.
	
	if (tcgetattr(fileno(stream),&old) != 0)	{
		
		fprintf(stderr,"Error: Failed to turn off echoing with tcgetaddr().\n");

		return 0;
	}

	new = old;

	new.c_lflag &= ~ECHO;

	if (tcsetattr(fileno(stream),TCSAFLUSH,&new) != 0)	{
		
		fprintf(stderr,"Error: Failed to turn off echoing with tcsetaddr().\n");

		return 0;
	}

	//Now read the password form stdin
	
	printf("Enter Password:");

	while ( ( (*p = getchar()) != 0xa ) && ( n < MAX_SIZE ) )	{
		
		p++;

		n++;
	}

	putchar(0xa);
	
	n = 0;
	
	printf("Reenter Password:");

	while ( ( ( *r = getchar()) != 0xa ) && ( n < MAX_SIZE) )	{
		
		r++;

		n++;

	}

	putchar(0xa);

	n = 0;

	//Restoring terminal to echoing:
	
	(void) tcsetattr(fileno(stream),TCSAFLUSH,&old);

	if ( strcmp(pwd,repwd) != 0 )	{
		
		fprintf(stderr,"Error:Passwords do not match. Try again.\n");
		
		return 0;
	}

	return 1;
	
}

void create_dir_clone(const unsigned char*dest,const unsigned char*src)	{
	
	struct stat st = {0};

	if ( stat(dest,&st) == -1 )	{
		
		mkdir(dest,0700);

	}
		do_chmod(dest,src);
		
		do_chown(dest,src);

}

static int encrypt(const char*dest,const char*src,const unsigned char key[crypto_secretstream_xchacha20poly1305_KEYBYTES])	{
	
	unsigned char buf_in[CHUNK_SIZE];

	memset(buf_in,0x0,CHUNK_SIZE*sizeof(unsigned char));
	
	unsigned char buf_out[CHUNK_SIZE + crypto_secretstream_xchacha20poly1305_ABYTES];

	memset(buf_out,0x0,(CHUNK_SIZE+crypto_secretstream_xchacha20poly1305_ABYTES)*sizeof(unsigned char));

	unsigned char header[crypto_secretstream_xchacha20poly1305_HEADERBYTES];

	memset(header,0x0,(crypto_secretstream_xchacha20poly1305_HEADERBYTES)*sizeof(unsigned char));

	crypto_secretstream_xchacha20poly1305_state st;

	memset(&st,0x0,sizeof(crypto_secretstream_xchacha20poly1305_state));

	FILE * out = 0, * in = 0;

	unsigned long long int out_len = 0;

	size_t rlen = 0;

	unsigned eof = 0;

	unsigned char tag = 0;
	
	if ( (in = fopen(src,"rb")) == NULL )	{
		
		fprintf(stderr,"%s:Error: Failed to read source file during encryption\n",src);
		
		exit(1);

	}

	if ( ( out = fopen(dest,"wb")) == NULL )	{
		
		fprintf(stderr,"%s:Error: Failed to create target file during encryption\n",dest);
		
		exit(1);

	}

	crypto_secretstream_xchacha20poly1305_init_push(&st,header,key);

	fwrite(header,1,sizeof(header),out);

	do	{
		
		rlen = fread(buf_in,1,sizeof(buf_in),in);
		
		eof = feof(in);

		tag = eof ? crypto_secretstream_xchacha20poly1305_TAG_FINAL : 0;

		crypto_secretstream_xchacha20poly1305_push(&st,buf_out,&out_len,buf_in,rlen,NULL,0,tag);

		fwrite(buf_out,1,(size_t) out_len,out);

	} while (!eof);

	fclose(out);

	fclose(in);

	return 0;
}

static int decrypt(const char*dest,const char*src,const unsigned char key[crypto_secretstream_xchacha20poly1305_KEYBYTES])	{
	
	unsigned char buf_in[CHUNK_SIZE + crypto_secretstream_xchacha20poly1305_ABYTES];
	
	memset(buf_in,0x0,sizeof(CHUNK_SIZE + crypto_secretstream_xchacha20poly1305_ABYTES));

	unsigned char buf_out[CHUNK_SIZE];
	
	memset(buf_out,0x0,sizeof(CHUNK_SIZE));

	unsigned char header[crypto_secretstream_xchacha20poly1305_HEADERBYTES];

	memset(header,0x0,sizeof(crypto_secretstream_xchacha20poly1305_HEADERBYTES));

	crypto_secretstream_xchacha20poly1305_state st;

	memset(&st,0x0,sizeof(crypto_secretstream_xchacha20poly1305_state));

	FILE * out = 0, * in = 0;

	unsigned long long out_len = 0;

	size_t rlen = 0;

	unsigned eof = 0;

	int ret = -1;

	unsigned char tag = 0;

	if ( (in = fopen(src,"rb")) == NULL )	{
		fprintf(stderr,"Error: Failed to read source file during decryption\n");
		
		exit(1);

	}

	if ( ( out = fopen(dest,"wb")) == NULL )	{
		
		fprintf(stderr,"Error: Failed to create target file during decryption\n");
		
		exit(1);

	}
	
	fread(header,1,sizeof(header),in);

	if ( crypto_secretstream_xchacha20poly1305_init_pull(&st,header,key) != 0)	{
		goto ret; // incomplete header
	}

	do	{
			rlen = fread(buf_in,1,sizeof(buf_in),in);
			eof = feof(in);

			if (crypto_secretstream_xchacha20poly1305_pull(&st,buf_out,&out_len,&tag,buf_in,rlen,NULL,0) != 0)	{
			goto ret; //corrupted chunk;	

			}

		if (tag == crypto_secretstream_xchacha20poly1305_TAG_FINAL && !eof)	{
			
			goto ret; //premature end (end of file reached before the end of the stream
		}

		fwrite(buf_out,1,(size_t)out_len,out);
		

	} while(!eof);

	ret = 0;

ret:
	if (ret==-1)	{ 
		
		fprintf(stderr,"Decryption failed, printing empty file\n");

	}

	fclose(in);
	
	fclose(out);

	return ret;

}

//copies file contents, chmod permissions, and chown permissions to destination
int archive(unsigned char*destfile,unsigned char*srcfile);

//quickly encrypts file with password defined in out

int encp(const unsigned char*dest,const unsigned char*src,const unsigned char*out)	{
	
	if (encrypt(dest,src,out) != 0)	{
		
		fprintf(stderr,"Error: Encryption failed\n");	

		return 0;
	}

	return 1;
}

//quickly decrypts file using key as defined in out

int dencp(const unsigned char*dest,const unsigned char*src,const unsigned char*out)	{
	
	if (decrypt(dest,src,out) != 0)	{
		
		fprintf(stderr,"Error: Decryption failed\n");	
		
		return 0;
	}

	return 1;
}

// The main function that copies files and automatically encrypts them at destination. Just files without concerns over date,time, chmod permissions, etc.

void ensync(const unsigned char*destpath,const unsigned char*srcpath,const unsigned char*out)	{
	
	struct stat sb;

	memset(&sb,0x0,sizeof(stat));

	if ( (stat(srcpath,&sb) == 0 ) && S_ISREG(sb.st_mode) ) {
		
		encrypt(destpath,srcpath,out);

		do_chmod(destpath,srcpath);

		do_chown(destpath,srcpath);
		
		return;		
	}

	create_dir_clone(destpath,srcpath);

	struct dirent *de = 0; //Pointer for entry

	DIR * dr = opendir(srcpath);

	unsigned char src_fullname[MAXSIZE];

	memset(src_fullname,0x0,MAXSIZE);

	unsigned char dest_fullname[MAXSIZE];
	
	memset(dest_fullname,0x0,MAXSIZE);

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
			
			create_dir_clone(dest_fullname,src_fullname);

			ensync(dest_fullname,src_fullname,out);	
				
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
			
			if(!encp(dest_fullname,src_fullname,out))	{
				
				exit(1);	
				
			}

			do_chmod(dest_fullname,src_fullname);

			do_chown(dest_fullname,src_fullname);

			memset(src_fullname,0x0,MAXSIZE);
			
			memset(dest_fullname,0x0,MAXSIZE);
		}

		else	{

			continue;
		}	

	}

	closedir(dr);

}

// The main function that copies files and automatically decrypts them at destination. Just files without concerns over date,time, chmod permissions, etc.

void dsync(const unsigned char*destpath,const unsigned char*srcpath,const unsigned char*out)	{
	
	struct stat sb;

	memset(&sb,0x0,sizeof(stat));

	if ( (stat(srcpath,&sb) == 0 ) && S_ISREG(sb.st_mode) ) {
		
		decrypt(destpath,srcpath,out);

		do_chmod(destpath,srcpath);

		do_chown(destpath,srcpath);
		
		return;		
	}

	create_dir_clone(destpath,srcpath);

	struct dirent *de = 0; //Pointer for entry

	DIR * dr = opendir(srcpath);

	unsigned char src_fullname[MAXSIZE];

	memset(src_fullname,0x0,MAXSIZE);

	unsigned char dest_fullname[MAXSIZE];
	
	memset(dest_fullname,0x0,MAXSIZE);

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
			
			create_dir_clone(dest_fullname,src_fullname);

			dsync(dest_fullname,src_fullname,out);	
				
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
			
			if(!dencp(dest_fullname,src_fullname,out))	{
				
				exit(1);

			}

			do_chmod(dest_fullname,src_fullname);

			do_chown(dest_fullname,src_fullname);

			memset(src_fullname,0x0,MAXSIZE);
			
			memset(dest_fullname,0x0,MAXSIZE);
		}

		else	{

			continue;
		}	

	}

	closedir(dr);

}

int main(int argc,char**argv)	{
	
	if (sodium_init() != 0)	{
		
		return 1;
	}	
	
	unsigned char out[crypto_secretstream_xchacha20poly1305_KEYBYTES];

	unsigned char salt[crypto_pwhash_SALTBYTES];

	memset(salt,0x0,crypto_pwhash_SALTBYTES);

	sodium_mlock(salt,crypto_pwhash_SALTBYTES);

	//randombytes_buf(salt,crypto_pwhash_SALTBYTES);

	memset(out,0x0,crypto_pwhash_STRBYTES);

	sodium_mlock(out,crypto_pwhash_STRBYTES);

	unsigned char pwd[MAXSIZE];

	memset(pwd,0x0,MAXSIZE);

	unsigned char repwd[MAXSIZE];

	memset(repwd,0x0,MAXSIZE);

	sodium_mlock(pwd,MAXSIZE*sizeof(unsigned char));
	
	sodium_mlock(repwd,MAXSIZE*sizeof(unsigned char));

	size_t n = 0;

	while (!get_pass(pwd,repwd,MAXSIZE,stdin))	{
		
		memset(pwd,0x0,MAXSIZE*sizeof(unsigned char));	
		
		memset(repwd,0x0,MAXSIZE*sizeof(unsigned char));	
	};

	sodium_munlock(repwd,MAXSIZE*sizeof(unsigned char));	

	if(crypto_pwhash(out,crypto_pwhash_STRBYTES,pwd,strnlen(pwd,MAXSIZE),salt,crypto_pwhash_OPSLIMIT_SENSITIVE,crypto_pwhash_MEMLIMIT_SENSITIVE,crypto_pwhash_ALG_DEFAULT) != 0)	{
		
		fprintf(stderr,"Error: Ran out of memory for pwhash\n");

		exit(1);
	}
	
	sodium_munlock(pwd,MAXSIZE*sizeof(unsigned char));	

//	delete(argv[2],argv[1],out);

//	ensync(argv[2],argv[1],out);
	
	dsync(argv[2],argv[1],out);

	sodium_munlock(salt,crypto_pwhash_SALTBYTES);
	
	sodium_munlock(out,crypto_pwhash_STRBYTES);
	
	return 0;
}
