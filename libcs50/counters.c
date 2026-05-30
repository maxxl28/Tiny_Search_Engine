#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "counters.h"
#include "mem.h"

typedef struct ctrnode {
    int key;
    int count;
    struct ctrnode* next;
} ctrnode_t;

struct counters {
    ctrnode_t* head;
};

static ctrnode_t* ctrnode_new(const int key){
    //takes in a key
    // creates a new node with that key and initializes count of that key to one
    ctrnode_t* node = mem_malloc(sizeof(ctrnode_t));
    if (node == NULL) {
        return NULL;
    }
    (*node).key = key;
    (*node).count = 1;
    (*node).next = NULL;
    return node;
}

counters_t* counters_new(void){
    //takes in nothing
    //creates a new linked list
    counters_t* ctrs = mem_malloc(sizeof(counters_t));
    if (ctrs == NULL) {
        return NULL;
    }
    (*ctrs).head = NULL;
    return ctrs;
}

int counters_add(counters_t* ctrs, const int key){
    //takes in a key and a linked list
    //finds that key if it exists and updates the count
    if (ctrs == NULL || key < 0) {
        return 0;
    }
    for (ctrnode_t* node = (*ctrs).head; node != NULL; node = (*node).next) { // finds key if it exists
        if ((*node).key == key) {
            (*node).count++;
            return (*node).count;
        }
    }
    ctrnode_t* node = ctrnode_new(key);
    if (node == NULL) {
        return 0;
    }
    (*node).next = (*ctrs).head;
    (*ctrs).head = node;
    return 1;
}

int counters_get(counters_t* ctrs, const int key){
    //takes in a key and a linked list of counters
    //returns the count of that key if it exists
    if (ctrs == NULL || key < 0) {
        return 0;
    }
    for (ctrnode_t* node = (*ctrs).head; node != NULL; node = (*node).next) { // finds key
        if ((*node).key == key) {
            return (*node).count;
        }
    }
    return 0;
}

bool counters_set(counters_t* ctrs, const int key, const int count){
    //takes in a key and a count
    //if that key exists, it will update its count to count
    if (ctrs == NULL || key < 0 || count < 0) {
        return false;
    }
    for (ctrnode_t* node = (*ctrs).head; node != NULL; node = (*node).next) { // finds the key if it exists
        if ((*node).key == key) {
            (*node).count = count;
            return true;
        }
    }
    //if we don't fin the key
    ctrnode_t* node = ctrnode_new(key);
    if (node == NULL) {
        return false;
    }
    (*node).count = count;
    (*node).next = (*ctrs).head;
    (*ctrs).head = node;
    return true;
}

void counters_print(counters_t* ctrs, FILE* fp){
    //prints all the keys and their counts to the file
    if (fp == NULL) {
        return;
    }
    if (ctrs == NULL) {
        fputs("(null)", fp);
        return;
    }
    fputc('{', fp);
    for (ctrnode_t* node = (*ctrs).head; node != NULL; node = (*node).next) {
        fprintf(fp, "%d=%d", (*node).key, (*node).count);
        if ((*node).next != NULL) {
            fputs(", ", fp);
        }
    }
    fputc('}', fp);
}

void counters_iterate(counters_t* ctrs, void* arg, void (*itemfunc)(void* arg, const int key, const int count)){
    //iterates through all the keys and applies the function to it
    if (ctrs == NULL || itemfunc == NULL) {
        return;
    }
    for (ctrnode_t* node = (*ctrs).head; node != NULL; node = (*node).next) {
        (*itemfunc)(arg, (*node).key, (*node).count);
    }
}

void counters_delete(counters_t* ctrs){
    //deletes the linked list of keys and frees memory
    if (ctrs == NULL) {
         return;
    }
    ctrnode_t* node = (*ctrs).head;
    while (node != NULL) {
        ctrnode_t* next = (*node).next;
        mem_free(node);
        node = next;
    }
    mem_free(ctrs);
}
