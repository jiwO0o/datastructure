#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_YEAR_DURATION	10	// 기간
#define SORT_BY_NAME	0 // 이름 순 정렬
#define SORT_BY_FREQ	1 // 빈도 순 정렬

// 이름 구조체 선언
typedef struct {
	char	name[20];		// 이름
	char	sex;			// 성별 M or F
	int		total_freq;		// 연도별 빈도 합
	int		freq[MAX_YEAR_DURATION]; // 연도별 빈도
} tName;

////////////////////////////////////////////////////////////////////////////////
// LIST type definition
typedef struct node
{
	tName		*dataPtr;
	struct node	*link; // 이름순 리스트를 위한 포인터
	struct node	*link2; // 빈도순 리스트를 위한 포인터
} NODE;

typedef struct
{
	int		count;
	NODE	*head; // 이름순 리스트의 첫번째 노드에 대한 포인터
	NODE	*head2; // 빈도순 리스트의 첫번째 노드에 대한 포인터
} LIST;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

// Allocates dynamic memory for a list head node and returns its address to caller
// return	head node pointer
// 			NULL if overflow
LIST *createList(void);

//  이름 리스트에 할당된 메모리를 해제 (head node, data node, name data)
void destroyList( LIST *pList);

// internal insert function
// inserts data into a new node
// return	1 if successful
// 			0 if memory overflow
static int _insert( LIST *pList, NODE *pPre, tName *dataInPtr);

// internal function
// connects node into a frequency list
static void _link_by_freq( LIST *pList, NODE *pPre, NODE *pLoc);

// internal search function
// searches list and passes back address of node containing target and its logical predecessor
// return	1 found
// 			0 not found
static int _search( LIST *pList, NODE **pPre, NODE **pLoc, tName *pArgu); //이름순으로 된 리스트에서 찾기

static int _search_by_freq( LIST *pList, NODE **pPre, NODE **pLoc, tName *pArgu); //빈도순으로 된 리스트에서 찾기. 위에랑 기능은 같음

// 이름 구조체를 위한 메모리를 할당하고, 이름(name)과 성별(sex)을 초기화
// return	할당된 이름 구조체에 대한 pointer
//			NULL if overflow
tName *createName( char *name, char sex); 

//  이름 구조체에 할당된 메모리를 해제
void destroyName( tName *pNode);

////////////////////////////////////////////////////////////////////////////////
// 입력 파일을 읽어 이름 정보(연도, 이름, 성별, 빈도)를 이름 리스트에 저장
// 이미 리스트에 존재하는(저장된) 이름은 해당 연도의 빈도만 저장
// 새로 등장한 이름은 리스트에 추가
// 주의사항: 동일 이름이 남/여 각각 사용될 수 있으므로, 이름과 성별을 구별해야 함
// 주의사항: 정렬 리스트(ordered list)를 유지해야 함
// start_year : 시작 연도 (2009)
void load_names( FILE *fp, int start_year, LIST *list);

// 이름순 리스트를 순회하며 빈도 순 리스트로 연결. 포인터 연산만 해서 연결하면 됨 (새로운 노드 추가x 삽입과 비슷)
void connect_by_frequency(LIST *list);

// 이름 리스트를 화면에 출력
void print_names( LIST *pList, int num_year); // 이름순
void print_names_by_freq( LIST *pList, int num_year); // 빈도순

////////////////////////////////////////////////////////////////////////////////
// compares two names in name structures
// for _search function
// 정렬 기준 : 이름(1순위), 성별(2순위)
static int compare_by_name(const tName *pName1, const tName *pName2)
{
	int ret = strcmp( pName1->name, pName2->name);
	if (ret != 0) return ret;

	return pName1->sex - pName2->sex;
}

// for _search_by_freq function
// 정렬 기준 : 빈도 내림차순(1순위), 이름(2순위), 성별(3순위)
static int compare_by_freq(const tName *pName1, const tName *pName2)
{
	int ret = pName2->total_freq - pName1->total_freq;
	if (ret != 0) return ret;
	
	return compare_by_name(pName1, pName2);
}

