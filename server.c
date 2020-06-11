#include <stdio.h>
#include <string.h>
#include <stdbool.h>

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

char keys[DICT_LENGTH][100];

int main(int argc, char ** argv){
	// load the dictionary
	FILE * dictionary;
	dictionary = fopen("words.txt", "r");
	
	int i;
	for(i = 0; i < DICT_LENGTH; i++){
		fscanf(dictionary, "%s\n", keys[i]);
	}
	fclose(dictionary);
	
	// search for the word
	bool correct = false;
	for(int i = 0; i < DICT_LENGTH; i++){
		if (strcmp(argv[1], keys[i]) == 0){
			correct = true;
			printf("Correct\n");
			break;
		}
	}
	if(!correct) {
		// build the levenshtein table
		printf("Incorrect, calculating...\n");
		int levenTable[DICT_LENGTH];
		for(i = 0; i < DICT_LENGTH; i++){
			levenTable[i] = levenshtein(argv[1], keys[i]);			
		}
		printf("a\n");
		// find a potential correction
		char* candidate;
		int levMin = 100;
		for(i = 0; i < DICT_LENGTH; i++){
			if(levenTable[i] < levMin){
				levMin = levenTable[i];
				candidate = keys[i];
			}
		}
		// display the candidate
		if (levMin <= 3)
			printf("Don't you mean %s?\n", candidate);
		else
			printf("This is certainly not a word.\n");
	}
	
	
	return 0;
}