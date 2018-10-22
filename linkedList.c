#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "linkedList.h"

/*
* Namn: list_create
* Syfte: Skapar en ny lista
* Parametrar: void
* Returvärde: pekare till den nya listan
*/

list* list_create(void){
	list* newList=malloc(sizeof(struct list));
	if(!newList){
		perror("no memory available");
		exit(1);
	}
	newList->head=malloc(sizeof(struct node));
	newList->head->next=NULL;
	newList->head->prev=NULL;
	if(pthread_mutex_init(&(newList->mutex),NULL))
		perror("mutex errror");
	return newList;
};

/*
* Namn: list_isEmpty
* Syfte: Kontrollerar om listan är tom
* Parametrar: lista som ska kontrolleras
* Returvärde: true om tom, annars false
*/

bool list_isEmpty(list* l){
	list_lock(l);
	bool result=l->head->next==NULL;
	list_unlock(l);
	return result;
};

/*
* Namn: list_hasNext
* Syfte: Kontrollerar om en position har ett nästa värde
* Parametrar: noden vars länk ska kollas
* Returvärde: true om den har en nästa länk, annars false
*/


bool list_hasNext(node_pos n){
	return n->next!=NULL;
};

/*
* Namn: list_next
* Syfte: Hämtar nästa värde i listan
* Parametrar: noden vars nästa värde ska hämtas.
* Returvärde: pekare till nästa nod.
*/

node_pos list_next(node_pos n){
	return n->next;
};

/*
* Namn: list_prev
* Syfte: Hämtar nästa värde i listan
* Parametrar: noden vars föregående värde ska hämtas.
* Returvärde: pekare till föregående nod.
*/

node_pos list_prev(node_pos n){
	return n->prev;
};

/*
* Namn: list_first
* Syfte: Hämtar första noden i listan.
* Parametrar: listan vars första position ska hämtas.
* Returvärde: pekare till första värdet i listan.
*/

node_pos list_first(list* l){
	return l->head;
};

/*
* Namn: list_insert
* Syfte: Lägger till ett värde på en specifik position i listan
* Parametrar: Position där värdet ska läggas till,
*				listan där värdet ska läggas till.
* Returvärde: position till det nya värdet.
*/


node_pos list_insert(list *l,node_pos n, value v){
	list_lock(l);
	node_pos newNode = malloc(sizeof(struct node));
	if(!newNode){
		perror("no memory available");
		exit(1);
	}
	newNode->value=v;
	newNode->prev=n;
	newNode->next=n->next;
	n->next=newNode;
	list_unlock(l);
	return newNode;
};

/*
* Namn: list_inspect
* Syfte: Läser av värdet på en specifik position.
* Parametrar: Positionen som ska läsas av.
* Returvärde: värdet från den specifierade positionen.
*/

value list_inspect(node_pos n){
	return n->value;
};

/*
* Namn: list_clear
* Syfte: Rensar listan på alla värden och frigör minnet för dessa värden.
* Parametrar: Listan som ska rensas,
*				en funktion som beskriver hur man ska fria värdets minne.
*/

void list_clear(list* l,freeFunc f){
	while(!list_isEmpty(l)){
		node_pos n = l->head->next;
		l->head->next=n->next;
		if(f!=NULL){
			f(n->value);
			free(n->value);
		}
		free(n);
	}
		free(l->head);
		free(l);
};


/*
* Namn: list_sort
* Syfte: Sorterar en lista med hjälp av en bubble sort.
* Parametrar: Listan som ska sorteras,
*				en funktion som beskriver hur man jämför två värden,
*				en funktion som beskriver hur man byter plats på två värden.
*/

void list_sort(list* l,compareFunc comp,swapFunc swap){
	bool swapped=true;
	while(swapped==true){
		swapped=false;
		node_pos n = list_first(l)->next;
		while(list_hasNext(n)){
			if(comp(n->value,n->next->value)){
				swap(n->value,n->next->value);
				swapped=true;
			}
			n=n->next;
		}
	}
};
/* Namn: list_pop
 * Syfte: tar bort första länken i listan och returnerar värdet till användaren
 * 			som behöver hantera frigöring av char pekaren.
 * Parametrar: Listan som värdet ska hämtas ifrån.
 * Returvärde: första värdet på listan.
 */
value list_pop(list *l){
	list_lock(l);
	node_pos n=l->head->next;
	char* v=n->value;
	l->head->next=n->next;
	if(n!=NULL)free(n);
	list_unlock(l);
	return v;
}
void list_lock(list* l){
	pthread_mutex_lock(&(l->mutex));
}
void list_unlock(list* l){
	pthread_mutex_unlock(&(l->mutex));
}