////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	LIST *list;
	int option;
	FILE *fp;
	
	if (argc != 3)
	{
		fprintf( stderr, "Usage: %s option FILE\n\n", argv[0]);
		fprintf( stderr, "option\n\t-n\t\tsort by name\n\t-f\t\tsort by frequency\n");
		return 1;
	}

	if (strcmp( argv[1], "-n") == 0) option = SORT_BY_NAME;
	else if (strcmp( argv[1], "-f") == 0) option = SORT_BY_FREQ;
	else {
		fprintf( stderr, "unknown option : %s\n", argv[1]);
		return 1;
	}

	if ((fp = fopen( argv[2], "r")) == NULL) 
	{
		fprintf( stderr, "cannot open file : %s\n", argv[2]);
		return 1;
	}
	
	// creates an empty list
	list = createList();
	if (!list)
	{
		printf( "Cannot create list\n");
		return 100;
	}
	
	// 입력 파일로부터 이름 정보를 리스트에 저장 (이름순 리스트 구축)
	load_names( fp, 2009, list);
	
	fclose( fp);

	if (option == SORT_BY_NAME) {
		
		// 이름순 리스트를 화면에 출력
		print_names( list, MAX_YEAR_DURATION);
	}
	else { // SORT_BY_FREQ
	
		// 빈도순 리스트 연결
		connect_by_frequency( list);
		
		// 빈도순 리스트를 화면에 출력
		print_names_by_freq( list, MAX_YEAR_DURATION);
	}
	
	// 이름 리스트 메모리 해제
	destroyList( list);
	
	return 0;
} //end of main

// Allocates dynamic memory for a list head node and returns its address to caller
// return	head node pointer
// 			NULL if overflow
LIST *createList(void) {
	
	LIST *headnode = (LIST *)malloc(sizeof(LIST));
	
	if (headnode == NULL)
		return NULL;
	
	headnode->count = 0;
	headnode->head = NULL;
	headnode->head2 = NULL;
	
	return headnode;
} //end of createList

//  이름 리스트에 할당된 메모리를 해제 (head node, data node, name data)
void destroyList( LIST *pList) {
	
	while (pList->head != NULL) {
		NODE *pLoc = pList->head;
		pList->head = pLoc->link;
		destroyName(pLoc->dataPtr);
		pLoc->link = NULL;
		pLoc->link2 = NULL;
		free(pLoc);
	}
	
	pList->count = 0;
	pList->head2 = NULL;
	free(pList);
} //end of destroyList

// internal insert function
// inserts data into a new node
// return	1 if successful
// 			0 if memory overflow
static int _insert( LIST *pList, NODE *pPre, tName *dataInPtr) {
	
	NODE *pNew = (NODE *)malloc(sizeof(NODE));
	
	if (pNew == NULL)
		return 0;
	
	pNew->dataPtr = dataInPtr;
	
	if (pPre == NULL) {
		pNew->link = pList->head;
		pList->head = pNew;
	}
	else {
		pNew->link = pPre->link;
		pPre->link = pNew;
	}
	
	pNew->link2 = NULL;
	pList->count++;
	return 1;
} //end of _insert

// internal function
// connects node into a frequency list
static void _link_by_freq( LIST *pList, NODE *pPre, NODE *pLoc) {
	
	if (pPre == NULL) { //pLoc이 첫노드
		pLoc->link2 = pList->head2;
		pList->head2 = pLoc;
	}
	
	else if (pPre->link2 == NULL) { //맨마지막 노드가 pLoc
		pPre->link2 = pLoc;
		pLoc->link2 = NULL;
	}
	
	else {
		pLoc->link2 = pPre->link2;
		pPre->link2 = pLoc;
	}
} //end of _link_by_freq

// internal search function
// searches list and passes back address of node containing target and its logical predecessor
// return	1 found
// 			0 not found
static int _search( LIST *pList, NODE **pPre, NODE **pLoc, tName *pArgu) { //이름순으로 된 리스트에서 찾기
	
	*pPre = NULL;
	*pLoc = pList->head;
	
	while ((*pLoc) != NULL && compare_by_name((*pLoc)->dataPtr, pArgu) < 0) { 
		*pPre = *pLoc;
		*pLoc = (*pLoc)->link;
	}
	
	if (*pLoc == NULL)
		return 0;
	else {
		if (compare_by_name((*pLoc)->dataPtr, pArgu) == 0)
			return 1;
		else
			return 0;
	}
} //end of _search

static int _search_by_freq( LIST *pList, NODE **pPre, NODE **pLoc, tName *pArgu) { //빈도순으로 된 리스트에서 찾기. 위에랑 기능은 같음

	*pPre = NULL;
	*pLoc = pList->head2;

	while (compare_by_freq((*pLoc)->dataPtr, pArgu) < 0) { //pArgu의 빈도가 작아서 뒤로 가야하는상황
		if ((*pLoc)->link2 == NULL)
			break;
		else {
			*pPre = *pLoc;
			*pLoc = (*pLoc)->link2;
		}
	}
	
	if ((*pLoc)->link2 == NULL) //리스트 끝까지 갔는데도 발견x pArgu가 맨뒤(pLoc 바로 뒤)에 연결되어야함
		return 1;
		
	else if (compare_by_freq((*pLoc)->dataPtr, pArgu) > 0) {
		if (*pPre == NULL) //pArgu가 맨앞에 연결되어야함
			return -1;
		else //pArgu가 pLoc 바로 앞에 연결되어야함
			return 0;
	}
} //end of _search_by_freq

