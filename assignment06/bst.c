#include <stdlib.h> // malloc
#include <stdio.h>

#include "bst.h"

// internal functions (not mandatory)
// used in BST_Insert
static void _insert( NODE *root, NODE *newPtr, int (*compare)(const void *, const void *)) {
	
	if (compare(newPtr->dataPtr, root->dataPtr) < 0) {
			if (root->left == NULL)
				root->left = newPtr;
			else
				_insert(root->left, newPtr, compare);
		}
		
		else {
			if (root->right == NULL)
				root->right = newPtr;
			else
				_insert(root->right, newPtr, compare);
		}
} //end of _insert

// used in BST_Insert
static NODE *_makeNode( void *dataInPtr) {
	NODE *new = (NODE *)malloc(sizeof(NODE));
	
	if (new == NULL)
		return NULL;
	
	new->dataPtr = dataInPtr;
	new->left = NULL;
	new->right = NULL;
	
	return new;
} //end of _makeNode

// used in BST_Destroy
static void _destroy( NODE *root, void (*callback)(void *)) {
	if (root != NULL) {
		_destroy(root->left, callback);
		_destroy(root->right, callback);
		callback(root->dataPtr);
		free(root);
	}
} //end of _destroy

// used in BST_Delete
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

// used in BST_Search
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

// used in BST_Traverse LNR
static void _traverse( NODE *root, void (*callback)(const void *)) {
	if (root != NULL) {
		_traverse(root->left, callback);
		callback(root->dataPtr);
		_traverse(root->right, callback);
	}
} //end of _traverse

// used in BST_TraverseR RNL
static void _traverseR( NODE *root, void (*callback)(const void *)) {
	if (root != NULL) {
		_traverse(root->right, callback);
		callback(root->dataPtr);
		_traverse(root->left, callback);
	}
} //end of _traverseR

// used in printTree RNL
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

/* Allocates dynamic memory for a tree head node and returns its address to caller
	return	head node pointer
			NULL if overflow
*/
TREE *BST_Create( int (*compare)(const void *, const void *)) {
	
	TREE *maketree = (TREE *)malloc(sizeof(TREE));
	
	if (maketree == NULL)
		return NULL;
	
	maketree->count = 0;
	maketree->root = NULL;
	maketree->compare = compare;
	
	return maketree;
} //end of BST_Create

/* Deletes all data in tree and recycles memory
*/
void BST_Destroy( TREE *pTree, void (*callback)(void *)) {
	
	if (pTree != NULL)
		_destroy(pTree->root, callback);

	pTree->count = 0;
	pTree->compare = NULL;
	free(pTree);
	
} //end of BST_Destroy

/* Inserts new data into the tree
	return	1 success
			0 overflow
*/
int BST_Insert( TREE *pTree, void *dataInPtr) {
	
	NODE *newPtr = _makeNode(dataInPtr);
		
	if (newPtr == NULL)
		return 0;
	else {
		if (pTree->root == NULL)
			pTree->root = newPtr;
		else
			_insert(pTree->root, newPtr, pTree->compare);
		
		pTree->count++;
		return 1;
	}
} //end of BST_Insert

/* Deletes a node with keyPtr from the tree
	return	address of data of the node containing the key
			NULL not found
*/
void *BST_Delete( TREE *pTree, void *keyPtr) {
	
	void *dataOutPtr;
	NODE *rootPtr = _delete(pTree->root, keyPtr, &dataOutPtr, pTree->compare);
	
	if (rootPtr == NULL)
		return NULL;
	else {
		pTree->root = rootPtr;
		pTree->count--;
		return dataOutPtr;
	}
} //end of BST_Delete

/* Retrieve tree for the node containing the requested key (keyPtr)
	return	address of data of the node containing the key
			NULL not found
*/
void *BST_Search( TREE *pTree, void *keyPtr) {
	
	NODE *find = _search(pTree->root, keyPtr, pTree->compare);
	
	if (find == NULL)
		return NULL;
	else
		return find->dataPtr;
} //end of BST_Search

/* prints tree using inorder traversal LNR
*/
void BST_Traverse( TREE *pTree, void (*callback)(const void *)) {
	_traverse(pTree->root, callback);
} //end of BST_Traverse

/* prints tree using right-to-left inorder traversal RNL
*/
void BST_TraverseR( TREE *pTree, void (*callback)(const void *)) {
	_traverse(pTree->root, callback);
} //end of BST_TraverseR

/* Print tree using right-to-left inorder traversal with level
*/
void printTree( TREE *pTree, void (*callback)(const void *)) {
	int level = 0;
	_inorder_print(pTree->root, level, callback);
} //end of printTree

/* returns number of nodes in tree
*/
int BST_Count( TREE *pTree) {
	return pTree->count;
} //end of BST_Count