/* linkedList.h
   *  Thomas Sarlin
   *  Laboration 1: Systemnära programmering, HT-18
   *  Representerar en implementation av en dubbellänkad lista.
   */

#include <pthread.h>
#include <stdbool.h>
typedef void* value;
typedef void freeFunc(value);
typedef bool compareFunc(value,value);
typedef void swapFunc(value,value);

typedef struct node {
	void* value;
	struct node* next;
	struct node* prev;
}node;
typedef node* node_pos;
typedef struct list {
	struct node* head;
    pthread_mutex_t mutex;
}list;

/*
* Namn: list_create
* Syfte: Skapar en ny lista
* Parametrar: void
* Returvärde: pekare till den nya listan
*/

list* list_create(void);

/*
* Namn: list_isEmpty
* Syfte: Kontrollerar om listan är tom
* Parametrar: lista som ska kontrolleras
* Returvärde: true om tom, annars false
*/

bool list_isEmpty(list*);

/*
* Namn: list_hasNext
* Syfte: Kontrollerar om en position har ett nästa värde
* Parametrar: noden vars länk ska kollas
* Returvärde: true om den har en nästa länk, annars false
*/

bool list_hasNext(node_pos);

/*
* Namn: list_next
* Syfte: Hämtar nästa värde i listan
* Parametrar: noden vars nästa värde ska hämtas.
* Returvärde: pekare till nästa nod.
*/

node_pos list_next(node_pos);

/*
* Namn: list_prev
* Syfte: Hämtar nästa värde i listan
* Parametrar: noden vars föregående värde ska hämtas.
* Returvärde: pekare till föregående nod.
*/

node_pos list_prev(node_pos);

/*
* Namn: list_first
* Syfte: Hämtar första noden i listan.
* Parametrar: listan vars första position ska hämtas.
* Returvärde: pekare till första värdet i listan.
*/

node_pos list_first(list*);
node_pos list_last(list*);
/*
* Namn: list_insert
* Syfte: Lägger till ett värde på en specifik position i listan
* Parametrar: Position där värdet ska läggas till,
*				listan där värdet ska läggas till.
* Returvärde: position till det nya värdet.
*/

node_pos list_insert(list*,node_pos,value);
/*
* Namn: list_inspect
* Syfte: Läser av värdet på en specifik position.
* Parametrar: Positionen som ska läsas av.
* Returvärde: värdet från den specifierade positionen.
*/

value list_inspect(node_pos);

/*
* Namn: list_clear
* Syfte: Rensar listan på alla värden och frigör minnet för dessa värden.
* Parametrar: Listan som ska rensas,
*				en funktion som beskriver hur man ska fria värdets minne.
*/

void list_clear(list*,freeFunc);

/*
* Namn: list_sort
* Syfte: Sorterar en lista med hjälp av en bubble sort.
* Parametrar: Listan som ska sorteras,
*				en funktion som beskriver hur man jämför två värden,
*				en funktion som beskriver hur man byter plats på två värden.
*/
void list_sort(list*,compareFunc,swapFunc);

/* Namn: list_pop
 * Syfte: tar bort första länken i listan och returnerar värdet till användaren
 * 			som behöver hantera frigöring av char pekaren.
 * Parametrar: Listan som värdet ska hämtas ifrån.
 * Returvärde: första värdet på listan.
 */
value list_pop(list*);
void list_lock(list*);

void list_unlock(list*);
