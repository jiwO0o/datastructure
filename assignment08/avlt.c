#define BALANCING

#include <stdlib.h> // malloc
#include <stdio.h>

#include "avlt.h"

#define max(x, y)	(((x) > (y)) ? (x) : (y)) //둘 중에 큰 값 return. 내 노드의 height = 내 자식들 중 height가 더 큰 노드의 height + 1

// internal functions (not mandatory)
// used in AVLT_Insert 삽입될 새 노드의 height 1로 설정
static NODE *_makeNode( void *dataInPtr) {
	NODE *new = (NODE *)malloc(sizeof(NODE));
	
	if (new == NULL)
		return NULL;
	
	new->dataPtr = dataInPtr;
	new->left = NULL;
	new->right = NULL;
	new->height = 1;
	
	return new;
} //end of _makeNode

// used in AVLT_Destroy
static void _destroy( NODE *root, void (*callback)(void *)) {
	if (root != NULL) {
		_destroy(root->left, callback);
		_destroy(root->right, callback);
		callback(root->dataPtr);
		free(root);
	}
} //end of _destroy

// used in AVLT_Delete
// return 	pointer to root
static NODE *_delete( NODE *root, void *keyPtr, void **dataOutPtr, int (*compare)(const void *, const void *)) {
	
	if (root == NULL)
		return NULL;
	
	if (compare(keyPtr, root->dataPtr) < 0)
		root->left = _delete(root->left, keyPtr, dataOutPtr, compare);
	
	else if (compare(keyPtr, root->dataPtr) > 0)
		root->right = _delete(root->right, keyPtr, dataOutPtr, compare);
	
	else { //삭제할 노드 찾은 경우
		NODE *dlt = root;
		*dataOutPtr = root->dataPtr;
		
		if (root->left == NULL) {
			if (root->right == NULL) //양쪽 다 자식이 없는 경우
				root = NULL;
			else //오른쪽에만 자식이 있는 경우
				root = root->right;
		}
		
		else if (root->right == NULL) //왼쪽에만 자식이 있는 경우
			root = root->left;
		
		else { //양쪽에 자식이 있는 경우 : 오른쪽 서브트리의 가장 작은 값을 가진 노드가 삭제될 노드의 자리로 위치하도록 해야 함
			NODE *smallest = root->right;
			while (smallest->left != NULL)
				smallest = smallest->left;
			root->dataPtr = smallest->dataPtr;
			
			if (smallest->right != NULL) {
				NODE *temp = smallest;
				smallest = smallest->right;
				free(temp);
			}
			
			else
				free(smallest);
		} //end of 양쪽에 자식이 있는 경우
		
		free(dlt);
	} //end of 삭제할 노드 찾은 경우
	
	return root;
} //end of _delete

// used in AVLT_Search
// Retrieve node containing the requested key
// return	address of the node containing the key
//			NULL not found
static NODE *_search( NODE *root, void *keyPtr, int (*compare)(const void *, const void *)) {
	
	if (root == NULL)
		return NULL;
	
	if (compare(keyPtr, root->dataPtr) == 0)
		return root;
	
	else if (compare(keyPtr, root->dataPtr) < 0)
		_search(root->left, keyPtr, compare);
		
	else
		_search(root->right, keyPtr, compare);
} //end of _search

// used in AVLT_Traverse
static void _traverse( NODE *root, void (*callback)(const void *)) {
	if (root != NULL) {
		_traverse(root->left, callback);
		callback(root->dataPtr);
		_traverse(root->right, callback);
	}
} //end of _traverse

// used in AVLT_TraverseR
static void _traverseR( NODE *root, void (*callback)(const void *)) {
	if (root != NULL) {
		_traverseR(root->right, callback);
		callback(root->dataPtr);
		_traverseR(root->left, callback);
	}
} //end of _traverseR

// used in printTree
static void _inorder_print( NODE *root, int level, void (*callback)(const void *)) {
	if (root != NULL) {
		level++;
		_inorder_print(root->right, level, callback);
		for (int i = 0; i < level; i++)
			printf("\t");
		callback(root->dataPtr);
		_inorder_print(root->left, level, callback);
	}
} //end of _inorder_print

// internal function
// return	height of the (sub)tree from the node (root)
static int getHeight( NODE *root) {
	
	int leftheight, rightheight;
	
	if (root == NULL)
		return 0;
	
	if (root->left == NULL)
		leftheight = 0;
	else
		leftheight = root->left->height;
	
	if (root->right == NULL)
		rightheight = 0;
	else
		rightheight = root->right->height;
	
	return max(leftheight, rightheight) + 1;
} //end of getHeight

// internal function
// Exchanges pointers to rotate the tree to the right
// updates heights of the nodes (rotate되는 당사자 node나 그 당사자의 자식 node의 height 바뀔 수 있음)
// return	new root
static NODE *rotateRight( NODE *root) {
	
	NODE *temp = root;
	root = root->left;
	temp->left = root->right;
	root->right = temp;
	
	if (root->left != NULL)
		root->left->height = getHeight(root->left);
	root->right->height = getHeight(root->right);
	root->height = getHeight(root);
	
	return root;
} //end of rotateRight

