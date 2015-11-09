#include "ConcurrentSkiplist.h"
#include <errno.h>

#define ThreadsN 6
#define ops 1000000
//#define N ThreadsN*MaxLevel

static SkiplistPtr skiplistPtr;
//NodePtr preds[N], succs[N];
NodePtr preds[ThreadsN][MaxLevel+1], succs[ThreadsN][MaxLevel+1];
/* NodePtr nodePtrNew; */

void* start_routine(void* args){
	int i, j, k;
	i = *(int* )args;
	j = ops / ThreadsN;
	//NodePtr preds[MaxLevel], succs[MaxLevel];
	for(k = 0; k < MaxLevel; k++){  //thread i private
		preds[i][k] =  skiplistPtr->head;
		succs[i][k] = skiplistPtr->NIL;
		}

	for(; j > 0; j--){
		int res = rand();
		switch(j % 10){
			case 0:
			case 1:
				addKey(skiplistPtr, res, preds[i], succs[i]);	  //20% add ops
				break;
			case 2:
				removeKey(skiplistPtr, res, preds[i], succs[i]); //10% remove ops
				break;
			default:	
				contains(skiplistPtr, res, preds[i], succs[i]); //70% contains ops
			}	
		}
}
void* test_lock(void* args){
	/* if(pthread_rwlock_trywrlock(&nodePtrNew->wrlock) != 0){
		printf("1\n");
		printf("lock failure\n");
		return;
	} */
	int i = (int)args;
	printf("i: %d\n", i);
	addKey(skiplistPtr, i, preds[i], succs[i]);
	return (void *)i;
	/*int i;
	for(i = 0; i < 3; i++){
		printf("%d\n", i);
	} */
	//unlock(&nodePtrNew, 0);
	/* if(pthread_rwlock_unlock(&nodePtrNew->wrlock) != 0)
			printf("%d\n", errno); */
}


void main(){
/* 	int ret, i;
	//srand(time(0));
	skiplistPtr =(SkiplistPtr)malloc(sizeof(struct Skiplist));
	initializeSkiplist(skiplistPtr);
	pthread_t threads[ThreadsN];
	for(i = 0; i < ThreadsN; i++){
		ret = pthread_create(&threads[i], NULL, start_routine, (void*)&i);
		if(ret != 0){
			printf("can't create thread\n");
			exit(-3);
			}	
		}
	for(i = 0; i < ThreadsN; i++){
		ret = pthread_join(threads[i], NULL);
		if(ret != 0){
			printf("can't create thread\n");
			exit(-1);
			}
		} */
	int i, k, j, ret, lfound, n;
	skiplistPtr =(SkiplistPtr)malloc(sizeof(struct Skiplist));
	initializeSkiplist(skiplistPtr);
	for(j = 0; j < ThreadsN; j++)
		for(k = 0; k <= MaxLevel; k++){  //thread i private
			preds[j][k] =  skiplistPtr->head;
			succs[j][k] = skiplistPtr->NIL;
			}
/* 	addKey(skiplistPtr, 1, preds[0], succs[0]);
	addKey(skiplistPtr, 2, preds[0], succs[0]); */
	/* nodePtrNew = (NodePtr)malloc(sizeof(struct Node));
	initializeNode(nodePtrNew, 3, 1); */
	pthread_t threads[ThreadsN];
	void *retcode;
	for(i = 0; i < ThreadsN; i++){
		ret = pthread_create(&threads[i], NULL, test_lock, (void *)i);
		if(ret != 0){
			printf("can't create thread\n");
			exit(-1);
		}
	}
	for(i = 0; i < ThreadsN; i++){
		ret = pthread_join(threads[i], &retcode);
		if(ret != 0){
			printf("can't join thread\n");
			exit(-1);
			}
		printf("thread[%d] exit\n", (int)retcode);
	}
	NodePtr nodePtrNew = skiplistPtr->head->nexts[0]; 
	for(n = 0; n < ThreadsN; n++){
		printf("node[%d]: key(%d), topLayer(%d)\n", n, nodePtrNew->key, nodePtrNew->topLayer);
		nodePtrNew = nodePtrNew->nexts[0];
	}
/* 	if(pthread_rwlock_trywrlock(&nodePtrNew->wrlock) != 0)
		printf("lock failure/n"); */
	
	
	/* for(i = 0; i < 3; i++){
		if(addKey(skiplistPtr, i, preds[0], succs[0]) == 1)
			printf("true\n");
		else
			printf("false\n");
	}	
*/	
	
	/* if((lfound = findNode(skiplistPtr, 1, preds[0], succs[0]))!= -1){
		printf("key in list.");
		for(i = lfound; i >= 0; i--)
			printf("%ld\n", preds[0][i]);
	} 
	else {
		printf("not in the list\n");
	} */
	
printf("done!\n");	
}




