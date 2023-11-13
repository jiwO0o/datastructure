#include <stdlib.h> // malloc

#include "adt_dlist.h"

// internal insert function
// inserts data into list
// for addNode function
// return	1 if successful
// 			0 if memory overflow
static int _insert( LIST *pList, NODE *pPre, void *dataInPtr);

// internal delete function
// deletes data from list and saves the (deleted) data to dataOutPtr
// for removeNode function
static void _delete( LIST *pList, NODE *pPre, NODE *pLoc, void **dataOutPtr);

// internal search function
// searches list and passes back address of node containing target and its logical predecessor
// for addNode, removeNode, searchNode functions
// return	1 found
// 			0 not found
static int _search( LIST *pList, NODE **pPre, NODE **pLoc, void *pArgu);





// Allocates dynamic memory for a list head node and returns its address to caller
// return	head node pointer
// 			NULL if overflow
LIST *createList( int (*compare)(const void *, const void *)) {
	
	LIST *headnode = (LIST *)malloc(sizeof(LIST));
	
	if (headnode == NULL)
		return NULL;
	
	headnode->count = 0;
	headnode->head = NULL;
	headnode->rear = NULL;
	headnode->compare = compare;
	
	return headnode;
} //end of createList

//  이름 리스트에 할당된 메모리를 해제 (head node, data node, name data)
void destroyList( LIST *pList, void (*callback)(void *)) {
	
	while (pList->head != NULL) {
		NODE *pLoc = pList->head;
		pList->head = pLoc->rlink;
		callback(pLoc->dataPtr);
		pLoc->rlink = NULL;
		pLoc->llink = NULL;
		free(pLoc);
	}
	
	pList->count = 0;
	pList->rear = NULL;
	free(pList);
} //end of destroyList

// Inserts data into list
//	return	0 if overflow
//			1 if successful
//			2 if duplicated key
int addNode( LIST *pList, void *dataInPtr) {
	
	int searchcheck;
	int insertcheck;
	NODE *pPre;
	NODE *pLoc;
	
	searchcheck = _search(pList, &pPre, &pLoc, dataInPtr);
	
	if (searchcheck == 1)
		return 2;
	else {
		insertcheck = _insert(pList, pPre, dataInPtr);
		
		if (insertcheck == 1)
			return 1;
		else if (insertcheck == 0)
			return 0;
	}
} //end of addNode

// Removes data from list
//	return	0 not found
//			1 deleted
int removeNode( LIST *pList, void *keyPtr, void **dataOutPtr) {
	
	NODE *pPre;
	NODE *pLoc;
	int searchcheck;
	
	searchcheck = _search(pList, &pPre, &pLoc, keyPtr);
	
	if (searchcheck == 0)
		return 0;
	else {
		_delete(pList, pPre, pLoc, dataOutPtr);
		return 1;
	}
} //end of removeNode

// interface to search function
//	pArgu	key being sought
//	dataOutPtr	contains found data
//	return	1 successful
//			0 not found
int searchNode( LIST *pList, void *pArgu, void **dataOutPtr) {
	
	int searchcheck;
	NODE *pPre;
	NODE *pLoc;
	
	searchcheck = _search(pList, &pPre, &pLoc, pArgu);
	*dataOutPtr = pLoc->dataPtr;
	
	return searchcheck;
} //end of searchNode

// returns number of nodes in list
int countList( LIST *pList) {
	return pList->count;
} //end of countList

// returns	1 empty
//			0 list has data
int emptyList( LIST *pList) {
	if (pList->count == 0)
		return 1;
	else
		return 0;
} //end of emptyList

// traverses data from list (forward)
void traverseList( LIST *pList, void (*callback)(const void *)) {
	NODE *walk = pList->head;
	
	while (walk != NULL) {
		callback(walk->dataPtr);
		walk = walk->rlink;
	}
} //end of traverseList

// traverses data from list (backward)
void traverseListR( LIST *pList, void (*callback)(const void *)) {
	NODE *walk = pList->rear;
	
	while (walk != NULL) {
		callback(walk->dataPtr);
		walk = walk->llink;
	}
} //end of traverseListR

// internal insert function
// inserts data into list
// for addNode function
// return	1 if successful
// 			0 if memory overflow
static int _insert( LIST *pList, NODE *pPre, void *dataInPtr) {
	
	NODE *pNew = (NODE *)malloc(sizeof(NODE));
	
	if (pNew == NULL)
		return 0;
	
	pNew->dataPtr = dataInPtr;
	
	if (pPre == NULL) {
		if (pList->rear == NULL) //empty
			pList->rear = pNew;
		else //insert before first node
			pList->head->llink = pNew;
		
		pNew->llink = NULL;
		pNew->rlink = pList->head;
		pList->head = pNew;
		
	}
	else {
		if (pPre->rlink == NULL) //맨끝에 삽입
			pList->rear = pNew;
		else
			pPre->rlink->llink = pNew; //중간에 삽입
		
		pNew->rlink = pPre->rlink;
		pPre->rlink = pNew;
		pNew->llink = pPre;
	}
	
	pList->count++;
	return 1;
} //end of _insert

// internal delete function
// deletes data from list and saves the (deleted) data to dataOutPtr
// for removeNode function
static void _delete( LIST *pList, NODE *pPre, NODE *pLoc, void **dataOutPtr) {
	
	*dataOutPtr = pLoc->dataPtr;

	if (pPre == NULL) { 
		if (pLoc->rlink == NULL) { //첫 노드이자 마지막 노드 지우기
			pList->head = NULL;
			pList->rear = NULL;
		}	
		else { //첫 노드 지우기
			pList->head = pLoc->rlink;
			pLoc->rlink->llink = NULL;
		}
	}
	else if (pLoc->rlink == NULL) { //마지막 노드 지우기
		pList->rear = pPre;
		pPre->rlink = NULL;
	}
	else { //중간 노드 지우기
		pPre->rlink = pLoc->rlink;
		pLoc->rlink->llink = pPre;
	}
	
	free(pLoc);
	pList->count--;
} //end of _delete

// internal search function
// searches list and passes back address of node containing target and its logical predecessor
// for addNode, removeNode, searchNode functions
// return	1 found
// 			0 not found
static int _search( LIST *pList, NODE **pPre, NODE **pLoc, void *pArgu) {
	
	*pPre = NULL;
	*pLoc = pList->head;
	
	while ((*pLoc) != NULL && pList->compare((*pLoc)->dataPtr, pArgu) < 0) { 
		*pPre = *pLoc;
		*pLoc = (*pLoc)->rlink;
	}
	
	if (*pLoc == NULL)
		return 0;
	else {
		if (pList->compare((*pLoc)->dataPtr, pArgu) == 0)
			return 1;
		else
			return 0;
	}
} //end of _search