// internal function
// Exchanges pointers to rotate the tree to the left
// updates heights of the nodes
// return	new root
static NODE *rotateLeft( NODE *root) {
	
	NODE *temp = root;
	root = root->right;
	temp->right = root->left;
	root->left = temp;
	
	if (root->right != NULL)
		root->right->height = getHeight(root->right);
	root->left->height = getHeight(root->left);
	root->height = getHeight(root);
	
	return root;
} //end of rotateLeft

// used in AVLT_Insert
// recursion 사용
// 삽입될 때 삽입된 node의 경로상 부모들은 다 height가 1만큼 증가
// return 	pointer to root
static NODE *_insert( NODE *root, NODE *newPtr, int (*compare)(const void *, const void *)) {
	
	if (compare(newPtr->dataPtr, root->dataPtr) < 0) {
		if (root->left == NULL)
			root->left = newPtr;
		else
			root->left = _insert(root->left, newPtr, compare);
	}
		
	else {
		if (root->right == NULL)
			root->right = newPtr;
		else
			root->right = _insert(root->right, newPtr, compare);
	}
	
	root->height = getHeight(root);
	int taller = getHeight(root->left) - getHeight(root->right);
	
	if (taller > 1) {
		if (getHeight(root->left->left) > getHeight(root->left->right)) //LL
			root = rotateRight(root);
		else { //LR
			root->left = rotateLeft(root->left);
			root = rotateRight(root);
		}
	}
	
	else if (taller < -1) {
		if (getHeight(root->right->right) > getHeight(root->right->left)) //RR
			root = rotateLeft(root);
		else { //RL
			root->right = rotateRight(root->right);
			root = rotateLeft(root);
		}
	}
	
	return root;
} //end of _insert




/* Allocates dynamic memory for a tree head node and returns its address to caller
	return	head node pointer
			NULL if overflow
*/
TREE *AVLT_Create( int (*compare)(const void *, const void *)) {
	
	TREE *headtree = (TREE *)malloc(sizeof(TREE));
	
	if (headtree == NULL)
		return NULL;
	
	headtree->count = 0;
	headtree->root = NULL;
	headtree->compare = compare;
	
	return headtree;
} //end of AVLT_Create

/* Deletes all data in tree and recycles memory
*/
void AVLT_Destroy( TREE *pTree, void (*callback)(void *)) {
	
	if (pTree != NULL)
		_destroy(pTree->root, callback);

	pTree->count = 0;
	pTree->compare = NULL;
	free(pTree);
	
} //end of AVLT_Destroy

/* Inserts new data into the tree
	return	1 success
			0 overflow
*/
int AVLT_Insert( TREE *pTree, void *dataInPtr) {
	
	NODE *newPtr = _makeNode(dataInPtr);
		
	if (newPtr == NULL)
		return 0;
	
	else {
		if (pTree->root == NULL)
			pTree->root = newPtr;
		else
			pTree->root = _insert(pTree->root, newPtr, pTree->compare);
		
		pTree->count++;
		return 1;
	}
} //end of AVLT_Insert

/* Deletes a node with keyPtr from the tree
	return	address of data of the node containing the key
			NULL not found
*/
void *AVLT_Delete( TREE *pTree, void *keyPtr) {
	
	void *dataOutPtr;
	NODE *rootPtr = _delete(pTree->root, keyPtr, &dataOutPtr, pTree->compare);
	
	if (rootPtr == NULL)
		return NULL;
	else {
		pTree->root = rootPtr;
		pTree->count--;
		return dataOutPtr;
	}
} //end of AVLT_Delete

/* Retrieve tree for the node containing the requested key (keyPtr)
	return	address of data of the node containing the key
			NULL not found
*/
void *AVLT_Search( TREE *pTree, void *keyPtr) {
	
	NODE *find = _search(pTree->root, keyPtr, pTree->compare);
	
	if (find == NULL)
		return NULL;
	else
		return find->dataPtr;
} //end of AVLT_Search

/* prints tree using inorder traversal
*/
void AVLT_Traverse( TREE *pTree, void (*callback)(const void *)) {
	_traverse(pTree->root, callback);
} //end of AVLT_Traverse

/* prints tree using right-to-left inorder traversal
*/
void AVLT_TraverseR( TREE *pTree, void (*callback)(const void *)) {
	_traverseR(pTree->root, callback);
} //end of AVLT_TraverseR

/* Print tree using right-to-left inorder traversal with level
*/
void printTree( TREE *pTree, void (*callback)(const void *)) {
	int level = 0;
	_inorder_print(pTree->root, level, callback);
} //end of printTree

/* returns number of nodes in tree
*/
int AVLT_Count( TREE *pTree) {
	return pTree->count;
} //end of AVLT_Count

/* returns height of the tree
*/
int AVLT_Height( TREE *pTree) {
	return pTree->root->height;
} //end of AVLT_Height