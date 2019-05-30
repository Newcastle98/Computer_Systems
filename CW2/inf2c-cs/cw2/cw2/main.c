/***************************************************************************
 * *    Inf2C-CS Coursework 2: Cache Simulation
 * *
 * *    Instructor: Boris Grot
 * *
 * *    TA: Siavash Katebzadeh
 ***************************************************************************/

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
//global variables:

uint32_t count = 0;

//makes block_num blocks which are arrays of len 4 containg ints for [set_num,validity,tag,usecount/time of entering]
uint32_t** allocateCache(uint32_t associativity, uint32_t block_num){
    uint32_t **cache;
    cache = malloc(block_num*sizeof(uint32_t *));
    int i;
    for (i = 0; i< block_num;i++ ){
        *(cache+i) = malloc(4*sizeof(uint32_t));
        int k;
        for (k = 0; k<4; k++){
            if (k == 0){
                *(*(cache+i)+k)= i/associativity;
                //printf("[%d] ", *(*(cache+i)+k));
            }else {
                *(*(cache+i)+k)= 0;
                //printf("[%d] ", *(*(cache+i)+k));
            }
           // printf("[%d] ", *(*(cache+i)+k));
        }//printf("\n");
    }
    return cache;
}

void deallocateCache(uint32_t **cache,uint32_t block_num){
        int i = 0;
        for (i = block_num-1; i>=0 ;i-- ){
            free (*(cache+i));
        }
        free(cache);
}
//sices up the address into tag,index,offset bits;
//return structure:[uint32_t,uint32_t,uint32_t]
uint32_t* sliceaddress(uint32_t address,uint32_t g_num_cache_tag_bits,uint32_t index,uint32_t g_cache_offset_bits){
    uint32_t* addressparts;
    uint32_t* add;
    add = &address;
    addressparts = malloc(3*sizeof(uint32_t));
    *addressparts = *add >> (32-g_num_cache_tag_bits);
    *(addressparts+1) = *add << (g_num_cache_tag_bits);
    *(addressparts+1) = *(addressparts+1) >> (g_num_cache_tag_bits+g_cache_offset_bits);
    *(addressparts+2) = *add << (32-g_cache_offset_bits);
    *(addressparts+2) = *(addressparts+2) >> (32-g_cache_offset_bits);

    return addressparts;
}
void printCache(uint32_t **cache,uint32_t block_num){
    int i;
    for (i = 0; i< block_num;i++ ){
        int k;
        for (k = 0; k<4; k++){
                printf("[%u] ", *(*(cache+i)+k));
        }printf("\n");
    }
}
void printSet(uint32_t** cache,uint32_t set_jump){
    int i;
    for (i = 0 ;i<associativity;i++){
        int k;
        for(k = 0 ; k<4;k++){
            printf("[%u] ",*(*(cache+set_jump+i)+k));
        }printf("\n");
        
    }
}

int FIFOcache(uint32_t set_jump,uint32_t num_in_set, uint32_t** cache, uint32_t* addressparts){
    uint32_t num_of_block_in_set =set_jump+num_in_set;
    uint32_t validity = *(*(cache+num_of_block_in_set)+1);
    uint32_t tag_in_cache = *(*(cache+num_of_block_in_set)+2);
    uint32_t tag_of_curr_addr =*(addressparts);
    uint32_t* curr_block = *(cache+num_of_block_in_set);
    //options:
    //hit
    if (validity && (tag_in_cache == tag_of_curr_addr) ){
        //printf("hit\n");
        g_result.cache_hits += 1;
        return 1;
    }
    //filled, but not a match so move on doing nothing if its not the last bit yet
    else if(validity && (tag_in_cache != tag_of_curr_addr) && num_in_set!=(associativity-1)){
        //printf("pass");
        return 0;
    }
    //empty block and no hit before --> save here
    else if(!validity){
        //printf("free space\n");
        g_result.cache_misses += 1;
        *(curr_block+1) = 1;                 //change validity
        *(curr_block+2) = *(addressparts); //save tag
        *(curr_block+3) = count++;           //count for arrival
        return 1;
    }
    //end of set,all filled, but no match so it is a miss --> replace some memory
    else if(num_in_set==(associativity-1)){
        g_result.cache_misses += 1;
        //printf("replace\n");
        int z;
        uint32_t min = 0;
        //get min count value
        for (z = 0; z<associativity; z++){
            uint32_t count_of_block = *(*(cache+set_jump+z)+3);
            if (z==0){
                min =count_of_block;
                //printf("min0: %d\n",min);
            }
            if (min>count_of_block){
                min = count_of_block;
                //printf("min: %d\n",min);
            }
        }
        //replace block with minimum count
        for (z = 0; z<associativity; z++){
            uint32_t count_of_block = *(*(cache+set_jump+z)+3);
            //printf("min: %d\n",count_of_block);
            if (min==count_of_block){
                uint32_t* block_to_replace = *(cache+set_jump+z);
                *(block_to_replace+2) = *(addressparts); //replace tag
                *(block_to_replace+3) = count++;           //new count
                //printf("min: %d\n",*(curr_block+3));
                //printf("replace happened\n");
                return 1;
            }
            
        }
        return 1;
    }else {
        printf("I left out stg");
        return 0;
    }
}

