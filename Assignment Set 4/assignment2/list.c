#include "list.h"
#include <stdlib.h>
#include <stdio.h>
#include "mythreads.h"

int init_list(list_t *list) {

    list->size = 0;
    list->nextId = 0;
    list->head = malloc(sizeof(node_t));
    list->head->thread.id = -1;
    list->head->next = list->head;
    list->head->prev = list->head;

    return 0;
}

int append(list_t *list, node_t *node, char idExists){
    if(list == NULL){
        printf("Not initialized list");
        return -1;
    }
    
    if(idExists == 0){
        node->thread.id = list->nextId;
        list->nextId++;
    }
    list->size++;     
    
    node_t* last = list->head->prev;
    node_t* first = list->head;
    last->next = node;
    first->prev = node;
    node->prev = last;
    node->next = first;
    
    return 0;
}

int search_for_blocked(list_t* proccess_list, list_t *blocked_list,unsigned blocker_id, char ReasonForBlock) {
    node_t *curr, *unblocked, *next;
    // printf("Entered search for blocked by %u\n", blocker_id);

    for (curr = blocked_list->head->next; curr != blocked_list->head; curr = next) {
        next = curr->next;
        if (curr->thread.state == ReasonForBlock && curr->thread.blockerId == blocker_id) {
            unblocked = detach_byaddress(curr);

            blocked_list->size--;
            unblocked->thread.state = RUNNING;
            printf("Unblocked Process %u\n", unblocked->thread.id);
            append(proccess_list, unblocked, 1);
            if(ReasonForBlock == BLOCKED_BY_SEM){
                return 1; //found
            }
        }
    }
    return 0;
}

node_t *search_list(list_t *list, unsigned id) {
    node_t *curr;

    for (curr = list->head; curr->thread.id != id && curr->next != list->head; curr = curr->next);

    if (curr->thread.id == id) {
        return curr;
    }
    
    return NULL;
}

int search_and_delete(list_t *list, unsigned id) {
    node_t *curr;

    for (curr = list->head->next; curr->thread.id != id; curr = curr->next);

    curr->prev->next = curr->next;
    curr->next->prev = curr->prev;
    
    list->size--;
    mycoroutines_destroy(&curr->thread.coroutine);
    free(curr);

    return 0;
}

int clear(list_t *list) {
    node_t *curr, *next;

    for (curr = list->head->next; curr != list->head; curr = next) {
        next = curr->next;
        destroy_node(curr);
    }
    destroy_node(list->head);
    list->size = 0;
    list->head = NULL;
    if (list->scheduler != NULL) {
        mycoroutines_destroy(&list->scheduler->coroutine);
    }
    if (list->blocker != NULL) {
        mycoroutines_destroy(&list->blocker->coroutine);
    }
    free(list);

    return 0;
}

void print_list(list_t *list) {
    node_t *curr;

    printf("Node id: %d \n", list->head->thread.id);
    for (curr = list->head->next; curr != list->head; curr = curr->next) {
        printf("Node id: %u \n", curr->thread.id);
        printf("blocked by id: %u \n", curr->thread.blockerId);
    }
    printf("\n");
}

node_t* detach_byaddress(node_t *detachable){
	node_t *prv = detachable->prev ,*nxt =detachable->next;
	prv->next = nxt;
	nxt->prev = prv;
    
	detachable->next = (detachable->prev = NULL);
	return(detachable);
}

void destroy_node(node_t *node) {
    mycoroutines_destroy(&node->thread.coroutine);
    free(node);
}