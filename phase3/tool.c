// tool.c, 159

#include "spede.h"
#include "type.h"
#include "extern.h"

int EmptyQ(q_t *p){
	return (p->size == 0);// try <=0
}

int FullQ(q_t *p){
	
	return (p->size==MAX_PROC);
}

void initq(q_t *q){
	q->size = 0;
	q->head = 0;
	q->tail = 0;
}


void MyBzero(char *p, int size) {//trying changing char to Q_t
/*
  int i;
	q->size = 0;
	q->head = 0;
	q->tail = 0;
  for(i = 0; i < MAX_PROC; i++) {
    p[i] = '\0'; 
  } 
  while(size--) *p++ = (char)0;   
*/

int i;
for(i=0; i<size; i++){
	*p++ = '\0';
}




} 

/*
 * void MyBzero (char *p, int size) {
 *  while(size--) *p++ = (char) 0;
 *
 * }
 */

void MyBZero(void *s, int n) {
int i;
for (i = 0; i < n; i++) {
	*((char*)s+n) = '\0';
	}
}

void EnQ(int pid, q_t *p) {

	if(FullQ(p) ){
		cons_printf("Que is full!\n");	
		return;
	}

	p->q[p->tail] = pid;
	p->tail ++;
	
	if(p->tail >= Q_SIZE) p->tail=0;

	p->size ++;
}

int DeQ(q_t *p) { // return -1 if q is empty
	int pid;
	if(p->size ==0){
    printf("Queue is empty\n");
    return -1;////////try Empty(&p)
   }

	pid = p->q[p->head];
	p->head++;

	if(p->head == Q_SIZE) p->head=0;
	p->size--;
	return pid;	
}

