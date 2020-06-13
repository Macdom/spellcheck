#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>

#define DICT_LENGTH 370105

int min3(int a, int b, int c){
	if(a < b){
		if (a < c)
			return a;
		return c;
	}
	if(b < c)
		return b;
	return c;
}

int levenshtein(char *w1, char *w2) {
	int x, y;
	int len1 = strlen(w1);
	int len2 = strlen(w2);
	int mat[len2+1][len1+1];
	mat[0][0] = 0;
	for(x = 1; x <= len2; x++){
		mat[x][0] = mat[x-1][0] + 1;
	}
	for(y = 1; y <= len1; y++){
		mat[0][y] = mat[0][y-1] + 1;
	}
	for(x = 1; x <= len2; x++){
		for(y = 1; y <= len1; y++){
			mat[x][y] = min3(mat[x-1][y] + 1, mat[x][y-1] + 1, mat[x-1][y-1] + (w1[y-1] == w2[x-1] ? 0 : 1));
		}
	}
	
	return mat[len2][len1];
}

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

char wordsTable[DICT_LENGTH][100];

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
	
	// load the dictionary
	FILE * dictionary;
	dictionary = fopen("words.txt", "r");
	
	int i;
	for(i = 0; i < DICT_LENGTH; i++){
		fscanf(dictionary, "%s\n", wordsTable[i]);
	}
	fclose(dictionary);
	
	// prepare the socket
	struct addrinfo 	  addrSetup;
	memset(&addrSetup, 0, sizeof(addrSetup));
	addrSetup.ai_flags    = AI_PASSIVE;
	addrSetup.ai_family   = AF_UNSPEC;
	addrSetup.ai_socktype = SOCK_STREAM;
	
	struct addrinfo *servinfo;
	
	int status = getaddrinfo(0, PORT, &addrSetup, &servinfo);
	
	if(status != 0){
		perror ("Couldn't prepare socket");
		exit(1);
	}
	
	// if it succeeds, print the address
	praddr(servinfo);
	
	// open the socket
	printf("Opening socket\n");
	int sock = socket(servinfo->ai_family,
						servinfo->ai_socktype,
						servinfo->ai_protocol);

	if(sock < 0){
		perror("Couldn't open socket");
		exit(2);
	}
	printf("Socket open\n");
	
	// bind socket to port
	if (bind(sock, servinfo->ai_addr, servinfo->ai_addrlen) == -1){
		perror("Couldn't bind socket\n");
		exit(3);
	}
	printf("Socket bound\n");
	
	// listen for client
	if(listen(sock, 10) == -1){
		perror("Error while listening");
		exit(4);
	}
	printf("Listening...\n");
	struct sockaddr client_info;
	unsigned int client_addrlen = sizeof(client_info);
	int newsock = 0;
	while(true){
		int newsock = accept(sock, &client_info, &client_addrlen);
		if(newsock < 0){
			perror("Error while accepting client");
		}
		else{
			printf(". Client accepted, responding\n");
		}
		
		// create buffer
		char buffer [100];
		
		// search for the word
		bool correct = false;
		char word[30] = "gnaoigniewgnoew";
		for(int i = 0; i < DICT_LENGTH; i++){
			if (strcmp(word, wordsTable[i]) == 0){
				correct = true;
				sprintf(buffer, "Correct\n");
				break;
			}
		}
		if(!correct) {
			// build the levenshtein table
			sprintf(buffer, "Incorrect, calculating...\n");
			int levenTable[DICT_LENGTH];
			for(i = 0; i < DICT_LENGTH; i++){
				levenTable[i] = levenshtein(word, wordsTable[i]);			
			}
			// find a potential correction
			char* candidate;
			int levMin = 100;
			for(i = 0; i < DICT_LENGTH; i++){
				if(levenTable[i] < levMin){
					levMin = levenTable[i];
					candidate = wordsTable[i];
				}
			}
			// display the candidate
			if (levMin <= 3){
				char cand [100];
				sprintf(cand, "Don't you mean %s?\n", candidate);
				strcat(buffer, cand);
			}	
			else
				strcat(buffer, "This is certainly not a word.\n");
		}
		
		// send response to client and close socket
		printf("Sending response...\n");
		write(newsock, buffer, strlen(buffer) + 1);
		close(newsock);
	}
	return 0;
}