#include "game.h"
int recvn(int sd, void *buf, int buf_len)
{
	// printf("Start receiving!\n");
	int n_left = buf_len;
	int n;
	while (n_left > 0)
	{
		if ((n = recv(sd, buf + (buf_len - n_left), n_left, 0)) < 0)
		{
			if (errno == EINTR)
				n = 0; //interrupt
			else
				return -1;
		}
		else if (n == 0)
			return 0;
		n_left -= n;
	}
	return buf_len;
}
int sendn(int sd, void *buf, int buf_len)
{
	int n_left = buf_len;
	int n;
	while (n_left > 0)
	{
		if ((n = send(sd, buf + (buf_len - n_left), n_left, 0)) < 0)
		{
			if (errno == EINTR)
				n = 0;
			else
				return -1;
		}
		else if (n == 0)
			return 0;
		n_left -= n;
	}
    return buf_len;
}
/*
bool checkFileExistence(char *s)
{
	DIR *d;
	struct dirent *dir;
	d = opendir("data");
	if (d)
	{
		while ((dir = readdir(d)) != NULL)
		{
			if (strcmp(s, dir->d_name) == 0)
			{
				closedir(d);
				return true;
			}
		}
		closedir(d);
		return false;
	}
}*/

struct message_s createMessage(unsigned char type,int length){
	/* return: a header with specified attributes */
	struct message_s m;
	memset(&m, 0, sizeof(struct message_s));
	strcpy(m.protocol, "pingpong");
	m.type=type;
	m.length=length;
	return m;
}

int checkHeader(struct message_s reply,unsigned char type)
/* Check whether the header information matches. return 0 if matched, -1 otherwise. */
{
	if (memcmp(reply.protocol, "pingpong", 5) != 0)
		{
			printf("ERROR: Wrong Protocol\n");
			return -1;
		}
	if (reply.type != type)
		{
			printf("ERROR: Wrong Type\n");
			return -1;
		}
	return 0;
};
