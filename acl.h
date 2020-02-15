#ifndef __ATTR_H__
#define __ATTR_H__

#define MAXSIZE	2048
void lsa(char*basepath);

void do_chmod(const unsigned char*src,const unsigned char*dest);

void do_chown(const unsigned char*src,const unsigned char*dest);
#endif	// __ATTR_H__
