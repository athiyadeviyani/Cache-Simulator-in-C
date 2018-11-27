/***************************************************************************
 * *    Inf2C-CS Coursework 2: Cache Simulation
 * *
 * *    Instructor: Boris Grot
 * *
 * *    TA: Siavash Katebzadeh
 ***************************************************************************/

/*
TERMINAL COMMANDS
gcc -o mem_sim mem_sim.c -std=gnu99 -lm
./mem_sim FIFO 4 256 32 mem_trace.txt
*/

// 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <inttypes.h>
#include <math.h>
#include <time.h>
/* Do not add any more header files */

/*
 * Various structures
 */
typedef enum {FIFO, LRU, Random} replacement_p;

const char* get_replacement_policy(uint32_t p) {
    switch(p) {
    case FIFO: return "FIFO";
    case LRU: return "LRU";
    case Random: return "Random";
    default: assert(0); return "";
    };
    return "";
}

typedef struct {
    uint32_t address;
} mem_access_t;

// These are statistics for the cache and should be maintained by you.
typedef struct {
    uint32_t cache_hits;
    uint32_t cache_misses;
} result_t;


/*
 * Parameters for the cache that will be populated by the provided code skeleton.
 */

replacement_p replacement_policy = FIFO;
uint32_t associativity = 0;
uint32_t number_of_cache_blocks = 0;
uint32_t cache_block_size = 0;


/*
 * Each of the variables below must be populated by you.
 */
uint32_t g_num_cache_tag_bits = 0;
uint32_t g_cache_offset_bits= 0;
result_t g_result;


/* Reads a memory access from the trace file and returns
 * 32-bit physical memory address
 */
mem_access_t read_transaction(FILE *ptr_file) {
    char buf[1002];
    char* token = NULL;
    char* string = buf;
    mem_access_t access;

    if (fgets(buf, 1000, ptr_file)!= NULL) {
        /* Get the address */
        token = strsep(&string, " \n");
        access.address = (uint32_t)strtoul(token, NULL, 16);
        return access;
    }

    /* If there are no more entries in the file return an address 0 */
    access.address = 0;
    return access;
}

void print_statistics(uint32_t num_cache_tag_bits, uint32_t cache_offset_bits, result_t* r) {
    /* Do Not Modify This Function */

    uint32_t cache_total_hits = r->cache_hits;
    uint32_t cache_total_misses = r->cache_misses;
    printf("CacheTagBits:%u\n", num_cache_tag_bits);
    printf("CacheOffsetBits:%u\n", cache_offset_bits);
    printf("Cache:hits:%u\n", r->cache_hits);
    printf("Cache:misses:%u\n", r->cache_misses);
    printf("Cache:hit-rate:%2.1f%%\n", cache_total_hits / (float)(cache_total_hits + cache_total_misses) * 100.0);
}

/*
 *
 * Add any global variables and/or functions here as needed.
 *
 */



    


