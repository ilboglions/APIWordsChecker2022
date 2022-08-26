#include <stdio.h>
#include <stdlib.h>
typedef struct elem_s{
	int info;
	struct elem_s * next;
} elem_t;
elem_t * listmergesort(elem_t * head){
	int len;
	int i;
	int lim;
	elem_t * second;
	elem_t * tmp;

	len = listlenght(head);
	if(len > 1){
		lim = (len/2)-1;
		tmp = head;
		for(i=0; i < lim; i++){
			tmp = tmp->next;
		}
		second = tmp->next;
		tmp ->next = NULL;
		head=listmergesort(head);
		second=listmergesort(second);

		head = merge (head, second);
	}
	return head;
}

elem_t * append(elem_t * head, int val){
	elem_t * tmp, * scambio;
	if(tmp = malloc(sizeof(elem_t))){
		tmp->info = val;
		tmp->next = NULL;
		if(head){
			scambio = head;
			while(scambio->next != NULL)
				scambio = scambio->next;
			scambio->next = tmp;
		}
		else
			head = tmp;
	}
	else
		printf("Errore con allocazione memoria (%d) \n", val);
	return head;
}
elem_t * emptylist(elem_t * head){
	if(head){
		if(head->next == NULL){
			free(head);
			head=NULL;
			return head;
		}
		head=emptylist(head->next);
	}
	return head;
}
void printlist(elem_t * head){
	while(head != NULL){
		printf("%d -> ", head->info);
		head = head->next;
	}
	printf("NULL\n");
}
int listlenght(elem_t * head){
	int count=0;
	elem_t * tmp;
	tmp = head;
	while(tmp){
		count++;
		tmp = tmp->next;
	}
	return count;
}
elem_t * merge(elem_t * first, elem_t * second){
	elem_t * newhead;
	elem_t * last;
	if(first->info < second->info){
		newhead= first;
		first = first->next;
	}
	else{
		newhead= second;
		second= second->next;
	}
	last=newhead;
	while(first != NULL && second != NULL){
		if(first->info < second->info){
			last->next = first;
			first = first->next;
		}
		else{
			last->next = second;
			second = second->next;
		}
		last = last->next;
	}
	if(first != NULL){
		last->next = first;
	}
	else
		last->next = second;


	return newhead;
}

