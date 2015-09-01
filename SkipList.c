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
	//int totalSize = sizeof(Node) + sizeof(NodePtr) * level;
	nodeptr->next = (NodePtr *)malloc(sizeof(NodePtr) * (level+1));
	for(i = 0; i <= level; i++){
		nodeptr->next[i] = &NIL;	
		}
	return nodeptr;
}

SkiplistPtr initSkiplist(){
	SkiplistPtr skiplistptr; //structure declaration ? space allocation
	skiplistptr = (SkiplistPtr)malloc(sizeof(NodePtr) + sizeof(int));
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

int randomIntGenerate(int range){
	srand((int)time(0));
	int randValue;
	randValue = rand()% range; // 0-MaxNum
	return randValue;
	//reference to leveldb
}	
void insertKey(SkiplistPtr skiplistPtr, int key){
	NodePtr nodePtr = searchKey(skiplistPtr, key);
	
		if(nodePtr != skiplistPtr->head && nodePtr->element == key) {
			return; //skiplist contains the key
		}else{
			int level = randomIntGenerate(MaxLevel);
			NodePtr targetPtr = initNodeWithLevel(level, key);
			targetPtr->next[0] = nodePtr->next[0];
			nodePtr->next[0] = targetPtr; //no-Targeted manifests level0 examined
			NodePtr p;
			int i;
			for( i = level; i > 0; i--){
				p = skiplistPtr->head;
				for(nodePtr = p->next[level]; nodePtr != &NIL && nodePtr->element < key;){
					p = nodePtr;
					nodePtr = nodePtr->next[level];
				}
				targetPtr->next[level] = p->next[level];
				p->next[level] = targetPtr;
			}
			if(skiplistPtr->currentmaxLevel < level)
				skiplistPtr->currentmaxLevel = level;
		}
}

void main(){
	SkiplistPtr skiplistPtr = initSkiplist();
	insertKey(skiplistPtr, 100);
	/*
	NodePtr p = skiplistPtr->head;
	while(p != &NIL){
		printf("%-10000d %d", p->element, p->level);
		p = p->next[0];
		}
		*/
	/*
	int i;
	¡ffor(i = 0; i < 10000; i++){
		int key = randomIntGenerate(10000);
		insertKey(skiplistPtr, key);
	}
	*/
	/*
	insertKey(skiplistPtr, 50);
	insertKey(skiplistPtr, 110);
	int flag = 1;
	NodePtr nodeptr, q; 
	nodeptr = skiplistPtr->head->next[0];
	while(nodeptr != &NIL ){
		q = nodeptr->next[0];
		if(q != &NIL){
			if(nodeptr->element > q->element){
				flag = 0;
				break;
			}
		}
		nodeptr = q;
	}
	printf("%s", (flag == 1)? "ture":"false");	
	*/
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
				q = q->next[i]
			}
			p->next[i] = q->next[i];
		}
		free(nodePtr);
		while(skiplistPtr->head[skiplistPtr->currentmaxLevel] == NIL){
			currentmaxLevel--;
			}
		}
}


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



//unit test
//randomly generate 10^4 
/*
void main(){
	SkiplistPtr skiplistPtr = initSkiplist() ;
	inserKey(skiplistPtr, 100);
	/*
	¡ffor(int i = 0; i < 10000; i++){
		int key = randomIntGenerate(10000);
		insertKey(skiplistPtr, key);
	}
	bool flag = true;
	NodePtr nodeptr, q; 
	nodeptr = skiplistPtr->head->next[0];
	while(nodeptr != NIL ){
		q = nodeptr->next[0];
		if(q != NIL){
			if(nodeptr->element > q->element){
				flag = false;
				break;
			}
		}
		nodeptr = p;
	}
	printf("%s", (flag == true)? "ture":"false");	
}
*/