// 이름 구조체를 위한 메모리를 할당하고, 이름(name)과 성별(sex)을 초기화
// return	할당된 이름 구조체에 대한 pointer
//			NULL if overflow
tName *createName( char *name, char sex) {
	
	tName *myname = (tName *)malloc(sizeof(tName));
	if (myname == NULL)
		return NULL;
	
	myname->sex = sex;
	strcpy(myname->name, name);
	
	return myname;
} //end of createName

//  이름 구조체에 할당된 메모리를 해제
void destroyName( tName *pNode) {
	pNode->sex = 0;
	pNode->total_freq = 0;
	memset(pNode->name, 0, sizeof(pNode->name));
	memset(pNode->freq, 0, sizeof(pNode->freq));
	
	free(pNode);
} //end of destroyName

////////////////////////////////////////////////////////////////////////////////
// 입력 파일을 읽어 이름 정보(연도, 이름, 성별, 빈도)를 이름 리스트에 저장
// 이미 리스트에 존재하는(저장된) 이름은 해당 연도의 빈도만 저장
// 새로 등장한 이름은 리스트에 추가
// 주의사항: 동일 이름이 남/여 각각 사용될 수 있으므로, 이름과 성별을 구별해야 함
// 주의사항: 정렬 리스트(ordered list)를 유지해야 함
// start_year : 시작 연도 (2009)
void load_names( FILE *fp, int start_year, LIST *list) {
	
	int year;
	char tempname[20];
	char tempsex;
	int tempfreq;
	int searchcheck;
	int insertcheck;
	tName *pname;
	NODE *pPre = NULL;
	NODE *pLoc = NULL;
	
	while (fscanf(fp, "%d %s %c %d", &year, tempname, &tempsex, &tempfreq) != EOF) {
		pname = createName(tempname, tempsex);
		searchcheck = _search(list, &pPre, &pLoc, pname);
		
		if (searchcheck == 1) { //발견됨
			pLoc->dataPtr->total_freq += tempfreq;
			pLoc->dataPtr->freq[year - start_year] += tempfreq;
		}
		
		else { //발견안됨
			insertcheck = _insert(list, pPre, pname);
			
			if (insertcheck == 0)
				exit(0);
			
			memset(pname->freq, 0, sizeof(pname->freq));
			pname->total_freq = tempfreq;
			pname->freq[year - start_year] = tempfreq;
		}
	}
} //end of load_names

// 이름순 리스트를 순회하며 빈도 순 리스트로 연결. 포인터 연산만 해서 연결하면 됨 (새로운 노드 추가x 삽입과 비슷) 빨간선처럼 정렬
void connect_by_frequency(LIST *list) {
	
	list->head2 = list->head;
	
	int searchcheck2;
	NODE *pPre;
	NODE *pLoc;
	NODE *walkpPre = list->head2;
	NODE *walkpLoc = walkpPre->link;
	
	while (walkpLoc != NULL) {
		searchcheck2 = _search_by_freq(list, &pPre, &pLoc, walkpLoc->dataPtr);
		
		if (searchcheck2 == 1) //리스트 끝까지 갔는데도 발견x pArgu가 맨뒤(pLoc 바로 뒤)에 연결되어야함
			_link_by_freq(list, pLoc, walkpLoc);
		else if (searchcheck2 == -1) //pArgu가 맨앞에 연결되어야함
			_link_by_freq(list, pPre, walkpLoc);
		else if (searchcheck2 == 0) //pArgu가 pLoc 바로 앞에 연결되어야함
			_link_by_freq(list, pPre, walkpLoc);
		
		walkpLoc = walkpLoc->link;
	}
} //end of connect_by_frequency

// 이름 리스트를 화면에 출력
void print_names( LIST *pList, int num_year) { // 이름순
	
	NODE *pos = pList->head;
	
	while (pos != NULL) {
		printf("%s	%c	%d	", pos->dataPtr->name, pos->dataPtr->sex, pos->dataPtr->total_freq);
		for (int i = 0; i < num_year; i++)
			printf("%d	", pos->dataPtr->freq[i]);
		printf("\n");

		pos = pos->link;
	}
} //end of print_names

void print_names_by_freq( LIST *pList, int num_year) { // 빈도순

	NODE *pos = pList->head2;
	
	while (pos != NULL) {
		printf("%s	%c	%d	", pos->dataPtr->name, pos->dataPtr->sex, pos->dataPtr->total_freq);
		for (int i = 0; i < num_year; i++)
			printf("%d	", pos->dataPtr->freq[i]);
		printf("\n");

		pos = pos->link2;
	}
} //end of print_names_by_freq