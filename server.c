#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define DICT_LENGTH 370105
#define ALPH_LENGTH 26

int charToIndex(char c){
	return (int)(c) - (int)('a'); 
}

// trie node
struct trieNode{
	struct trieNode *letters[ALPH_LENGTH];
	// true if end of word
	bool isEnd;
};

// create new tree node
struct trieNode *getNode(){
	struct trieNode * pNode = NULL;
	pNode = (struct trieNode *)malloc(sizeof(struct trieNode));
	
	if(pNode){
		int i;
		pNode->isEnd = false;
		for(i = 0; i < ALPH_LENGTH; i++){
			pNode->letters[i] = NULL;
		}
	}
	return pNode;
}

// insert key if not present, if prefix or node mark node
void insert(struct trieNode *root, const char *key){
	int level;
	int length = strlen(key);
	int index;
	
	struct trieNode *pCrawl = root;
	
	for (level = 0; level < length; level++){
		index = charToIndex(key[level]);
		if(!pCrawl->letters[index]){
			pCrawl->letters[index] = getNode();
		}
		
		pCrawl = pCrawl->letters[index];
	}
	
	// mark last node as end
	pCrawl->isEnd = true;
}

// returns true if key present in trie
bool search(struct trieNode *root, const char *key){
	int level;
	int length = strlen(key);
	int index;
	
	struct trieNode *pCrawl = root;
	
	for (level = 0; level < length; level++){
		index = charToIndex(key[level]);
		if(!pCrawl->letters[index]){
			return false;
		}
		
		pCrawl = pCrawl->letters[index];
	}
	return (pCrawl != NULL && pCrawl->isEnd);
}

char keys[DICT_LENGTH][100];

int main(){
	FILE * dictionary;
	dictionary = fopen("words.txt", "r");
	
	
	int i;
	for(i = 0; i < DICT_LENGTH; i++){
		fscanf(dictionary, "%s\n", keys[i]);
	}
	fclose(dictionary);
	
	printf("%s\n", keys[12345]);
	
	struct trieNode *root = getNode();
	
	for(i = 0; i < DICT_LENGTH; i++){
		insert(root, keys[i]);
	}
	
	
	return 0;
}