void newOrder(uint32_t set_jump, uint32_t** cache,uint32_t* curr_block){
    int elem_in_set;
    if(*(curr_block+3)==1){
        //do nothing
    }else{
    for (elem_in_set = 0; elem_in_set<associativity; elem_in_set++){
        uint32_t* order_of_block = (*(cache+set_jump+elem_in_set)+3);
        //newly hit block gets order one
        if(curr_block==(*(cache+set_jump+elem_in_set))){
            *order_of_block =1;
        }else if(*order_of_block == 0){
            //pass
        }else{
            *order_of_block +=1;
        }
    }
    }
}
int LRUcache(uint32_t set_jump,uint32_t num_in_set, uint32_t** cache, uint32_t* addressparts){
    uint32_t num_of_block_in_set =set_jump+num_in_set;
    uint32_t validity = *(*(cache+num_of_block_in_set)+1);
    uint32_t tag_in_cache = *(*(cache+num_of_block_in_set)+2);
    uint32_t tag_of_curr_addr =*(addressparts);
    uint32_t* curr_block = *(cache+num_of_block_in_set);
    //options:
    //hit --> new ordering, hitcount+=1
    if (validity && (tag_in_cache == tag_of_curr_addr) ){
        //printf("hit\n");
        g_result.cache_hits += 1;
        newOrder(set_jump, cache, curr_block);
        return 1;
    }
    //filled, but not a match so move on doing nothing if its not the last bit yet
    else if(validity && (tag_in_cache != tag_of_curr_addr) && num_in_set!=(associativity-1)){
        //printf("pass");
        return 0;
    }
    //empty block and no hit before --> save here, new ordering
    else if(!validity){
        //printf("free space\n");
        g_result.cache_misses += 1;
        *(curr_block+1) = 1;                 //change validity
        *(curr_block+2) = *(addressparts); //save tag
        newOrder(set_jump, cache, curr_block);
        return 1;
    }
    //end of set,all filled, but no match so it is a miss --> replace some memory
    else if(num_in_set==(associativity-1)){
        g_result.cache_misses += 1;
        //printf("replace\n");
        int z;
        uint32_t max = 0;
        //get max count value
        for (z = 0; z<associativity; z++){
            uint32_t order_of_block = *(*(cache+set_jump+z)+3);
            if (z==0){
                max =order_of_block;
            }
            if (max<order_of_block){
                max = order_of_block;
            }
        }
        //replace block with minimum count
        for (z = 0; z<associativity; z++){
            uint32_t order_of_block = *(*(cache+set_jump+z)+3);
            if (max==order_of_block){
                uint32_t* block_to_replace = *(cache+set_jump+z);
                *(block_to_replace+2) = *(addressparts); //replace tag
                newOrder(set_jump, cache, curr_block);   //new count and order
                return 1;
            }
            
        }
        return 1;
    }else {
        printf("I left out stg");
        return 0;
    }
}

int RANDOMcache(uint32_t set_jump,uint32_t num_in_set, uint32_t** cache, uint32_t* addressparts){
    uint32_t num_of_block_in_set =set_jump+num_in_set;
    uint32_t validity = *(*(cache+num_of_block_in_set)+1);
    uint32_t tag_in_cache = *(*(cache+num_of_block_in_set)+2);
    uint32_t tag_of_curr_addr =*(addressparts);
    uint32_t* curr_block = *(cache+num_of_block_in_set);
    //options:
    //hit
    if (validity && (tag_in_cache == tag_of_curr_addr) ){
        printf("hit\n");
        g_result.cache_hits += 1;
        return 1;
    }
    //filled, but not a match so move on doing nothing if its not the last bit yet
    else if(validity && (tag_in_cache != tag_of_curr_addr) && num_in_set!=(associativity-1)){
        printf("pass");
        return 0;
    }
    //empty block and no hit before --> save here
    else if(!validity){
        printf("free space\n");
        g_result.cache_misses += 1;
        *(curr_block+1) = 1;                 //change validity
        *(curr_block+2) = *(addressparts); //save tag
        return 1;
    }
    //end of set,all filled, but no match so it is a miss --> replace some memory
    else if(num_in_set==(associativity-1)){
        g_result.cache_misses += 1;
        printf("replace\n");
        uint32_t z;
        uint32_t randMod = rand()%associativity;
        //replace random block
        for (z = 0; z<associativity; z++){
            if (randMod==z){
                uint32_t* block_to_replace = *(cache+set_jump+z);
                *(block_to_replace+2) = *(addressparts); //replace tag
                //printf("replace happened\n");
                return 1;
            }
        }
        return 1;
    }else {
        printf("I left out stg");
        return 0;
    }
}
//chooses mode to use,
int chooseCacheMode(replacement_p replacement_policy, uint32_t set_jump,uint32_t num_in_set, uint32_t** cache, uint32_t* addressparts){
    if (replacement_policy == Random){
        return RANDOMcache(set_jump,num_in_set,cache,addressparts);
    }else if (replacement_policy == LRU){
        return LRUcache(set_jump,num_in_set,cache,addressparts);
    }else {
        return FIFOcache(set_jump,num_in_set,cache,addressparts);
    }
}

