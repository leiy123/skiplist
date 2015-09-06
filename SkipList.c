//#include "SkipList.h"
#include<stdlib.h>
#include<time.h>
#include<stdio.h>

#define MaxLevel 10 //macro definition


typedef struct Node{
	int element; //key
	int level;
	//NodePtr next[1]; //ptr[level],change to NodePtr *next
	struct Node** next;
}*NodePtr;




typedef struct Skiplist{
	NodePtr head; // capitalize the first charator of name structure
	int currentmaxLevel;
} *SkiplistPtr;

static struct Node NIL = {0x7fffffff, 0, NULL}; //tail node const

//level starts from 0

NodePtr initNodeWithLevel(int level, int key){
	int i;
	NodePtr nodeptr = (NodePtr)malloc(sizeof(struct Node));
	nodeptr->element = key;
	nodeptr->level = level;
	//int totalSize = sizeof(Node) + sizeof(NodePtr) * level;
	nodeptr->next = (NodePtr *)malloc(sizeof(NodePtr) * (level+1));
	for(i = 0; i <= level; i++){
		nodeptr->next[i] = &NIL;	
		}
	return nodeptr;
}

SkiplistPtr initSkiplist(){
	srand(time(0));
	SkiplistPtr skiplistptr; //structure declaration ? space allocation
	skiplistptr = (SkiplistPtr)malloc(sizeof(struct Skiplist)); //sizeof(NodePtr) + sizeof(int)
	skiplistptr->head = (NodePtr)initNodeWithLevel(MaxLevel, 0x7fffffff);
	skiplistptr->currentmaxLevel = 0;
}

//return node targeted or key position to be inserted
NodePtr searchKey(SkiplistPtr skiplistPtr, int key){
	if(skiplistPtr->head->next[0] == &NIL) return skiplistPtr->head;
	NodePtr p, q;
	p = skiplistPtr->head; //p: precurser , q : successor
	//find the first node no less than target 
	int i = p->level;
	while(i >= 0){
	for(q = p->next[i]; q != &NIL && q->element < key ; ){
		p = q;
		q = q->next[i];
		} //exit with linkedlist in the same level
	if((q != &NIL) && (q->element == key)) return q;
		i--; //move towards bottom 
	}
	return p; 
}
int genLevel(int range){
	int i, height;
	volatile int temp;
	height = 0;
	for(i = 0; i < range; i++)
		{
		temp = rand();
		if(temp % 2) height++;     //odd ,up to range probability(1/2)
			else return height;
		}
}
	//reference to leveldb	
void insertKey(SkiplistPtr skiplistPtr, int key){
	NodePtr nodePtr = searchKey(skiplistPtr, key);
	
		if(nodePtr != skiplistPtr->head && nodePtr->element == key) {
			return; //skiplist contains the key
		}else{
			int level = genLevel(MaxLevel);
			NodePtr targetPtr = initNodeWithLevel(level, key);
			targetPtr->next[0] = nodePtr->next[0];
			nodePtr->next[0] = targetPtr; //no-Targeted manifests level0 examined
			NodePtr p;
			int i;
			for( i = level; i > 0; i--){
				p = skiplistPtr->head;
				for(nodePtr = p->next[i]; nodePtr != &NIL && nodePtr->element < key;){
					p = nodePtr;
					nodePtr = nodePtr->next[i];
				}
				targetPtr->next[i] = p->next[i];
				p->next[i] = targetPtr;
			}
			if(skiplistPtr->currentmaxLevel < level)
				skiplistPtr->currentmaxLevel = level;
		}
}

void deleteKey(SkiplistPtr skiplistPtr, int key){
	NodePtr nodePtr = searchKey(skiplistPtr, key);
	if( nodePtr == skiplistPtr ->head || nodePtr->element != key ) return;
	else{
		NodePtr p, q;	
		int i;
		for(i = nodePtr->level; i >= 0; i--){
			p = skiplistPtr->head;
			for(q = p->next[i]; q != nodePtr;  ){
				p = q;
				q = q->next[i];
			}
			p->next[i] = q->next[i];
		}
		free(nodePtr);
		while(skiplistPtr->currentmaxLevel != 0 && skiplistPtr->head->next[skiplistPtr->currentmaxLevel] == &NIL){
			skiplistPtr->currentmaxLevel--;
			}
		}
}

void releaseSkiplist(SkiplistPtr skipListPtr){
	NodePtr p, q;
	p = skipListPtr->head->next[0];
	while(p != &NIL){
		q = p->next[0];
		free(p);
	}
	free(&NIL);
	free(skipListPtr);//whether to release the skipList structure
}



void main(){
	SkiplistPtr skiplistPtr = initSkiplist();
	/*
	insertKey(skiplistPtr, 50);
	insertKey(skiplistPtr, 100);
	insertKey(skiplistPtr, 110);
	
	deleteKey(skiplistPtr, 50);
	deleteKey(skiplistPtr, 110);
	deleteKey(skiplistPtr, 100);
	printf("%d\n", skiplistPtr->currentmaxLevel);
	NodePtr p = skiplistPtr->head;
	while(p != &NIL){
		printf("%-4d %d\n", p->element, p->level);
		p = p->next[0];
		}
		*/
		
	
	int i;
	for(i = 0; i < 10000; i++){
		int key = rand();
		insertKey(skiplistPtr, key);
	}

	
	
	int flag = 1;
	NodePtr nodeptr, q;
	int level = skiplistPtr->currentmaxLevel;
	for(; level >= 0; level--){
		nodeptr = skiplistPtr->head->next[level];
		while(nodeptr != &NIL ){
			q = nodeptr->next[level];
			if(q != &NIL){
				if(nodeptr->element > q->element){
					flag = 0;
					break;
				}
			}
			nodeptr = q;
			}
		}
	printf("current level is %s\n",skiplistPtr->currentmaxLevel);
	printf("%s\n", (flag == 1)? "ture":"false");	
}




/*
void releaseSkiplist(SkipListPtr skipListPtr){
	NodePtr p, q;
	p = skipListPtr->head->next[0];
	while(p != &NIL){
		q = p->next[0];
		free(p);
	}
	free(&NIL);
	free(skipListPtr);//whether to release the skipList structure
	}

*/





