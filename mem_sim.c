/***************************************************************************
 * *    Inf2C-CS Coursework 2: Cache Simulation
 * *
 * *    Instructor: Boris Grot
 * *
 * *    TA: Siavash Katebzadeh
 ***************************************************************************/

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

    
    //  STRUCT FOR CACHE BLOCK
    typedef struct {
      uint32_t valid;
      uint32_t tag;
      int block_age;
    } Block;

    // STRUCT FOR CACHE SET
    typedef struct {
      Block* blocks;
    } Set;

    // STRUCT FOR ACTUAL CACHE
    typedef struct {
      Set* sets;
    } Cache;

    // dynamically allocate the size of a cache set
    
    uint32_t g_cache_index_bits = 0;
    uint32_t no_of_sets = 0;

    no_of_sets = number_of_cache_blocks/associativity;

    g_cache_offset_bits = log(cache_block_size) / log(2);
    g_cache_index_bits = (int) (log(no_of_sets) / log(2));
    g_num_cache_tag_bits = 32 - g_cache_offset_bits - g_cache_index_bits;
    // while (i < no_of_sets) {
    //     while (j < associativity) {
    //        myCache.sets[i].blocks[j].valid = 0;
    //        myCache.sets[i].blocks[j].tag = 0;
    //        myCache.sets[i].blocks[j].block_age = 0;
    //        j++;
    //    }
    //    i++;
    // }

    int FIFO_index = 0;

    int random = 0;
    int counter = 0;

    int k = 0;

    Cache myCache;
    myCache.sets = (Set*) malloc(no_of_sets * sizeof(Set));

    for (k = 0; k < no_of_sets; k++) {
        myCache.sets[k].blocks = (Block*)malloc(sizeof(Block) * (associativity));
    }

    // dynamically allocate the size for a block in each set
    int i = 0;
    int j = 0;

    for (i = 0; i < no_of_sets; i++) {
        for (j = 0; j < associativity; j++) {
            myCache.sets[i].blocks[j].valid = 0;
            myCache.sets[i].blocks[j].tag = 0;
            myCache.sets[i].blocks[j].block_age = 0;
        }
    }

    int blocks_per_set = number_of_cache_blocks / no_of_sets;

    int hits = 0;
    int misses = 0;

    mem_access_t access;
    /* Loop until the whole trace file has been read. */
    while(1) {

        access = read_transaction(ptr_file);
        // If no transactions left, break out of loop.
        if (access.address == 0)
            break;
        
        

        /* Add your code here */
        uint32_t tag = access.address >> (g_cache_index_bits + g_cache_offset_bits);
        uint32_t index = ((1 << g_cache_index_bits) - 1) & access.address >> g_cache_offset_bits;
        //uint32_t index = access.address << g_num_cache_tag_bits >> (g_num_cache_tag_bits + g_cache_offset_bits);
        uint32_t offset = access.address << (g_num_cache_tag_bits + g_cache_index_bits);
        
       // printf("String is %s \n" , index);

      //  counter++;
        
        int i = 0;

        // go through all the blocks in the set
        while (i < blocks_per_set) {
            // event handler for when the block is empty
            
            if (myCache.sets[index].blocks[i].valid == 0) {
                
                myCache.sets[index].blocks[i].valid = 1;
                // fill the empty block with the address tag
                myCache.sets[index].blocks[i].tag = tag;
                myCache.sets[index].blocks[i].block_age = 0;
              //  g_result.cache_misses++;
                misses++;
                i++;
                break;
            } 
            // event handler for when the block is FULL
            // but it is a HIT
            else if (myCache.sets[index].blocks[i].tag == tag && myCache.sets[index].blocks[i].valid == 1) {
                myCache.sets[index].blocks[i].block_age = 0;
                int j = 0;
                while (j < blocks_per_set) {
                    if (i != j) {
                        myCache.sets[index].blocks[j].block_age++;
                        j++;
                    }
                    j++;
                }
                //g_result.cache_hits++;
                hits++;
                i++;
                break;
            }
        //     // even handler for when the block is FULL and NOT a hit
            else if (replacement_policy == Random && (blocks_per_set - 1) == i) {
                random = rand()%(blocks_per_set);
                myCache.sets[index].blocks[random].tag = tag;
                myCache.sets[index].blocks[random].valid = 1;
              //  myCache.sets[index].blocks[random].block_age = 0;
                //g_result.cache_misses++;
                misses++;
                i++;
                break;
            }

            else if (replacement_policy == LRU && (blocks_per_set - 1) == i) {
                int k = 0;
                int max = 0;
                int maxIndex = 0;
                while (k < blocks_per_set) {
                    if (myCache.sets[index].blocks[k].block_age > max) {
                        max = myCache.sets[index].blocks[k].block_age;
                        maxIndex = k;
                        k++;
                    }
                    k++;
                }

                //g_result.cache_misses++;
                misses++;

                myCache.sets[index].blocks[maxIndex].tag = tag;
                myCache.sets[index].blocks[maxIndex].valid = 1;
                myCache.sets[index].blocks[maxIndex].block_age = 0;
                while (j < blocks_per_set) {
                    if (i != j) {
                        myCache.sets[index].blocks[j].block_age++;
                        j++;
                    }
                    j++;
                }
                i++;
                break;
                
            }

            else if (replacement_policy == FIFO && (blocks_per_set - 1) == i) {
                FIFO_index = FIFO_index % blocks_per_set;
                myCache.sets[index].blocks[FIFO_index].tag = tag;
                myCache.sets[index].blocks[FIFO_index].valid = 1;
            //    myCache.sets[index].blocks[random].block_age = 0;
                //g_result.cache_misses++;
                misses++;
                FIFO_index++;
                i++;
                break;
            }

           i++;
        }

    }

    g_result.cache_hits = hits;
    g_result.cache_misses = misses;

    /* Do not modify code below. */
    /* Make sure that all the parameters are appropriately populated. */
    print_statistics(g_num_cache_tag_bits, g_cache_offset_bits, &g_result);

    /* Close the trace file. */
    fclose(ptr_file);
    return 0;
}
