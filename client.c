#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>

// address printer
void praddr(struct addrinfo* info){
	printf(". FAMILY: %s ", (info->ai_family == AF_INET ? "IP4" : "IP6"));
	void * addr;
	if(info->ai_family == AF_INET) { //IPv4
		struct sockaddr_in *ip = (struct sockaddr_in *)info->ai_addr;
		addr = &(ip->sin_addr);
	}
	else{ //IPv6
		struct sockaddr_in6 *ip = (struct sockaddr_in6 *)info->ai_addr;
		addr = &(ip->sin6_addr);
	}
	char human_address[INET6_ADDRSTRLEN];
	inet_ntop(info->ai_family, addr, human_address, INET6_ADDRSTRLEN);
	printf(" %s\n", human_address);
}

int prepareAndOpenSocket(char PORT[]){
	// prepare the socket
	struct addrinfo addrSetup;
	memset(&addrSetup, 0, sizeof(addrSetup));
	addrSetup.ai_flags    = AI_PASSIVE;
	addrSetup.ai_family   = AF_INET;
	addrSetup.ai_socktype = SOCK_STREAM;
	
	struct addrinfo *servinfo;
	
	int status = getaddrinfo("127.0.0.1", PORT, &addrSetup, &servinfo);
	
	if(status != 0){
		perror ("Couldn't prepare socket");
		exit(1);
	}
	
	// if it succeeds, print the address
	praddr(servinfo);
	
	// open the socket
	int sock = socket(servinfo->ai_family,
						servinfo->ai_socktype,
						servinfo->ai_protocol);

	if(sock < 0){
		perror("Couldn't open socket");
		exit(2);
	}
	printf("Socket open\n");

	// connect to server
	if(connect(sock, servinfo->ai_addr, servinfo->ai_addrlen) == -1){
		close(sock);
		perror("Failed to connect");
		exit(3);
	}
	
	return sock;
	printf("Connected\n");
}

int main(int argc, char ** argv){
	
	// resolve the port number
	char PORT[4];
	if (argc != 2){
		printf("Wrong number of arguments, only port needed\n");
		exit(-1);
	}
	else{
		strcpy(PORT, argv[1]);
	}

	int sock = prepareAndOpenSocket(PORT);
	
	// receive the message
	int rbytes = 0;
	char buffer[256];
	if ((rbytes = read(sock, buffer, 255)) > 0){
		buffer[rbytes] = 0;
		printf("%s", buffer);
	}
	printf("\n");
	
	// close the socket and exit
	close(sock);
	return 0;
}
