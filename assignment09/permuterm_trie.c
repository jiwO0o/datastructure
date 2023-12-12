#include <stdio.h>
#include <stdlib.h>	// malloc
#include <string.h>	// strdup
#include <ctype.h>	// isupper(대문자인지 확인), tolower(소문자로 변경)

#define MAX_DEGREE	27 // 'a' ~ 'z' and EOW 최대 27개의 subtree 가질 수 있음
#define EOW			'$' // end of word

// used in the following functions: trieInsert, trieSearch, triePrefixList
// ex) 'a' = 0, 'z' = 25, $ = 26
#define getIndex(x)		(((x) == EOW) ? MAX_DEGREE-1 : ((x) - 'a'))

// TRIE type definition
typedef struct trieNode {
	int 			index; // -1 (non-word), 0, 1, 2, ...
	struct trieNode	*subtrees[MAX_DEGREE];
} TRIE;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

/* Allocates dynamic memory for a trie node and returns its address to caller (초기 index는 -1로 설정)
	return	node pointer
			NULL if overflow
*/
TRIE *trieCreateNode(void);

/* Deletes all data in trie and recycles memory
*/
void trieDestroy( TRIE *root);

/* Inserts new entry into the trie
	return	1 success
			0 failure
*/
// 주의! 엔트리를 중복 삽입하지 않도록 체크해야 함
// 대소문자를 소문자로 통일하여 삽입
// 영문자와 EOW 외 문자를 포함하는 문자열은 삽입하지 않음
int trieInsert( TRIE *root, char *str, int dic_index);

/* Retrieve trie for the requested key
	return	index in dictionary (trie) if key found
			-1 key not found
*/
int trieSearch( TRIE *root, char *str);

/* prints all entries in trie using preorder traversal
*/
void trieList( TRIE *root, char *dic[]);

/* prints all entries starting with str (as prefix) in trie 전방일치
	ex) "ab" -> "abandoned", "abandoning", "abandonment", "abased", ...
	this function uses trieList function
*/
void triePrefixList( TRIE *root, char *str, char *dic[]);

/* makes permuterms for given str
	ex) "abc" -> "abc$", "bc$a", "c$ab", "$abc"
	return	number of permuterms
*/
int make_permuterms( char *str, char *permuterms[]);

/* recycles memory for permuterms
*/
void clear_permuterms( char *permuterms[], int size);

/* wildcard search
	ex) "ab*", "*ab", "a*b", "*ab*"
	this function uses triePrefixList function
*/
void trieSearchWildcard( TRIE *root, char *str, char *dic[]);

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	TRIE *permute_trie;
	char *dic[100000];

	int ret;
	char str[100];
	FILE *fp;
	char *permuterms[100];
	int num_p; // # of permuterms
	int word_index = 0;
	
	if (argc != 2)
	{
		fprintf( stderr, "Usage: %s FILE\n", argv[0]);
		return 1;
	}
	
	fp = fopen( argv[1], "rt");
	if (fp == NULL)
	{
		fprintf( stderr, "File open error: %s\n", argv[1]);
		return 1;
	}
	
	permute_trie = trieCreateNode(); // trie for permuterm index
	
	while (fscanf( fp, "%s", str) != EOF)
	{
		num_p = make_permuterms( str, permuterms);
		
		for (int i = 0; i < num_p; i++)
			trieInsert( permute_trie, permuterms[i], word_index);
		
		clear_permuterms( permuterms, num_p);
		
		dic[word_index++] = strdup( str);
	}
	
	fclose( fp);
	
	printf( "\nQuery: ");
	while (fscanf( stdin, "%s", str) != EOF)
	{
		// wildcard search term
		if (strchr( str, '*')) 
		{
			trieSearchWildcard( permute_trie, str, dic);
		}
		// keyword search
		else 
		{
			ret = trieSearch( permute_trie, str);
			
			if (ret == -1) printf( "[%s] not found!\n", str);
			else printf( "[%s] found!\n", dic[ret]);
		}
		printf( "\nQuery: ");
	}

	for (int i = 0; i < word_index; i++)
		free( dic[i]);
	
	trieDestroy( permute_trie);
	
	return 0;
} //end of main




/* Allocates dynamic memory for a trie node and returns its address to caller (초기 index는 -1로 설정)
	return	node pointer
			NULL if overflow
*/
TRIE *trieCreateNode(void) {
	
	TRIE *maketrie = (TRIE *)malloc(sizeof(TRIE));
	
	if (maketrie == NULL)
		return NULL;
	
	maketrie->index = -1;
	for (int i = 0; i < MAX_DEGREE; i++)
		maketrie->subtrees[i] = NULL;
	
	return maketrie;
} //end of trieCreateNode

/* Deletes all data in trie and recycles memory
*/
void trieDestroy( TRIE *root) {
	
	for (int i = 0; i < MAX_DEGREE; i++)
		if (root->subtrees[i] != NULL)
			trieDestroy(root->subtrees[i]);
		
	free(root);
} //end of trieDestroy

