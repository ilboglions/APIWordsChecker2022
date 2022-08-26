#include <stdio.h>
#include <stdlib.h>
typedef struct elem_s{
	int info;
	struct elem_s * next;
} elem_t;
elem_t * find (elem_t * head, int ricerca){
	elem_t * tmp,*res;
	tmp = head;
	res = NULL;
	while(tmp != NULL && !res){
		if(tmp->info == ricerca)
			res=tmp;
		tmp = tmp->next;
	}
	return res;
}
elem_t * push (elem_t * head, int val){
	elem_t * tmp;
	if(tmp = malloc(sizeof(elem_t))){
		tmp->info = val;
		tmp->next = head;
		head = tmp;
	}
	else
		printf("errore con allocazione memoria (%d) \n", val);
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

elem_t * delete(elem_t * head, int val){
	elem_t * tmp, * ptr;
	while(head != NULL && head->info == val){
		tmp = head;
		head = head->next;
		free(tmp);
	}
	ptr=head;
	while(ptr && ptr->next){
		while(ptr->next && ptr->next->info == val){
			tmp= ptr->next;
			ptr->next = tmp->next;
			free(tmp);
		}
		ptr= ptr->next;	
	}
	return head;
}
elem_t * deletepos(elem_t * head, int pos){
	elem_t * tmp,* del;
	tmp = head;
	if(tmp){
		if(pos){
			while(tmp->next && (pos>1)){
				tmp = tmp->next;
				pos--;
			}
			del=tmp->next;
			tmp->next= tmp->next->next;
			free(del);
		}
		else{
			head=tmp->next;
			free(tmp);
		}
	}
	return head;
}
int listlenght(elem_t * head){
	int count;
	elem_t * tmp;
	tmp = head;
	while(tmp){
		count++;
		tmp = tmp->next;
	}
	return count;
}
elem_t * emptylist(elem_t * head){
	if(head){
		if(head->next != NULL){
			head->next = emptylist(head->next);
		}
		free(head);
	}
	return NULL;
}
