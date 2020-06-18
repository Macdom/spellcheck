#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>

#define ADDRESS "/tmp/spellSock"

int prepareAndOpenSocket(){
	
	int sock = socket(AF_UNIX, SOCK_STREAM, 0);
	if(sock < 0){
		perror ("Couldn't open socket");
		exit(1);
	}
	
	struct sockaddr_un servinfo;

	servinfo.sun_family = AF_UNIX;
	
	strcpy(servinfo.sun_path, ADDRESS);
	printf("Socket open\n");
	
	// connect to server
	if(connect(sock, (struct sockaddr*)&servinfo, SUN_LEN(&servinfo)) == -1){
		close(sock);
		perror("Failed to connect");
		exit(3);
	}
	
	printf("Connected\n");	
	return sock;
}

int main(int argc, char ** argv){
	
	char word[30];
	// check argument count
	if (argc != 2){
		printf("Usage: ./client word\n");
		exit(-1);
	}
	else{
		// read the word to spellcheck
		strcpy(word, argv[1]);
	}

	int sock = prepareAndOpenSocket();
	
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
