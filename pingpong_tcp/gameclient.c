// Command: ./gameclient <server ip addr> <server port>

#include "game.h"
#include <arpa/inet.h>
#define DEBUG 0

int main(int argc, char **argv)
{
	if (argc < 3)
	{
		fprintf(stderr, "too few argument");
		return 0;
	}
	int sd = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in server_addr;
	memset(&server_addr, '\0', sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(argv[1]); //get s_addr from argv[1]
	server_addr.sin_port = htons(atoi(argv[2]));	  //get port from argv[2]

	/*connect with server*/
	if (connect(sd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	{
		printf("connection error: %s (Errno:%d)\n", strerror(errno), errno);
		exit(0);
	}

	/*request a file*/
	while(1)
	{	
		printf("Press enter to send a request; or press \"q\" to quit:");
		char c;
		scanf("%c",&c);
		if(c == 'q'){
			break;
		}

		struct message_s msg = createMessage(0xA1, sizeof(struct message_s));

		struct message_s reply; //store server reply
		memset(&reply, '\0', sizeof(struct message_s));

		int len;
		/*send the request*/
		if ((len = sendn(sd, &msg, sizeof(msg))) < 0)
		{
			printf("Send Error: %s (Errno:%d)\n", strerror(errno), errno);
			exit(0);
		}
		if (DEBUG) printf("Request Sent!\n");

		/*receive FILE_DATA*/
		memset(&reply, '\0', sizeof(struct message_s));
		if ((len = recvn(sd, &reply, sizeof(reply))) < 0)
		{
			printf("Receive Error: %s (Errno:%d)\n", strerror(errno), errno);
			exit(0);
		}
		if (checkHeader(reply, 0xFF) < 0)
		{
			close(sd);
			return 0;
		}
		if (DEBUG) printf("Server Replied FILE_DATA header.\n");

		/*receive file*/
		char *payload = (char *)malloc(reply.length - sizeof(reply));
		//printf("file size should be: %d\n",reply.length-10);
		if ((len = recvn(sd, payload, reply.length - sizeof(reply))) < 0)
		{
			printf("Receive Error: %s (Errno:%d)\n", strerror(errno), errno);
			exit(0);
		}
		if (DEBUG) printf("Server Replied file contents.\n");
		/*write into local file
		int fd = open(filename, O_CREAT | O_RDWR | O_TRUNC, S_IRWXU);
		int size = write(fd, payload, reply.length - sizeof(reply));*/
		printf("SUCCESSFULLY Received a file from the server.\n");
		free(payload);
		// close(fd);
	}
	// Inform the server and close the connection
	struct message_s msg = createMessage(0xCC, sizeof(struct message_s));
	if (sendn(sd, &msg, sizeof(msg)) < 0){
		printf("Send Error: %s (Errno:%d)\n", strerror(errno), errno);
		exit(0);
	}
	close(sd);
	return 0;
}