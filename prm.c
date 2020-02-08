#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sodium.h>

#define CHUNK_SIZE	4096

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
		
		fprintf(stderr,"Error: Failed to read source file\n");
		
		exit(1);

	}

	if ( ( out = fopen(dest,"wb")) == NULL )	{
		
		fprintf(stderr,"Error: Failed to create target file\n");
		
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
		fprintf(stderr,"Error: Failed to read source file\n");
		
		exit(1);

	}

	if ( ( out = fopen(dest,"wb")) == NULL )	{
		fprintf(stderr,"Error: Failed to create target file\n");
		
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
	fclose(in);
	fclose(out);

	return ret;

}

int main(void)	{
	
	unsigned char key[crypto_secretstream_xchacha20poly1305_KEYBYTES];

	if (sodium_init() != 0)	{
		return 1;
	}	
	crypto_secretstream_xchacha20poly1305_keygen(key);
	
	if (encrypt("/home/tsalim/git/prm/test.c.prm","/home/tsalim/git/prm/test.c",key) != 0)	{
		
		fprintf(stderr,"Error: Encryption failed\n");	

		return 1;
	}

	return 0;
}