int main(int argc, char** argv) {
    time_t t;
    /* Intializes random number generator */
    /* Important: *DO NOT* call this function anywhere else. */
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
    //cache structure:
    uint32_t index = log2(number_of_cache_blocks/associativity);
    g_cache_offset_bits = log2(cache_block_size);
    g_num_cache_tag_bits = 32-g_cache_offset_bits-index;
    
    //allocate cache (block_num *
    uint32_t** cache = allocateCache(associativity, number_of_cache_blocks);
    
    
    
    

    mem_access_t access;
    /* Loop until the whole trace file has been read. */
    //reset count before every loop
    count = 0;
    while(1) {
        //printf("%d", count);
        access = read_transaction(ptr_file);
        // If no transactions left, break out of loop.
        if (access.address == 0){
            break;
        /* Add your code here */
        }
        else {
            uint32_t* addressparts;
            //addressparts = [tag, index(=set), offset]
            addressparts = sliceaddress(access.address,g_num_cache_tag_bits,index, g_cache_offset_bits);
            
            uint32_t setnum =*(*(cache+number_of_cache_blocks-1))+1;
            int set;
            //search for set, than check filled(validity != 0) blocks for hit, if none, save/replace data according to the given policy
            for (set = 0; set<setnum;set++){
                uint32_t set_jump =set*associativity;
                if (*(*(cache+set_jump))==*(addressparts+1)){
                    uint32_t num_in_set;
                    for (num_in_set=0; num_in_set<associativity; num_in_set++){
                        
                        uint32_t act = chooseCacheMode(replacement_policy,set_jump,num_in_set,cache,addressparts);
                        if (act){
                            //printSet(cache,set_jump);
                            break;
                        }
                    }
                    break; //we found and used the set, loop should stop
                }
            }
            /*}
             //FIFOcache(set_jump,num_in_set,cache,addressparts);

            else if (replacement_policy == LRU){
                for (set = 0; set<setnum;set++){
                    uint32_t set_jump =set*associativity;
                    if (*(*(cache+set_jump))==*(addressparts+1)){
                        uint32_t num_in_set;
                        for (num_in_set=0; num_in_set<associativity; num_in_set++){
                            uint32_t act = LRUcache(set_jump,num_in_set,cache,addressparts);
                            if (act){
                                break;
                            }
                        }
                        break; //we found and used the set, loop should stop
                    }
                }
            }
            else if (replacement_policy == Random){
                for (set = 0; set<setnum;set++){
                    uint32_t set_jump =set*associativity;
                    if (*(*(cache+set_jump))==*(addressparts+1)){
                        uint32_t num_in_set;
                        for (num_in_set=0; num_in_set<associativity; num_in_set++){
                            uint32_t act = RANDOMcache(set_jump,num_in_set,cache,addressparts);
                            if (act){
                                break;
                            }
                        }
                        break; //we found and used the set, loop should stop
                    }
                }
                
            }
             */
            //printf("[%u;%d;%d]",*(addressparts+0),*(addressparts+1),*(addressparts+2));
            //printf("address: %u\n",access.address);
        }
    }
    printCache(cache,number_of_cache_blocks);
    printf("rand num: %d",rand());


    
    /* Do not modify code below. */
    /* Make sure that all the parameters are appropriately populated. */
    print_statistics(g_num_cache_tag_bits, g_cache_offset_bits, &g_result);
    
    /* Close the trace file. */
    fclose(ptr_file);
    //mycode
    deallocateCache(cache, number_of_cache_blocks);
    //end of my code
    return 0;
}
