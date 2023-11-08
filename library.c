#include "library.h"

#include <stdio.h>

// ||---------------||
// || DYNAMIC ARRAY ||
// ||---------------||

/*
 * Initialize dynamic array
 */
void initArray(Array *arr, size_t init_capacity) {
    arr->array = (void **)malloc(init_capacity * sizeof(void *));
    arr->size = 0;
    arr->capacity = init_capacity;
}

/*
 * Function lets you add an element to the array at any index.
 */
void insertArrayElement(Array *arr, void *element, size_t index) {
    // index not in bounds of array
    if (index > arr->size) {
        fprintf(stderr, "Index (%zu) out of bounds for array size (%zu)", index, arr->size);
        exit(EXIT_FAILURE);
    }
    // check if array needs resizing
    if (arr->size == arr->capacity) {
        arr->capacity *= 2; // double cap
        arr->array = (void **) realloc(arr->array, arr->capacity * sizeof(void *));
        // if realloc fails exit with error message
        if (arr->array == NULL) {
            fprintf(stderr, "Reallocation error adding element (%p) to dynamic array.", element);
            exit(EXIT_FAILURE);
        }
    }

    // shift elements to the right
    for (size_t i = arr->size; i > index; i--) {
        arr->array[i] = arr->array[i-1];
    }

    arr->array[arr->size] = element; // add element to end of array
    arr->size++; // increase size
}

/*
 * Gets element at index
 */
void *getArrayElement(Array *arr, size_t index) {
    if (index < arr->size) {
        return arr->array[index];
    }
}

/*
 * Free up space after usage.
 */
void freeArray(Array *arr) {
    free(arr->array);
    arr->size = 0;
    arr->capacity = 0;
}

// ||---------------||
// ||   HASH TABLE  ||
// ||---------------||

struct hashtab_s {
    size_t capacity;
    size_t size;
    size_t rehashes;
    Bucket *table;
    size_t (*hash)(const void *key);
    // these three are user defined!!
    bool (*equals)(const void *key1, const void *key2);
    void (*print)(const void *key, const void *value);
    void (*delete)(void *key, void *value);
};

/*
 * Calculates load factor for the hash table based on current size and capacity
 */
static double loadFactor(HashADT t) {
    return (double)t->size / t->capacity;
}

/*
 * Function to resize the hashtable when needed
 */
static void resize(HashADT t) {
    size_t newCapacity = t->capacity * RESIZE_FACTOR;
    t->rehashes++;
    Bucket *new_table = (Bucket *)calloc(newCapacity, sizeof(Bucket));
    if (new_table == NULL) {
        fprintf(stderr, "Memory allocation failed during resizing\n");
        exit(1);
    }

    for (size_t i = 0; i < t->capacity; i++) {
        if (t->table[i].isOccupied) {
            KeyValue *pair = t->table[i].pair;
            size_t index = t->hash(pair->key) % newCapacity;
            while (new_table[index].isOccupied) {
                index = (index + 1) % newCapacity;
            }
            new_table[index].pair = pair;
            new_table[index].isOccupied = true;
        }
    }

    free(t->table);
    t->table = new_table;
    t->capacity = newCapacity;
}

/*
 * Creates key-value pair
 */
static KeyValue *createPair(const void *key, const void *value) {
    KeyValue *pair = (KeyValue *)malloc(sizeof(KeyValue));
    if (pair == NULL) {
        fprintf(stderr, "Memory allocation failed during key-value pair creation");
        exit(1);
    }
    pair->key = key;
    pair->value = value;
    return pair;
}


/*
 * Creates a new HashADT instance
 */
HashADT ht_create(size_t (*hash)( const void *key),
                  bool (*equals)(const void *key1, const void *key2),
                  void (*print) ( const void *key, const void *value),
                  void (*delete)(void *key, void *value)) {
    HashADT t = (HashADT)malloc(sizeof(struct hashtab_s));
    if (t == NULL) {
        fprintf(stderr, "Memory allocation failed creating the hashtable");
        exit(1);
    }
    t->capacity = INITIAL_CAPACITY;
    t->size = 0;
    t->table = (Bucket *)calloc(t->capacity, sizeof(Bucket));
    if (t->table == NULL) {
        fprintf(stderr, "Memory allocation failed creating table's buckets");
        exit(1);
    }
    t->hash = hash;
    t->equals = equals;
    t->print = print;
    t->delete = delete;
    return t;
}

/*
 * Destroys specified HashADT, deallocating any dynamic storage
 */
