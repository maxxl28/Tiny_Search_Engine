#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "hashtable.h"
#include "set.h"
#include "mem.h"
#include "hash.h"

struct hashtable {
    int num_slots;
    set_t** slots;  //arry of pointers
};

hashtable_t* hashtable_new(const int num_slots){
    if (num_slots <= 0) {
        return NULL;
    }
    hashtable_t* ht = mem_malloc(sizeof(hashtable_t));
    if (ht == NULL) {
        return NULL;
    }
    (*ht).num_slots = num_slots;
    (*ht).slots = mem_malloc(num_slots * sizeof(set_t*));//allocate array of pointers
    //initialize each to empty mem
    if ((*ht).slots == NULL) {
        mem_free(ht);
        return NULL;
    }
    for (int i = 0; i < num_slots; i++) {
        (*ht).slots[i] = set_new();
    }
    return ht;
}

bool hashtable_insert(hashtable_t* ht, const char* key, void* item){
    //takes in a hashtable, a key and an iterm
    //inserts iit
    if (ht == NULL || key == NULL || item == NULL) {
        return false;
    }
    // hash the key to find which slot to insert into
    int slot = hash_jenkins(key, (*ht).num_slots);
    return set_insert((*ht).slots[slot], key, item);
}

void* hashtable_find(hashtable_t* ht, const char* key){
    //takes in a hashtable and a key
    //returns the value
    if (ht == NULL || key == NULL) {
        return NULL;
    }
    // hash the key to find which slot to look into
    int slot = hash_jenkins(key, (*ht).num_slots);
    return set_find((*ht).slots[slot], key);
}

void hashtable_print(hashtable_t* ht, FILE* fp, void (*itemprint)(FILE* fp, const char* key, void* item)){
    //takes in a hashtable and a file
    //prints all objects of the hastable to the file 
    if (fp == NULL) {
        return;
    }
    if (ht == NULL) {
        fputs("(null)", fp);
        return;
    }
    for (int i = 0; i < (*ht).num_slots; i++) {
        fprintf(fp, "Number %d: ", i);
        set_print((*ht).slots[i], fp, itemprint);
        fputc('\n', fp);
    }
}

void hashtable_iterate(hashtable_t* ht, void* arg, void (*itemfunc)(void* arg, const char* key, void* item)){
    //takes in a hashtable and a function
    // iterates through the hastable and applies the function
    if (ht == NULL || itemfunc == NULL) {
        return;
    }
    for (int i = 0; i < (*ht).num_slots; i++) {
        set_iterate((*ht).slots[i], arg, itemfunc);
    }
}

void hashtable_delete(hashtable_t* ht, void (*itemdelete)(void* item)){
    //deletes the hastable and frees all memory
    if (ht == NULL) {
        return;
    }
    for (int i = 0; i < (*ht).num_slots; i++) {
        set_delete((*ht).slots[i], itemdelete);
    }
    mem_free((*ht).slots);
    mem_free(ht);
}