int main(int argc, char** argv) {
    time_t t;
    /* Intializes random number generator */
    /* Important: DO NOT call this function anywhere else. */
    srand((unsigned) time(&t));
    /* ----------------------------------------------------- */
    /* ----------------------------------------------------- */

    /*
     *
     * Read command-line parameters and initialize configuration variables.
     *
     */
    int improper_args = 0;
    char file[10000];
    if (argc < 6) {
        improper_args = 1;
        printf("Usage: ./mem_sim [replacement_policy: FIFO LRU Random] [associativity: 1 2 4 8 ...] [number_of_cache_blocks: 16 64 256 1024] [cache_block_size: 32 64] mem_trace.txt\n");
    } else  {
        /* argv[0] is program name, parameters start with argv[1] */
        if (strcmp(argv[1], "FIFO") == 0) {
            replacement_policy = FIFO;
        } else if (strcmp(argv[1], "LRU") == 0) {
            replacement_policy = LRU;
        } else if (strcmp(argv[1], "Random") == 0) {
            replacement_policy = Random;
        } else {
            improper_args = 1;
            printf("Usage: ./mem_sim [replacement_policy: FIFO LRU Random] [associativity: 1 2 4 8 ...] [number_of_cache_blocks: 16 64 256 1024] [cache_block_size: 32 64] mem_trace.txt\n");
        }
        associativity = atoi(argv[2]);
        number_of_cache_blocks = atoi(argv[3]);
        cache_block_size = atoi(argv[4]);
        strcpy(file, argv[5]);
    }
    if (improper_args) {
        exit(-1);
    }
    assert(number_of_cache_blocks == 16 || number_of_cache_blocks == 64 || number_of_cache_blocks == 256 || number_of_cache_blocks == 1024);
    assert(cache_block_size == 32 || cache_block_size == 64);
    assert(number_of_cache_blocks >= associativity);
    assert(associativity >= 1);

    printf("input:trace_file: %s\n", file);
    printf("input:replacement_policy: %s\n", get_replacement_policy(replacement_policy));
    printf("input:associativity: %u\n", associativity);
    printf("input:number_of_cache_blocks: %u\n", number_of_cache_blocks);
    printf("input:cache_block_size: %u\n", cache_block_size);
    printf("\n");

    


    /* Open the file mem_trace.txt to read memory accesses */
    FILE *ptr_file;
    ptr_file = fopen(file,"r");
    if (!ptr_file) {
        printf("Unable to open the trace file: %s\n", file);
        exit(-1);
    }

    /* result structure is initialized for you. */
    memset(&g_result, 0, sizeof(result_t));

    /* Do not delete any of the lines below.
     * Use the following snippet and add your code to finish the task. */

    /* You may want to setup your Cache structure here. */

    // Struct for the cache block
    typedef struct {
      uint32_t valid;
      uint32_t tag;
      int block_age;
    } Block;

    // Struct for the cache set
    typedef struct {
      Block* blocks;
    } Set;

    // Final cache structure
    typedef struct {
      Set* sets;
    } Cache;

    // Bit calculations
    uint32_t g_cache_index_bits = 0;
    uint32_t no_of_sets = 0;

    no_of_sets = number_of_cache_blocks/associativity;

    g_cache_offset_bits = log(cache_block_size) / log(2);
    g_cache_index_bits = (int) (log(no_of_sets) / log(2));
    g_num_cache_tag_bits = 32 - g_cache_offset_bits - g_cache_index_bits;

    int random = 0;

    int k = 0;

    // Dynamically allocate memory for the cache block, the cache set, and the cache
    Cache myCache;
    myCache.sets = (Set*) malloc(no_of_sets * sizeof(Set));

    for (k = 0; k < no_of_sets; k++) {
        myCache.sets[k].blocks = (Block*)malloc(sizeof(Block) * (associativity));
    }

    int i = 0;
    int j = 0;

    for (i = 0; i < no_of_sets; i++) {
        for (j = 0; j < associativity; j++) {
            myCache.sets[i].blocks[j].valid = 0;
            myCache.sets[i].blocks[j].tag = 0;
            myCache.sets[i].blocks[j].block_age = 0;
        }
    }

    // blocks per set = associativity
    int blocks_per_set = number_of_cache_blocks / no_of_sets;

    // Dynamically allocate the array that stores the FIFO and LRU indexes of each set
    int* FIFO_index_array = (int*) malloc(no_of_sets * sizeof(int));
    int* LRU_index_array = (int*) malloc(no_of_sets * sizeof(int));

    int l = 0;

    // Initialize each index in each array to 0
    for (l = 0; l < blocks_per_set; l++) {
        FIFO_index_array[l] = 0;
        LRU_index_array[l] = 0;
    }

    mem_access_t access;
    /* Loop until the whole trace file has been read. */
    while(1) {

        access = read_transaction(ptr_file);
        // If no transactions left, break out of loop.
        if (access.address == 0)
            break;
        

        /* Add your code here */

        // Extract the tag, index, and offset from the address 
        uint32_t tag = access.address >> (g_cache_index_bits + g_cache_offset_bits);
        uint32_t index = ((1 << g_cache_index_bits) - 1) & access.address >> g_cache_offset_bits;
        uint32_t offset = access.address << (g_num_cache_tag_bits + g_cache_index_bits);
        
        int i;
        
        // go through all the blocks in the set
        for (i = 0; i < blocks_per_set; i++) {
            
            // event handler for when the block is empty
            if (myCache.sets[index].blocks[i].valid == 0) {  

                // assign the tag to the block
                myCache.sets[index].blocks[i].tag = tag;

                // mark it as not empty (valid)
                myCache.sets[index].blocks[i].valid = 1;            

                // increase the block age for all the other blocks
                int j = 0;
                for (j = 0; j < blocks_per_set; j++) {
                    myCache.sets[index].blocks[j].block_age = myCache.sets[index].blocks[j].block_age + 1;
                }

                // set the block age of the current block to 0 (because it's the most recently accessed)
                myCache.sets[index].blocks[i].block_age = 0;

                // increment the number of misses
                g_result.cache_misses++;

                break;
            } 

            // event handler for when the block is FULL but it is a HIT
            // condition for a hit is when the address tag is equal to the tag in the cache block
            else if (myCache.sets[index].blocks[i].tag == tag && myCache.sets[index].blocks[i].valid == 1) {

                // set the block age of the current block to 0 (because it's the most recently accessed)
                myCache.sets[index].blocks[i].block_age = 0;

                // increase the block age for all the other blocks
                int j = 0;
                for (j = 0; j < blocks_per_set; j++) {
                    myCache.sets[index].blocks[j].block_age = myCache.sets[index].blocks[j].block_age + 1;
                }

                // set the block age of the current block to 0 (because it's the most recently accessed)
                myCache.sets[index].blocks[i].block_age = 0;

                // increment the number of hits
                g_result.cache_hits++;

                break;
            }

            // event handler for when the block is full and the entire block has been searched and a hit has not been found
            // i == (blocks_per_set - 1) ensures that the entire block has been searched
            else if (replacement_policy == Random && ( i == (blocks_per_set - 1)) ) {

                // generates a random integer
                random = rand()%(blocks_per_set);

                // assign the tag to the block that has the random integer as the block number
                myCache.sets[index].blocks[random].tag = tag;

                // mark the block valid
                myCache.sets[index].blocks[random].valid = 1;

                // increment the number of misses              
                g_result.cache_misses++;
                
                break;
            }

            else if (replacement_policy == LRU && ( i == (blocks_per_set - 1))) {

                // find the oldest block in the set (maximum block age)
                int k = 0;
                int max = -1;
                for (k = 0; k < blocks_per_set; k++) {
                    if (myCache.sets[index].blocks[k].block_age > max) {
                        max = myCache.sets[index].blocks[k].block_age;
                        LRU_index_array[index] = k;
                    }
                }

                // increment the number of misses
                g_result.cache_misses++;


                // increase the block age for all the other blocks
                int j = 0;
                for (j = 0; j < blocks_per_set; j++) {
                    myCache.sets[index].blocks[j].block_age = myCache.sets[index].blocks[j].block_age + 1;
                }

                // asign the address tag to the current block that is the least recently used block in the set (oldest block age)
                myCache.sets[index].blocks[LRU_index_array[index]].tag = tag;

                // mark the block valid
                myCache.sets[index].blocks[LRU_index_array[index]].valid = 1;

                // set the block age of the current block to 0 (because it's the most recently accessed)
                myCache.sets[index].blocks[LRU_index_array[index]].block_age = 0;
            
                break;
                
            }

            else if (replacement_policy == FIFO && ( i == (blocks_per_set - 1))) {

                // The FIFO index for each set is different
                // But it moves through the entire block and moves back to the first position once it has reached the end
                // hence the modulo operation is used here
                FIFO_index_array[index] = FIFO_index_array[index] % blocks_per_set;

                // assign the address tag to the current block which is the first in block in the set
                myCache.sets[index].blocks[FIFO_index_array[index]].tag = tag;

                // mark the block valid
                myCache.sets[index].blocks[FIFO_index_array[index]].valid = 1;

                // increment the number of misses
                g_result.cache_misses++;

                // increment the FIFO index for this particular set
                FIFO_index_array[index]++;
            
                break;
            }

        }

    }
    
    // free the memory used by the previous cache set, cache block, and cache allocation
    int a;
    for (a = 0; a < no_of_sets; a++) {
        free(myCache.sets[a].blocks);
    }
    free(myCache.sets);

    /* Do not modify code below. */
    /* Make sure that all the parameters are appropriately populated. */
    print_statistics(g_num_cache_tag_bits, g_cache_offset_bits, &g_result);

    /* Close the trace file. */
    fclose(ptr_file);
    return 0;
}
