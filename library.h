#ifndef ADTOOL_LIBRARY_H
#define ADTOOL_LIBRARY_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


// ||---------------||
// ||    Helpers    ||
// ||---------------||
typedef struct Node {
    void *data;
    struct Node *next;
} Node;
// helper to create new nodes (used for more than linked list)
Node *newNode(void *data);

// ||---------------||
// || DYNAMIC ARRAY ||
// ||---------------||
typedef struct {
    void **array;
    size_t size;
    size_t capacity;
} Array;

void initArray(Array *arr, size_t init_capacity);
void insertArrayElement(Array *arr, void *element, size_t index);
void *getArrayElement(Array *arr, size_t index);
void freeArray(Array *arr);

// ||---------------||
// ||   HASH TABLE  ||
// ||---------------||
/// Initial capacity of table upon creation
#define INITIAL_CAPACITY 16
/// The load at which the table will rehash
#define LOAD_THRESHOLD 0.75
/// The table size will double upon each rehash
#define RESIZE_FACTOR 2

typedef struct {
    const void *key;
    const void *value;
} KeyValue;

typedef struct {
    KeyValue *pair;
    bool isOccupied;
} Bucket;

typedef struct hashtab_s *HashADT;

HashADT ht_create(
        size_t (*hash)( const void *key ),
        bool (*equals)( const void *key1, const void *key2 ),
        void (*print)( const void *key, const void *value ),
        void (*delete)( void *key, void *value )
);

void ht_destroy( HashADT t );
void ht_dump( const HashADT t, bool contents );
const void *ht_get( const HashADT t, const void *key );
bool ht_has( const HashADT t, const void *key );
void *ht_put( HashADT t, const void *key, const void *value );
void **ht_keys( const HashADT t );
void **ht_values( const HashADT t );
static double loadFactor(HashADT t);

// ||---------------||
// ||  Linked List  ||
// ||---------------||
typedef struct {
    Node *head;
    size_t size;
} LinkedList;

void initLinkedList(LinkedList *list);
void insert(LinkedList *list, void *data);
bool isLinkedListEmpty(LinkedList *list);
void removeLinkedListNode(LinkedList *list, void *data);
void removeFirstLinkedNode(LinkedList *list);
size_t getLinkedListSize(LinkedList *list);
void freeLinkedList(LinkedList *list);


// ||---------------||
// ||     QUEUE     ||
// ||---------------||

// uses the Node struct made
typedef struct {
    struct Node *head;
    struct Node *tail;
    size_t size;
    size_t capacity;
} Queue;

void initQueue(Queue *queue, size_t capacity);
bool isQueueEmpty(Queue *queue);
bool isQueueFull(Queue *queue);
void *dequeue(Queue *queue);
void enqueue(Queue *queue, void *data);
size_t getQueueSize(Queue *queue);
void freeQueue(Queue *queue);

// ||---------------||
// ||     STACK     ||
// ||---------------||


#endif //ADTOOL_LIBRARY_H