void ht_destroy( HashADT t) {
    if (t == NULL) {
        fprintf(stderr, "Invalid table to destroy\n");
        exit(1);
    }
    for (size_t i = 0; i < t->capacity; i++) {
        if (t->table[i].isOccupied) {
            KeyValue *pair = t->table[i].pair;
            if (t->delete != NULL) {
                t->delete((void *)pair->key, (void *)pair->value);
            }
            free(pair);
        }
    }
    free(t->table);
    free(t);
}

/*
 * Prints all required information about the hashtable
 */
void ht_dump(const HashADT t, bool contents) {
    if (t == NULL) {
        fprintf(stderr, "Invalid table to dump\n");
        exit(1);
    }
    printf("Size: %zu\n", t->size);
    printf("Capacity: %zu\n", t->capacity);

    int collisions = 0;
    if (contents) {
        for (size_t i = 0; i < t->capacity; i++) {
            if (t->table[i].isOccupied) {
                KeyValue *pair = t->table[i].pair;
                if (pair != NULL) {
                    if (i != t->hash(pair->key) % t->capacity) {
                        collisions++;
                    }
                }
            }
        }
        printf("Collisions: %d\n", collisions);
        printf("Rehashes: %zu\n", t->rehashes);
        for (size_t i = 0; i < t->capacity; i++) {
            if (t->table[i].isOccupied) {
                KeyValue *pair = t->table[i].pair;
                if (pair != NULL && pair->key != NULL && pair->value != NULL) {
                    printf("%zu : ( ", i);
                    t->print(pair->key, pair->value);
                    printf(" )\n");
                }
            } else {
                printf("%zu : null\n", i);
            }
        }
    }
}

/*
 * Gets value with specified key
 */
const void *ht_get(const HashADT t, const void *key) {
    size_t index = t->hash(key) % t->capacity;
    while (t->table[index].isOccupied) {
        if (t->equals(key, t->table[index].pair->key)) {
            return t->table[index].pair->value;
        }
        index = (index + 1) % t->capacity;
    }
    return NULL;
}

/*
 * returns a boolean depending on if a key has a value or not
 */
bool ht_has(const HashADT t, const void *key) {
    return ht_get(t, key) != NULL;
}

/*
 * Puts a value at a key in the hashtable
 */
void *ht_put(HashADT t, const void *key, const void *value) {
    if (t == NULL || key == NULL) {
        fprintf(stderr, "Invalid table or key\n");
        exit(1);
    }
    if (loadFactor(t) >= LOAD_THRESHOLD) {
        resize(t);
    }

    size_t index = t->hash(key) % t->capacity;
    while (t->table[index].isOccupied) {
        if (t->equals(key, t->table[index].pair->key)) {
            void *old = (void *)t->table[index].pair->value;
            t->table[index].pair->value = value;
            return old;
        }
        index = (index + 1) % t->capacity;
    }
    KeyValue *pair = createPair(key, value);

    // get empty slot
    while (t->table[index].isOccupied) {
        index = (index + 1) % t->capacity;
    }
    // put pair in table
    t->table[index].pair = pair;
    t->table[index].isOccupied = true;
    t->size++; // Increment the size here

    // no old value for key
    return NULL;
}

/*
 * Returns array of all the keys
 */
void **ht_keys(const HashADT t) {
    if (t == NULL) {
        fprintf(stderr, "Invalid table to obtain keys\n");
    }

    void **keys = (void **)malloc(t->size * sizeof(void *));
    if (keys == NULL) {
        fprintf(stderr, "Memory allocation failed while creating keys\n");
        exit(1);
    }

    size_t keysIndex = 0;
    for (size_t i = 0; i < t->capacity; i++) {
        if (t->table[i].isOccupied) {
            keys[keysIndex] = (void *)t->table[i].pair->key;
            keysIndex++;
        }
    }

    return keys;
}

/*
 * Returns collection of values as array of pointers, allocates space which the caller is responsible
 * to deallocate
 */
void **ht_values(const HashADT t) {
    if (t == NULL) {
        fprintf(stderr, "Invalid table to return values\n");
        exit(1);
    }

    void **values = (void **)malloc(t->size * sizeof(void *));
    if (values == NULL) {
        fprintf(stderr, "Memory allocation error creating values\n");
        exit(1);
    }

    size_t valuesIndex = 0;
    for (size_t i = 0; i < t->capacity; i++) {
        if (t->table[i].isOccupied) {
            values[valuesIndex] = (void *)t->table[i].pair->value;
            valuesIndex++;
        }
    }
    return values;
}