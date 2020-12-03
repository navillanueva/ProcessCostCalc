


#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "queue.h"



//To create a queue
queue* queue_init(int size){

	queue * q = (queue *)malloc(sizeof(queue));
    	q->max_size=size;
    	q->header=0;
    	q->nextPosition=0;
    	q->num_elem=0;
    	q->p=(struct element *)malloc(size*(sizeof( struct element)));
   	return q;
}


// To Enqueue an element
int queue_put(queue *q, struct element* x) {
	memcpy(((q->p)+(q->nextPosition)), x, sizeof(struct element));
	q->nextPosition=((q->nextPosition)+1)%q->max_size;
	q->num_elem++;
	return 0;
}


// To Dequeue an element.
struct element* queue_get(queue *q) {
	struct element* element;
    	element=((q->p)+(q->header));
    	q->header=((q->header)+1)%q->max_size;
    	q->num_elem--;
    	return element;
}


//To check queue state
int queue_empty(queue *q){
	if(q->num_elem==0){
    		return 1;
    	}
    	
    	else{
    		return 0;
    	}

}

int queue_full(queue *q){
    	if(q->num_elem==q->max_size){
    		return 1;
    	}
    	
    	else{
    		return 0;
    	}
}

//To destroy the queue and free the resources
int queue_destroy(queue *q){
	free(q->p);
	free(q);
    	return 0;
}
