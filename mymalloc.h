#include <inttypes.h>
#include <stddef.h>

#define HEAP_SIZE 1024

typedef enum {
    BEST_FIT,
    NEXT_FIT,
    FIRST_FIT,
    WORST_FIT
} Strategy;

typedef enum {
    ADDR_ORDERED_LIST,
    UNORDERED_LIST
} ListType;

extern Strategy strategy;
extern ListType listtype;

typedef struct  __attribute__((aligned(16))) tag {
    uint64_t size;
    uint32_t isfree;
    uint32_t padding;
} Tag;

typedef struct  __attribute__((aligned(16))) block {
    struct block *next;
    struct block *prev;
    Tag info;
    char data[];
} Block;

extern Block *free_list;
extern Block *heap_start;
extern Block *heap_end;
extern Block *last_freed;

void *mymalloc(size_t size);
void myfree(void *p);

Block *split_block(Block *b, size_t size);
Block *left_coalesce(Block *b);
Block *right_coalesce(Block *b);

Block *next_block_in_freelist(Block *b);
Block *next_block_in_addr(Block *b);
Block *prev_block_in_freelist(Block *b);
Block *prev_block_in_addr(Block *b);

uint64_t numberof16blocks(size_t size_inbytes);
void printheap();
ListType getlisttype();
int setlisttype(ListType listtype);
Strategy getstrategy();
int setstrategy(Strategy strategy);