/* Inserts new entry into the trie
	return	1 success
			0 failure
*/
// 주의! 엔트리를 중복 삽입하지 않도록 체크해야 함
// 대소문자를 소문자로 통일하여 삽입
// 영문자와 EOW 외 문자를 포함하는 문자열은 삽입하지 않음
int trieInsert( TRIE *root, char *str, int dic_index) {
	
	TRIE *trienode = root;
	
	for (int i = 0; i < strlen(str); i++) {
		if (getIndex(str[i]) < 0 || getIndex(str[i]) > 26)
			return 0;
		
		if (isupper(str[i]) == 1) //대문자
			str[i] = tolower(str[i]);
	}
	
	for (int i = 0; i < strlen(str); i++) {
		if (trienode->subtrees[getIndex(str[i])] == NULL)
			trienode->subtrees[getIndex(str[i])] = trieCreateNode();
		
		trienode = trienode->subtrees[getIndex(str[i])];
	}
	
	if (trienode->index == -1) {
		trienode->index = dic_index;
		return 1;
	}
	else
		return 0;
} //end of trieInsert

/* Retrieve trie for the requested key
	return	index in dictionary (trie) if key found
			-1 key not found
*/
int trieSearch( TRIE *root, char *str) {
	
	TRIE *trienode = root;
	
	for (int i = 0; i < strlen(str); i++) {
		if (trienode->subtrees[getIndex(str[i])] == NULL)
			return -1;
		else
			trienode = trienode->subtrees[getIndex(str[i])];
	}
	
	return trienode->index;
} //end of trieSearch

/* prints all entries in trie using preorder traversal
*/
void trieList( TRIE *root, char *dic[]) {
	
	if (root->index >= 0)
		printf("%s\n", dic[root->index]);
	
	for (int i = 0; i < MAX_DEGREE; i++)
		if (root->subtrees[i] != NULL)
			trieList(root->subtrees[i], dic);
} //end of trieList

/* prints all entries starting with str (as prefix) in trie 전방일치
	ex) "ab" -> "abandoned", "abandoning", "abandonment", "abased", ...
	this function uses trieList function
*/
void triePrefixList( TRIE *root, char *str, char *dic[]) {
	
	TRIE *trienode = root;
	
	for (int i = 0; i < strlen(str); i++)
		if (trienode->subtrees[getIndex(str[i])] != NULL)
			trienode = trienode->subtrees[getIndex(str[i])];
	
	trieList(trienode, dic);
} //end of triePrefixList

/* makes permuterms for given str
	ex) "abc" -> "abc$", "bc$a", "c$ab", "$abc"
	return	number of permuterms
*/
int make_permuterms( char *str, char *permuterms[]) {
	
	int i, j;
	int len = strlen(str);
	char *per = (char *)malloc(sizeof(char) * (len + 2));
	
	if (per == NULL)
		return -1;
	
	strcpy(per, str);
	
	per[len] = '\0';
	per[len + 1] = '$';
	permuterms[0] = strdup(per);
	
	per[len] = '$';
	per[len + 1] = '\0';
	
	for (i = 1; i < len + 1; i++) {
		char temp = per[0];
		
		for (j = 0; j < len; j++)
			per[j] = per[j + 1];
	
		per[j] = temp;
		
		permuterms[i] = strdup(per);
	}
	
	free(per);
	return len + 1;
} //end of make_permuterms

/* recycles memory for permuterms
*/
void clear_permuterms( char *permuterms[], int size) {
	for (int i = 0; i < size; i++)
		free(permuterms[i]);
} //end of clear_permuterms

/* wildcard search
	ex) "ab*", "*ab", "a*b", "*ab*"
	this function uses triePrefixList function
*/
void trieSearchWildcard( TRIE *root, char *str, char *dic[]) {
	
	int i, j, k;
	int middle;
	int count = 0;
	int len = strlen(str);
	
	for (i = 0; i < len; i++) {
		if (str[i] == '*') {
			count++;
			middle = i;
		}
	}
		
	if (count == 2) //*ab* -> ab*
		str++;
	
	else if (count == 1) {
		if (str[len - 1] == '*') { //ab* -> $ab*
			for (i = len; i > 0; i--)
				str[i] = str[i - 1];
			str[0] = '$';
			str[len + 1] = '\0';
		}
		else if (str[0] == '*') { //*ab -> ab$*
			for (i = 0; i < len - 1; i++)
				str[i] = str[i + 1];
			str[i] = '$';
			str[i + 1] = '*';
			str[i + 2] = '\0';
		}
		else { //a*b -> b$a*
			char temp[100];
			int mid = middle;
			
			for (i = 0; i < mid; i++)
				temp[i] = str[i];
			
			for (i = 0; i < len - mid - 1; i++, middle++)
				str[i] = str[middle + 1];
			str[i] = '$';
			
			for (j = i + 1, k = 0; j < mid + i + 1; j++, k++)
				str[j] = temp[k];
			str[j] = '*';
			str[j + 1] = '\0';
		}
	}
		
	triePrefixList(root, str, dic);
} //end of trieSearchWildcard