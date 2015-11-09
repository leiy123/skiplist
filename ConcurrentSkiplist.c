#include "ConcurrentSkiplist.h"
#include <errno.h>


//extern NodePtr preds[i], succs[i]; //thread private

static int randomLevel(int range){
	int i, height, temp;
	height = 0;
	for(i = 0; i < range; i++)
		{
		temp = rand();
		if(temp % 2) height++;     //odd ,up to range
			else return height;
		}  
}

void initializeNode(NodePtr nodePtr, int key, int level){
	//nodePtr = (NodePtr)malloc(sizeof(struct Node));  //clear 0x0 when return to calling function
	nodePtr->key = key;
	nodePtr->topLayer = level;
	nodePtr->nexts = (NodePtr* )malloc((level+1) * sizeof(NodePtr));
	if(nodePtr->nexts == NULL)
		exit(-2);
	nodePtr->marked = 0x00;
	nodePtr->fullyLinked = 0x00;
	//nodePtr->wrlock = PTHREAD_RWLOCK_INITIALIZER;
	pthread_rwlock_init(&nodePtr->wrlock, NULL);
	} 
	
void initializeSkiplist(SkiplistPtr skiplistPtr){
	srand(time(0));
	//skiplistPtr =(SkiplistPtr)malloc(sizeof(struct Skiplist)); //clear 0x0 when return to calling function
	skiplistPtr->head = (NodePtr)malloc(sizeof(struct Node));
	skiplistPtr->NIL = (NodePtr)malloc(sizeof(struct Node));
	initializeNode(skiplistPtr->head, -1, MaxLevel);
	initializeNode(skiplistPtr->NIL, 0x7fffffff, MaxLevel);
	int i;
	for(i = 0; i < MaxLevel; i++){
		skiplistPtr->head->nexts[i] = skiplistPtr->NIL;  //initialization: head->NIL
	}
	skiplistPtr->currentLevel = -1; 
}


//
 void unlock(NodePtr preds[], int highestLevel){
	int i = highestLevel;
	if(highestLevel == -1) return;
	//pthread_rwlock_unlock(&preds[0]->wrlock);
	if(pthread_rwlock_unlock(&preds[i]->wrlock) != 0)
		printf("%d/n", errno);
	 while(--i >= 0){
		if(preds[i+1] != preds[i])
			if(pthread_rwlock_unlock(&preds[i]->wrlock) != 0)
				printf("%d/n", errno);
		} 	
}


static void nodeFree(NodePtr nodePtr){
	pthread_rwlock_destroy(&nodePtr->wrlock);
	free(nodePtr->nexts);
	free(nodePtr);
}

/*
  *if not-founded, return -1
  *else return the node's toplayer
  *fill in preds: predecessors, succs: successors
  *helper function(reentrant ???)
  */
int findNode(const SkiplistPtr skiplistPtr, int v, NodePtr preds[], NodePtr succs[]){
	int lFound, i, j ;
	lFound = -1;
	NodePtr cur, pred;
	pred = skiplistPtr->head;
	for(i = skiplistPtr->currentLevel; i >= 0; i--){
		for(cur = pred->nexts[i];cur != skiplistPtr->NIL && cur->key < v; cur = pred->nexts[i]){
			pred = cur;
			}
		preds[i] = pred;
		if(cur != skiplistPtr->NIL && cur->key == v) {
			if(lFound == -1){
				lFound = i;   //i = cur->toplayer;
			for(j = 0; j <= lFound; j++){
				succs[j] = cur;
				}
			}
			else
				continue;
				}
		else{
			succs[i] = cur;
			}
	}
	return lFound;
}

/*
  *if v existing in list, return 0
  *else inserting v, return 1
  */
