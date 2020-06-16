#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>

int prepareAndOpenSocket(char* name){
	
	struct sockaddr_un servinfo;
	int sock = socket(PF_UNIX, SOCK_STREAM, 0);
	if(sock != 0){
		printf("%d\n", sock);
		perror ("Couldn't open socket");
		exit(1);
	}
	
	memset(&servinfo, 0, sizeof(struct sockaddr_un));
	servinfo.sun_family = AF_UNIX;
	strncpy(servinfo.sun_path, name, sizeof(servinfo.sun_path) - 1);
	printf("Socket open\n");

	// connect to server
	if(connect(sock, (struct sockaddr*)&servinfo, SUN_LEN(&servinfo) < 0)){
		close(sock);
		perror("Failed to connect");
		exit(3);
	}
	
	return sock;
	printf("Connected\n");
}

int main(int argc, char ** argv){
	
	char PORT[4], word[30];
	// check argument count
	if (argc != 3){
		printf("Usage: ./client port word\n");
		exit(-1);
	}
	else{
		// resolve port number
		strcpy(PORT, argv[1]);
		// read the word to spellcheck
		strcpy(word, argv[2]);
	}

	int sock = prepareAndOpenSocket("spellSock");
	
	// send the word
	write(sock, word, strlen(word) + 1);
	printf("Sent word: %s, waiting for response\n", word);
	
	// receive the message
	int rbytes = 0;
	char responseBuffer[100];
	if ((rbytes = read(sock, responseBuffer, 100)) > 0){
		responseBuffer[rbytes] = 0;
		printf("%s", responseBuffer);
	}
	printf("\n");
	
	// close the socket and exit
	close(sock);
	return 0;
}
