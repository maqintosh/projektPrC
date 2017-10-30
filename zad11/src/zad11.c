/*
 ============================================================================
 Name        : zad11.c
 Author      : Mateusz Cieslinski
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================

2. W pliku nagłówkowym przygotować strukturę, która będzie wykorzystana do
przechowywania elementów bufora w liście wiązanej.
3. Zadeklarować zmienne pozwalające na powiązanie elementów listy,
czyli dla: dowiązania nowego elementu, wskazania poprzedniego elementu i wskazania pierwszego elementu (punktu zakotwiczenia).
4. Przygotować i przypisać wartości dla zmiennych pierwszego elementu na liście.
5. Dodać do listy elementy, tak aby zawierała 10 par wartości.
6. Wyświetlić zawartość poszczególnych elementów listy.
7. Usunąć piąty i siódmy element z listy (pamiętać o zapewnieniu spójności listy).
8. Wyświetlić zawartość poszczególnych elementów listy.



 */

#include <stdio.h>
#include <stdlib.h>
#include "zad11.h"

int main(void) {
	//node_t head;
	node_t * head = NULL;
	head = malloc(sizeof(node_t));
	if (head == NULL) {
	    return 1;
	}

	head->val = 1;
	for(int i=2;i<=10;i++){
	push(head,i);}

	//head->next->val = 2;
	//head->next->next = NULL;
	print_list(head);
	printf("\nTERAZ USUWAM ELEMENT 5 i 7\n");
	remove_by_index(&head,5);
	remove_by_index(&head,7);
	//pop(&head);
	print_list(head);
	//deleteNode(&head, 4);
	return EXIT_SUCCESS;
}

void print_list(node_t * head) {
	    node_t * current = head;
	    //printf("LISTA ERROR ? \n");
	    printf("Element_:");
	    while (current != NULL) {

	        printf("%d " ,current->val);
	        current = current->next;
	    }
	}

void push(node_t * head, int val) {
    node_t * current = head;
    while (current->next != NULL) {
        current = current->next;
    }

    /* now we can add a new variable */
    current->next = malloc(sizeof(node_t));
    current->next->val = val;
    current->next->next = NULL;
}

int remove_by_index(node_t ** head, int n) {
    //int i = 0;
    int retval = -1;
    node_t * current = *head;
    node_t * temp_node = NULL;

    if (n == 0)
        {
            *head = temp_node->next;   // Change head
            free(temp_node);               // free old head
            //return;
        }

    for (int i = 0; i < n-1; i++) {
        if (current->next == NULL) {
            return -1;
        }
        current = current->next;
    }

    temp_node = current->next;
    retval = temp_node->val;
    current->next = temp_node->next;
    free(temp_node);

    return retval;

}


int pop(node_t ** head) {
    int retval = -1;
    node_t * next_node = NULL;

    if (*head == NULL) {
        return -1;
    }

    next_node = (*head)->next;
    retval = (*head)->val;
    free(*head);
    *head = next_node;

    return retval;
}


void deleteNode(struct node **head_ref, int position)
{
   // If linked list is empty
   if (*head_ref == NULL)
      return;

   // Store head node
   struct node* temp = *head_ref;

    // If head needs to be removed
    if (position == 0)
    {
        *head_ref = temp->next;   // Change head
        free(temp);               // free old head
        return;
    }

    // Find previous node of the node to be deleted
    for (int i=0; temp!=NULL && i<position-1; i++)
         temp = temp->next;

    // If position is more than number of ndoes
    if (temp == NULL || temp->next == NULL)
         return;

    // Node temp->next is the node to be deleted
    // Store pointer to the next of node to be deleted
    struct node *next = temp->next->next;

    // Unlink the node from linked list
    free(temp->next);  // Free memory

    temp->next = next;  // Unlink the deleted node from list
}


