#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define MaxLevel 10

typedef struct Node{
	int key;
	int topLayer;
	struct Node** nexts;
	char marked;  //other threads deleting the node
	char fullyLinked; //linked at all layers
	pthread_rwlock_t wrlock; //pthread_mutex_t the same?
}*NodePtr;

typedef struct Skiplist{
	struct Node* head;
	struct Node* NIL;
	int currentLevel;
}*SkiplistPtr;

static int randomLevel(int range);
static void unlock(NodePtr preds[], int highestLevel);
void initializeSkiplist(SkiplistPtr skiplistptr);
static void initializeNode(NodePtr nodePtr, int key, int level);
int findNode(const SkiplistPtr skiplistPtr, int v, NodePtr preds[], NodePtr succs[]);
int addKey(SkiplistPtr skiplistPtr, int v);
int removeKey(SkiplistPtr skiplistPtr, int v);
int okToDelete(NodePtr candidate, int lFound);
int contains(const SkiplistPtr skiplistPtr, int v);



