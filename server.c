#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdbool.h>

#define DICT_LENGTH 370105
#define ADDRESS "/tmp/spellSock"

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

void loadFileToArray(char* name, char array[DICT_LENGTH][100]){
	FILE * f;
	f = fopen(name, "r");
	
	int i;
	for(i = 0; i < DICT_LENGTH; i++){
		fscanf(f, "%s\n", array[i]);
	}
	fclose(f);
}

int openSocket(){
	// open the socket
	printf("Opening socket\n");
	int sock = socket(AF_UNIX, SOCK_STREAM, 0);

	if(sock < 0){
		perror("Couldn't open socket");
		exit(1);
	}
	printf("Socket open\n");
	return sock;
}	

struct sockaddr_un prepareSocket(){
	struct sockaddr_un servinfo;
	servinfo.sun_family = AF_UNIX;
	
	strcpy(servinfo.sun_path, ADDRESS);
	
	return servinfo;
}

bool wordSearch(char* word, char array[DICT_LENGTH][100]){
	for(int i = 0; i < DICT_LENGTH; i++){
		if (strcmp(word, array[i]) == 0){
			return true;
		}
	}
	return false;
}

void findCandidate(char* word, char* buffer, char array[DICT_LENGTH][100]){
	// build the levenshtein table
	sprintf(buffer, "Incorrect, calculating...\n");
	int levenTable[DICT_LENGTH];
	int i;
	for(i = 0; i < DICT_LENGTH; i++){
		levenTable[i] = levenshtein(word, array[i]);			
	}
	// find a potential correction
	char* candidate;
	int levMin = 100;
	for(i = 0; i < DICT_LENGTH; i++){
		if(levenTable[i] < levMin){
			levMin = levenTable[i];
			candidate = array[i];
		}
	}
	// display the candidate
	if (levMin <= 3){
		char cand [100];
		sprintf(cand, "Don't you mean %s?\n", candidate);
		strcat(buffer, cand);
	}	
	else{
		strcat(buffer, "This is certainly not a word.\n");
	}
}


// create on heap to avoid stack overflow
char wordsTable[DICT_LENGTH][100];

int main(){
	
	// load the dictionary
	loadFileToArray("words.txt", wordsTable);
	
	// open and prepare socket
	int sock = openSocket();
	struct sockaddr_un servinfo = prepareSocket();
	
	// bind socket to port
	unlink(ADDRESS);
	if (bind(sock, (struct sockaddr *)&servinfo, SUN_LEN(&servinfo)) == -1){
		perror("Couldn't bind socket\n");
		exit(2);
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
			printf("Client accepted, responding\n");
		}
		
		// receive the word
		int rbytes = 0;
		char word[31];
		if ((rbytes = read(newsock, word, 30)) > 0){
			word[rbytes] = 0;
		}
		printf("Received word: %s\n", word);
		// create response buffer
		char responseBuffer [100];
		
		// process the word
		bool correct = wordSearch(word, wordsTable);
		if(correct)
			sprintf(responseBuffer, "Correct\n");
		else{
			findCandidate(word, responseBuffer, wordsTable);
		}
		
		// send response to client and close socket
		printf("Sending response...\n");
		write(newsock, responseBuffer, strlen(responseBuffer) + 1);
		printf("Response sent.\n");
		close(newsock);
	}
	return 0;
}