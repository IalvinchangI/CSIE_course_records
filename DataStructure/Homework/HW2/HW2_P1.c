#include<stdio.h>
#include<stdlib.h>
#include<string.h>

// key type
#define ktype int
// the number represent no data
#define EMPTY -1

typedef struct __hashTable {
    int bucket;
    int slot;
    ktype *container;
} _HashTable, *HashTable;


int hash(HashTable, ktype);


HashTable HashTable_create(int , int);
void HashTable_free(HashTable);
void HashTable_insert(HashTable, ktype);
int  HashTable_search(HashTable, ktype);
void HashTable_delete(HashTable, ktype);
void HashTable_print(HashTable);


int main(void) {
    // setting
    int bucket = 0;
    int slot = 0;

    char title[16] = {0};  // 6(bucket/slot) + 1 + 3(int) + 1 + ?(buffer)
    fgets(title, 15, stdin);
    if (sscanf(title, "bucket %d", &bucket) != 1) {
        printf(">>> Input Error: input bucket size <<<");
        exit(EXIT_FAILURE);
    }
    memset(title, 0, 16 * sizeof(char));
    fgets(title, 15, stdin);
    if (sscanf(title, "slot %d", &slot) != 1) {
        printf(">>> Input Error: input slot size <<<");
        exit(EXIT_FAILURE);
    }

    HashTable table = HashTable_create(bucket, slot);

    // operation
    int key = 0;
    while (1) {
        char input[16] = {0};  // 6(delete/insert/search) + 1 + 5(int) + 1 + ?(buffer)
        fgets(input, 15, stdin);
        // printf("%s", input);

        if (strcmp(input, "exit\n") == 0) {
            break;
        }

        if (sscanf(input, "insert %d", &key) == 1) {
            HashTable_insert(table, key);
            HashTable_print(table);
        }
        else if (sscanf(input, "delete %d", &key) == 1) {
            HashTable_delete(table, key);
            HashTable_print(table);
        }
        else if (sscanf(input, "search %d", &key) == 1) {
            int index = HashTable_search(table, key);
            printf("%d %d\n", index / table->slot, index % table->slot);
        }
        else {  // not support
            break;
        }
    }

    HashTable_free(table);
    
    return 0;
}


/**
 * @brief hash function
 * 
 * @param table HashTable object
 * @param key 
 * @return the index after hashing key
 */
int hash(HashTable table, ktype key) {
    return key % table->bucket;
}


// =============================== define the method of HashTable =============================== //

/**
 * @brief create a HashTable
 * 
 * @param bucket the bucket size
 * @param slot the slot size
 * @return HashTable object
 */
HashTable HashTable_create(int bucket, int slot) {
    HashTable out = (HashTable) malloc(sizeof(_HashTable));
    if (out == NULL) {
        printf(">>> Out of Memory: malloc a new HashTable <<<");
        exit(EXIT_FAILURE);
    }

    int containerSize = bucket * slot * sizeof(ktype);
    ktype *container = (ktype*) malloc(containerSize);
    if (container == NULL) {
        printf(">>> Out of Memory: malloc a container in the new HashTable <<<");
        exit(EXIT_FAILURE);
    }
    memset(container, EMPTY, containerSize);

    out->bucket = bucket;
    out->slot = slot;
    out->container = container;

    return out;
}

/**
 * @brief free all the memory in HashTable
 * 
 * @param table HashTable object
 */
void HashTable_free(HashTable table) {
    free(table->container);
    free(table);
}

/**
 * @brief insert a key into HashTable
 * 
 * @param table HashTable object
 * @param key 
 */
void HashTable_insert(HashTable table, ktype key) {
    // the index in container
    int index = hash(table, key) * table->slot;

    // insert
    int i = 0;
    int tableSize = table->slot * table->bucket;
    while (1) {
        if (i >= tableSize) {  // no space
            printf(">>> No Enough Space: insert a new key into HashTable <<<\n");
            break;
        }
        if (table->container[index] == EMPTY) {  // find space to store key
            table->container[index] = key;
            break;
        }

        index = ++index % tableSize;
        i++;
    }
}

/**
 * @brief search a key in HashTable
 * 
 * @param table HashTable object
 * @param key 
 * @return the index of the container or return -1 if the key is not in the HashTable
 */
int HashTable_search(HashTable table, ktype key) {
    // the index in container
    int index = hash(table, key) * table->slot;

    // search
    int i = 0;
    int tableSize = table->slot * table->bucket;
    while (1) {
        if (table->container[index] == EMPTY || i >= tableSize) {  // not found
            return -1;
        }
        if (table->container[index] == key) {  // found
            return index;
        }

        index = ++index % tableSize;
        i++;
    }
}

/**
 * @brief delete a key in HashTable
 * 
 * @param table HashTable object
 * @param key 
 */
void HashTable_delete(HashTable table, ktype key) {
    int bucket = hash(table, key);
    // the index in container
    int index = bucket * table->slot;

    // delete
    int i = 0;
    int tableSize = table->slot * table->bucket;
    while (1) {
        if (table->container[index] == EMPTY || i >= tableSize) {  // not found
            return;
        }
        if (table->container[index] == key) {  // found
            table->container[index] = EMPTY;
            break;
        }

        index = ++index % tableSize;
        i++;
    }

    // shift   // it seems that there will be a infinit loop
    int next = (index + 1) % tableSize;
    int startIndex = index;
    int indexBucket = index / table->slot;
    while (table->container[next] != EMPTY && next != startIndex) {
        // key = table->container[next];
        // printf("%d [%d, %d]\n", key, index, next);
        // HashTable_print(table);
        int nextBucket = hash(table, table->container[next]);
        if (next < startIndex && bucket == nextBucket) {
            // go back to the original bucket
            break;
        }
        if (
            // next value places at right bucket
            (nextBucket == (next / table->slot) && nextBucket != (index / table->slot)) || 
            // next value does not belong to the bucket which index direct to
            (bucket <= indexBucket && (nextBucket < bucket || indexBucket < nextBucket)) || 
            (bucket > indexBucket && (bucket < nextBucket || nextBucket < indexBucket))
        ) {
            next = ++next % tableSize;
            continue;
        }
        // move
        table->container[index] = table->container[next];
        table->container[next] = EMPTY;

        // next
        index = next;
        indexBucket = index / table->slot;
        next = ++next % tableSize;
    }
}

/**
 * @brief print the HashTable
 * 
 * @param table HashTable object
 */
void HashTable_print(HashTable table) {
    int* wordCount = (int*) malloc(table->bucket * sizeof(int));

    for (int bucket = 0; bucket < table->bucket; bucket++) {
        wordCount[bucket] = 4;
        printf("[ ");
        for (int slot = 0; slot < table->slot; slot++) {
            wordCount[bucket] += printf("%d ", table->container[bucket * table->slot + slot]);
        }
        printf("] ");
    }
    printf("\n");
    for (int bucket = 0; bucket < table->bucket; bucket++) {
        wordCount[bucket] -= printf("%d", bucket);
        for (int i = 0; i < wordCount[bucket]; i++) {
            printf(" ");
        }
    }
    printf("\n");

    free(wordCount);
}
