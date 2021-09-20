#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <dirent.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/file.h>
struct message_s
{
	unsigned char protocol[8]; /* protocol string (8 bytes) */
	unsigned char type;		   /* type (1 byte) */
	unsigned int length;	   /* length (header + payload) (4 bytes) */
} __attribute__((packed, scalar_storage_order("big-endian")));
;

int recvn(int sd, void *buf, int buf_len);
int sendn(int sd, void *buf, int buf_len);
// bool checkFileExistence(char *s);
struct message_s createMessage(unsigned char type,int length);
int checkHeader(struct message_s reply,unsigned char type);

