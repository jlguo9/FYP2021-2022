// Command: ./gameserver <PORT_NUMBER>

#include "game.h"
#include <pthread.h>

#define MAX_CLIENT_NUM 1
#define DEBUG 0

unsigned int listContents(char *buf)
{
	/* list the contents in directory */
	DIR *d;
	struct dirent *dir;
	d = opendir("data");
	if (d)
	{
		int len = 0;
		buf[0] = '\0';
		if (DEBUG) printf("Contents:\n");
		while ((dir = readdir(d)) != NULL)
		{
			strcat(buf, dir->d_name);
			strcat(buf, "\n");
			len += strlen(dir->d_name) + 1;
			if(DEBUG) printf("Name:%s\n", dir->d_name);
		}
		closedir(d);
		return len;
	}
	return (0);
}

void *server(void *arg)
{
	int client_sd = *(int *)arg;
	char buff[100];
	int len;
	struct message_s msg;
	memset(&msg, '\0', sizeof(struct message_s));
	while(1){
		if ((len = recvn(client_sd, &msg, sizeof(msg))) <= 0)
		{
			if (len==0) fprintf(stderr,"Client disconnected.\n");
			else printf("receive error: %s (Errno:%d)\n", strerror(errno), errno);
			pthread_exit(NULL);
		}
		else if(msg.type == 0xCC){
			printf("Client disconnected.\n");
			break;
		}
		/* check request header */
		if (memcmp(msg.protocol, "pingpong", 8) != 0)
		{
			fprintf(stderr,"ERROR: Wrong Protocol");
			close(client_sd);
		}
		else if (msg.type != 0xA1)
		{
			fprintf(stderr,"ERROR: Wrong Type in header");
			close(client_sd);
		}
		else if (msg.length == 0)
		{
			fprintf(stderr,"ERROR: Message Length Equals to 0");
			close(client_sd);
		}
		else if (msg.type == 0xA1)
		{		
			char file_path[] = "./data/test.txt";
			int fd = open(file_path, O_RDONLY);
				
			/* calculate file size */
			if (DEBUG) printf("begin sleep.\n");
			if (DEBUG) sleep(6);
			struct stat st;
			fstat(fd, &st);
			int size = st.st_size;
			if (DEBUG) printf("file found, size: %d bytes.\n", size);
			/* read file contents */
			char *data = (char *)malloc(size + 4);
			memset(data,'\0',size);
			len = read(fd, data, size);

			if (len < 0)
				fprintf(stderr,"Error in reading file!\n");
			else
			{
				if(DEBUG)printf("Read %d bytes of data\n", len);
				/* prepare header for data */
				struct message_s file_data = createMessage(0xFF, sizeof(file_data) + size);
				/*send data*/
				if ((len = sendn(client_sd, &file_data, sizeof(file_data))) < 0)
				{
					printf("Send Error: %s (Errno:%d)\n", strerror(errno), errno);
					pthread_exit(NULL);
				}
				if (DEBUG) printf("Data header sent. Begin send data!!\n");
				if ((len = sendn(client_sd, data, size)) < 0)
				{
					printf("Send Error: %s (Errno:%d)\n", strerror(errno), errno);
					pthread_exit(NULL);
				}
				printf("%d bytes sucessfullly sent.\n", len);
			}
			free(data);
			close(fd);			
		}
	}
	close(client_sd);
}

int main(int argc, char **argv)
{	
	if(argc<2) {fprintf(stderr,"too few argument"); return 0;}
	int sd = socket(AF_INET, SOCK_STREAM, 0);
	long val = 1;
	if (setsockopt(sd,SOL_SOCKET,SO_REUSEADDR, &val, sizeof(long)) == -1)
    	{
        	perror("setsocopt");
        	exit(1);
    	}
	int client_sd;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
	memset(&server_addr, '\0', sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	int port=atoi(argv[1]);
	server_addr.sin_port = htons(port);
	if (bind(sd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	{
		printf("bind error: %s (Errno:%d)\n", strerror(errno), errno);
		exit(0);
	}
	if (listen(sd, MAX_CLIENT_NUM) < 0)
	{
		printf("listen error: %s (Errno:%d)\n", strerror(errno), errno);
		exit(0);
	}
	int *thread_sd;
	int addr_len = sizeof(client_addr);
	while (1)
	{
		if (DEBUG) printf("Waiting to accept connection.\n");
		if ((client_sd = accept(sd, (struct sockaddr *)&client_addr, &addr_len)) < 0)
		{
			printf("accept error: %s (Errno:%d)\n", strerror(errno), errno);
			exit(0);
		}
		if (DEBUG) printf("Accept connection from %d\n", client_addr.sin_addr.s_addr);
		thread_sd = (int *)malloc(sizeof(int));
		*thread_sd = client_sd;
		pthread_t thread;
		pthread_create(&thread, NULL, server, thread_sd);
	}

	return 0;
}
