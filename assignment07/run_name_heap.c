#include <stdio.h>
#include <string.h> // strdup
#include <stdlib.h>
#include "adt_heap.h"

// User structure type definition
typedef struct 
{
	char	*name;	// 이름
	char	sex;	// 성별 M or F
	int		freq;	// 빈도
} tName;

////////////////////////////////////////////////////////////////////////////////
tName *createName( char *name, char sex, int freq)
{
	tName *newName = malloc( sizeof( tName));
	
	if (newName == NULL) return NULL;
	
	newName->name = strdup( name);
	newName->sex = sex;
	newName->freq = freq;
	
	return newName;
}

void destroyName( void *pName)
{
	free( ((tName *)pName)->name); // string
	free( pName); // tName
}

// compares two names in name structures
int cmpName( const void *pName1, const void *pName2)
{
	int ret = strcmp( ((tName *)pName1)->name, ((tName *)pName2)->name);
	if (ret != 0) return ret;

	return ((tName *)pName1)->sex - ((tName *)pName2)->sex;
}

// prints contents of name structure
void print_name( const void *dataPtr)
{
	printf("%s\t%c\t%d\n", ((tName *)dataPtr)->name, ((tName *)dataPtr)->sex, ((tName *)dataPtr)->freq);
}

// prints name in name structure
void print_name_only( const void *dataPtr)
{
	printf(" %s", ((tName *)dataPtr)->name);
}

////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	HEAP *heap;
	void *dataPtr;
	
	char name[100];
	char sex;
	int freq;
	tName *pName;
	
	if (argc != 2)
	{
		fprintf(stderr, "usage: %s FILE\n", argv[0]);
		return 1;
	}
	
	FILE *fp;
	
	if ((fp = fopen(argv[1], "rt")) == NULL)
	{
		fprintf( stderr, "file open error: %s\n", argv[1]);
		return 1;
	}
	
	heap = heap_Create(cmpName); // initial capacity = 10
	
	printf("Insert:");
	
	while(fscanf(fp, "%s\t%c\t%d", name, &sex, &freq) != EOF)
	{
		printf(" %s", name);
		pName = createName(name, sex, freq);
		
		// insert function call
		if (heap_Insert(heap, pName) == 0) {
			destroyName(pName);
			break;
		}
 	}
	printf("\n");
	fclose(fp);
	
	printf("Heap:  ");
	heap_Print(heap, print_name_only);
	printf("\n");
	
	printf( "Delete: ");
	
	for (int i = 0; i < 10 && !heap_Empty(heap); i++)
	{
		// delete function call
		heap_Delete(heap, &dataPtr);

		print_name(dataPtr);

		destroyName(dataPtr);
 	}
	printf("\n");
	
	heap_Destroy(heap, destroyName);
	
	return 0;
}