int  addKey(SkiplistPtr skiplistPtr, int v, NodePtr preds[], NodePtr succs[]){
	int topLayer = randomLevel(MaxLevel);
	//NodePtr preds[MaxLevel], succs[MaxLevel]; //threads local
	int flag = 0; //whether to retry
	while(1){
		int lFound = findNode(skiplistPtr, v, preds, succs);
		if(lFound != -1){
			NodePtr nodeFound = succs[lFound];
			if(!nodeFound->marked){
				while(!nodeFound->fullyLinked) {;}
				return 0;
				}
			continue; //retry : other threads deleting the node
			}
		int highestLocked = -1;
		//inserting new node with key v
		NodePtr pred, succ, prepred = NULL; //avoid locking the same node twice;
		int i, valid ;
		valid = 1;
		for(i = 0; i <= topLayer && valid; i++){
			pred = preds[i];
			succ = succs[i];
			if(pred != prepred){ 
				if(pthread_rwlock_trywrlock(&pred->wrlock) != 0){
					unlock(preds, highestLocked);
					flag =1;
					break;
					}
				highestLocked = i;
				prepred = pred;
				}
			valid = !pred->marked && !succ->marked && pred->nexts[i] == succ;
			}
		if(!valid){
			unlock(preds, highestLocked);
			flag = 0;  //retry ,clear flag state
			continue;
			}
		if(flag) {
			flag = 0;  
			continue; 
		}//retry : some node already locked in other threads
		
		//succesfully locking all the preds, linked the node with key v
		NodePtr nodePtrNew = (NodePtr)malloc(sizeof(struct Node));
		if(nodePtrNew== NULL){
			exit(-1);
			}
		initializeNode(nodePtrNew, v, topLayer);

		//update only when height upper the current(head locking) 
		if(topLayer > skiplistPtr->currentLevel)
			skiplistPtr->currentLevel = topLayer; 
		for(i = 0; i <= topLayer; i++){
			nodePtrNew->nexts[i] = succs[i]; //preds[i]->nexts[i];
			preds[i]->nexts[i] = nodePtrNew;
			}
		nodePtrNew->fullyLinked = 0x01;
		unlock(preds, highestLocked);
		return 1;
		}
}

int removeKey(SkiplistPtr skiplistPtr, int v, NodePtr preds[], NodePtr succs[]){
	NodePtr nodeToDelete = NULL;
	char isMarked = 0;
	int topLayer = -1;
	//NodePtr preds[MaxLevel], succs[MaxLevel];
	while(1){
		int lFound = findNode(skiplistPtr, v, preds, succs);
		if(isMarked || (lFound != -1 && okToDelete(succs[lFound], lFound))){
			if(!isMarked){
				nodeToDelete = succs[lFound];
				topLayer = nodeToDelete->topLayer;
				if(pthread_rwlock_trywrlock(&nodeToDelete->wrlock) != 0){
					continue; //lock failure
					}
				if(nodeToDelete->marked){
					pthread_rwlock_unlock(&nodeToDelete->wrlock);
					return 0;
					}
				nodeToDelete->marked = 1; //logically deleting v
				isMarked = 1; //when fail locking preds, retry
				}
		int highestLocked, i; 
		highestLocked = -1;
		NodePtr pred, succ, prevpred = NULL;
		char valid, flag;
		valid = 1;
		flag = 0;

		//locking preds, retry when failing
		for(i = 0; valid && i < topLayer; i++){
			pred = preds[i];
			succ = succs[i];
			if(pred != prevpred){
				if(pthread_rwlock_trywrlock(&pred->wrlock) != 0){
					flag = 1;
					break;
					}
				highestLocked = i;
				prevpred = pred;
				}
			valid = !pred->marked && pred->nexts[i] == succ;
			}
		if(flag || !valid) {
			unlock(preds, highestLocked);
			continue;	
		}

		//modify links
		for(i = topLayer; i >= 0; i--){
			preds[i]->nexts[i] = nodeToDelete->nexts[i];
			if((preds[i] == skiplistPtr->head) && 
					(preds[i]->nexts[i] == skiplistPtr->NIL))
					skiplistPtr->currentLevel--;
			}
		pthread_rwlock_unlock(&nodeToDelete->wrlock);
		nodeFree(nodeToDelete);
		unlock(preds, highestLocked);
		return 1;
		}
		else return 0; //no existing the node with key v
		}
}
	
int okToDelete(NodePtr candidate, int lFound){
		return (candidate->fullyLinked && candidate->topLayer == lFound 
			&& !candidate->marked);
}
int contains(const SkiplistPtr skiplistPtr, int v, NodePtr preds[], NodePtr succs[]){
	//NodePtr preds[], succs[]; //threads local
	int lFound = findNode(skiplistPtr, v, preds, succs);
	return (lFound != -1 && succs[lFound]->fullyLinked && !succs[lFound]->marked);
}

