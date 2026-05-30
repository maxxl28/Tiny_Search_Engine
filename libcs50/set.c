#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "set.h"
#include "mem.h"

typedef struct setnode {
    char* key;
    void* item;
    struct setnode* next;
} setnode_t;

struct set {
    setnode_t* head;
};

static setnode_t* setnode_new_helper(const char* key, void* item){
    //creates a new setnode
    setnode_t* node = mem_malloc(sizeof(setnode_t));
    if (node == NULL) {
        return NULL;
    }
    (*node).key = mem_malloc(strlen(key) + 1);
    if ((*node).key == NULL) {
        mem_free(node);
        return NULL;
    }
    strcpy((*node).key, key);
    (*node).item = item;
    (*node).next = NULL;
    return node;
}

set_t* set_new(void){
    //creates a new set
    set_t* set = mem_malloc(sizeof(set_t));
    if (set == NULL) {
        return NULL;
    }
    (*set).head = NULL;
    return set;
}

bool set_insert(set_t* set, const char* key, void* item){
    //inserts a key value pair in the set
    //if that key already exists then it does nothing
    if (set == NULL || key == NULL || item == NULL) {
        return false;
    }
    for (setnode_t* node = (*set).head; node != NULL; node = (*node).next) { // this checks if the key alr exists
        if (strcmp((*node).key, key) == 0) {
            return false;  
        }
    }
    setnode_t* node = setnode_new_helper(key, item);
    if (node == NULL) {
        return false;
    }
    //insert
    (*node).next = (*set).head;
    (*set).head = node;
    return true;
}

void* set_find(set_t* set, const char* key){
    //finds a key in the set if the key exists
    if (set == NULL || key == NULL) {

        return NULL;
    }
    for (setnode_t* node = (*set).head; node != NULL; node = (*node).next) {
        if (strcmp((*node).key, key) == 0) {
            return (*node).item;
        }
    }
    return NULL;
}

void set_print(set_t* set, FILE* fp, void (*itemprint)(FILE* fp, const char* key, void* item) ){
    //prints the set
    if (fp == NULL) {
        return;
    }
    if (set == NULL) {
        fputs("(null)", fp);
        return;
    }
    fputc('{', fp);
    for (setnode_t* node = (*set).head; node != NULL; node = (*node).next) {
        if (itemprint != NULL) {
            (*itemprint)(fp, (*node).key, (*node).item);
        }
        if ((*node).next != NULL) {
            fputs(", ", fp);
        }
    }
    fputc('}', fp);
}

void set_iterate(set_t* set, void* arg, void (*itemfunc)(void* arg, const char* key, void* item) ){
    //iterates through the set and applies the function to each key value pair
    if (set == NULL || itemfunc == NULL) {
        return;
    }
    for (setnode_t* node = (*set).head; node != NULL; node = (*node).next) {
        (*itemfunc)(arg, (*node).key, (*node).item);
    }
}

void set_delete(set_t* set, void (*itemdelete)(void* item) ){
    //deletes the set and frees memory
    if (set == NULL) {
        return;
    }
    setnode_t* node = (*set).head;
    while (node != NULL) {
        setnode_t* next = (*node).next;
        if (itemdelete != NULL) {
            (*itemdelete)((*node).item);
        }
        mem_free((*node).key);
        mem_free(node);
        node = next;
    }
    mem_free(set);
}
