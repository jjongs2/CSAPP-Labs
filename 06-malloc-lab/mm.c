#include "mm.h"

#include <stdint.h>
#include <string.h>

#include "memlib.h"

#define DEBUG 0

#if (DEBUG > 0)
#define CHECK_HEAP() mm_check()
#else
#define CHECK_HEAP()
#endif

#if (DEBUG > 1)
#define LOG(...) fprintf(stderr, __VA_ARGS__)
#else
#define LOG(...)
#endif

#define F_ALLOC 1
#define F_PREV_ALLOC 2

#define INVALID_PTR ((void *)-1)
#define WSIZE 8
#define LIST_COUNT 16
#define ALIGNMENT 32
#define CHUNK_SIZE ((LIST_COUNT - 1) * ALIGNMENT)
#define MINI_CHUNK_SIZE (6 * ALIGNMENT)
#define FLAG_MASK (ALIGNMENT - 1)
#define SIZE_MASK (~FLAG_MASK)

#define MAX(x, y) ((x) < (y) ? (y) : (x))
#define MIN(x, y) ((y) < (x) ? (y) : (x))
#define ALIGN(size) ((size + FLAG_MASK) & SIZE_MASK)
#define PACK(size, flags) ((size) | (flags))
#define GET(p) (*(uint64_t *)(p))
#define PUT(p, val) (*(uint64_t *)(p) = (val))
#define GET_SIZE(p) (GET(p) & SIZE_MASK)
#define GET_FLAGS(p) (GET(p) & FLAG_MASK)
#define GET_ALLOC(p) (GET(p) & F_ALLOC)
#define GET_PREV_ALLOC(p) (GET(p) & F_PREV_ALLOC)
#define HDRP(bp) (bp)
#define PLDP(bp) ((char *)bp + WSIZE)
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - WSIZE)
#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)))
#define PREV_BLKP(bp) ((char *)(bp) - GET_SIZE((char *)bp - WSIZE))

typedef struct s_node {
    uint64_t header;
    struct s_node *prev;
    struct s_node *next;
} t_node;

team_t team;
static t_node *free_lists;
static char *virtual_brk;

int mm_check(void);
static void *adjust_heap(ssize_t size);
static void *coalesce(void *bp);
static void *find_fit(size_t asize);
static size_t get_list_index(size_t size);
static void place(void *bp, size_t asize);
static void prepend_node(t_node *bp);
static void remove_node(t_node *bp);

int mm_init(void) {
    size_t i;

    free_lists = mem_sbrk(LIST_COUNT * sizeof(t_node));
    if (free_lists == INVALID_PTR)
        return -1;
    for (i = 0; i < LIST_COUNT; ++i) {
        free_lists[i].header = 0;
        free_lists[i].prev = &free_lists[i];
        free_lists[i].next = &free_lists[i];
    }
    virtual_brk = (char *)mem_heap_hi() + 1;
    LOG("mm_init(): Heap initialized\n");
    CHECK_HEAP();
    return 0;
}

void *mm_malloc(size_t size) {
    size_t asize = ALIGN(WSIZE + size);
    size_t extend_size;
    void *bp;

    if (!free_lists)
        mm_init();
    if (size == 0)
        return NULL;
    bp = find_fit(asize);
    if (bp) {
        remove_node(bp);
        place(bp, asize);
        LOG("mm_malloc(): Allocate %zu bytes at %p\n", size, PLDP(bp));
        CHECK_HEAP();
        return PLDP(bp);
    }
    extend_size =
        (asize == ALIGNMENT) ? MINI_CHUNK_SIZE : MAX(asize, CHUNK_SIZE);
    bp = adjust_heap(extend_size);
    if (bp == INVALID_PTR)
        return NULL;
    PUT(HDRP(bp), PACK(extend_size, F_ALLOC | F_PREV_ALLOC));
    if (asize <= CHUNK_SIZE)
        place(bp, asize);
    LOG("mm_malloc(): Extend heap and allocate %zu bytes at %p\n", size,
        PLDP(bp));
    CHECK_HEAP();
    return PLDP(bp);
}

void mm_free(void *ptr) {
    void *bp;

    if (!ptr || !free_lists)
        return;
    bp = coalesce((char *)ptr - WSIZE);
    if (bp)
        prepend_node(bp);
    LOG("mm_free(): Free memory at %p\n", ptr);
    CHECK_HEAP();
    return;
}

void *mm_realloc(void *ptr, size_t size) {
    size_t asize = ALIGN(WSIZE + size);
    size_t old_size;
    void *bp;

    if (size == 0) {
        mm_free(ptr);
        return NULL;
    }
    if (!ptr)
        return mm_malloc(size);
    bp = (char *)ptr - WSIZE;
    old_size = GET_SIZE(HDRP(bp));
    if (NEXT_BLKP(bp) == virtual_brk) {
        adjust_heap(asize - old_size);
        PUT(HDRP(bp), PACK(asize, GET_FLAGS(HDRP(bp))));
        LOG("mm_realloc(): Extend existing block to %zu bytes at %p\n", size,
            ptr);
        CHECK_HEAP();
        return ptr;
    }
    bp = mm_malloc(size);
    if (!bp)
        return NULL;
    old_size = MIN(asize - WSIZE, size);
    memcpy(bp, ptr, old_size);
    mm_free(ptr);
    LOG("mm_realloc(): Reallocate from %p to %p with size %zu\n", ptr, bp,
        size);
    CHECK_HEAP();
    return bp;
}

static void *adjust_heap(ssize_t size) {
    void *old_vbrk = virtual_brk;
    intptr_t increment;

    virtual_brk += size;
    increment = MAX(virtual_brk - 1 - (char *)mem_heap_hi(), 0);
    if (mem_sbrk(increment) == INVALID_PTR)
        return INVALID_PTR;
    return old_vbrk;
}

static void *coalesce(void *bp) {
    void *next_bp = NEXT_BLKP(bp);
    size_t next_alloc = GET_ALLOC(HDRP(next_bp));
    size_t prev_alloc = GET_PREV_ALLOC(HDRP(bp));
    size_t size = GET_SIZE(HDRP(bp));

    if (!prev_alloc) {
        bp = PREV_BLKP(bp);
        remove_node(bp);
        size += GET_SIZE(HDRP(bp));
    }
    if (next_bp == virtual_brk) {
        adjust_heap(-size);
        return NULL;
    }
    if (!next_alloc) {
        remove_node(next_bp);
        size += GET_SIZE(HDRP(next_bp));
    } else {
        PUT(HDRP(next_bp), GET(HDRP(next_bp)) & ~F_PREV_ALLOC);
    }
    PUT(HDRP(bp), PACK(size, GET_PREV_ALLOC(HDRP(bp))));
    PUT(FTRP(bp), GET(HDRP(bp)));
    return bp;
}

static void *find_fit(size_t asize) {
    t_node *dummy, *free_bp;
    size_t i;

    for (i = get_list_index(asize); i < LIST_COUNT; ++i) {
        dummy = &free_lists[i];
        free_bp = dummy->prev;
        while (free_bp != dummy) {
            if (asize <= GET_SIZE(HDRP(free_bp)))
                return free_bp;
            free_bp = free_bp->prev;
        }
    }
    return NULL;
}

static size_t get_list_index(size_t size) {
    if (size > CHUNK_SIZE)
        return LIST_COUNT - 1;
    return size >> 5;
}

static void place(void *bp, size_t asize) {
    size_t csize = GET_SIZE(HDRP(bp));
    size_t rest_size = csize - asize;
    void *rest_bp = (char *)bp + asize;
    char *next_bp = NEXT_BLKP(bp);

    if (rest_size <= ALIGNMENT) {
        PUT(HDRP(bp), PACK(csize, F_ALLOC | F_PREV_ALLOC));
        PUT(HDRP(next_bp), GET(HDRP(next_bp)) | F_PREV_ALLOC);
        return;
    }
    PUT(HDRP(bp), PACK(asize, F_ALLOC | F_PREV_ALLOC));
    PUT(HDRP(rest_bp), PACK(rest_size, F_PREV_ALLOC));
    PUT(FTRP(rest_bp), GET(HDRP(rest_bp)));
    prepend_node(rest_bp);
}

static void prepend_node(t_node *free_bp) {
    size_t size = GET_SIZE(HDRP(free_bp));
    t_node *dummy = &free_lists[get_list_index(size)];

    free_bp->prev = dummy;
    free_bp->next = dummy->next;
    dummy->next->prev = free_bp;
    dummy->next = free_bp;
}

static void remove_node(t_node *free_bp) {
    free_bp->prev->next = free_bp->next;
    free_bp->next->prev = free_bp->prev;
}

int mm_check(void) {
    char *heap_start = mem_heap_lo();
    char *heap_end = mem_heap_hi();
    int error_count = 0;
    char *bp, *next_bp;
    t_node *dummy, *free_bp;
    size_t i;

    for (bp = heap_start; GET_SIZE(HDRP(bp)) > 0; bp = next_bp) {
        next_bp = NEXT_BLKP(bp);
        if (bp < heap_start || bp > heap_end ||
            GET_SIZE(HDRP(bp)) % ALIGNMENT != 0) {
            fprintf(stderr, "Error: Invalid block at %p (size: %llu)\n", bp,
                    GET_SIZE(HDRP(bp)));
            error_count += 1;
        }
        if (!GET_ALLOC(HDRP(bp))) {
            if (!GET_ALLOC(HDRP(next_bp))) {
                fprintf(stderr, "Error: Contiguous free blocks at %p and %p\n",
                        bp, next_bp);
                error_count += 1;
            }
            if (GET(HDRP(bp)) != GET(FTRP(bp))) {
                fprintf(stderr,
                        "Error: Header/footer mismatch for free block at %p\n",
                        bp);
                error_count += 1;
            }
        }
        if (bp != heap_start &&
            GET_ALLOC(HDRP(PREV_BLKP(bp))) != GET_PREV_ALLOC(HDRP(bp))) {
            fprintf(stderr,
                    "Error: PREV_ALLOC flag inconsistency at block %p\n", bp);
            error_count += 1;
        }
        if (next_bp < heap_end && next_bp != bp + GET_SIZE(HDRP(bp))) {
            fprintf(stderr, "Error: Block overlap detected at %p\n", bp);
            error_count += 1;
        }
    }
    for (i = 0; i < LIST_COUNT; ++i) {
        free_bp = dummy = &free_lists[i];
        while (free_bp->next != dummy) {
            free_bp = free_bp->next;
            if (GET_ALLOC(HDRP(free_bp)) || (char *)free_bp < heap_start ||
                (char *)free_bp > heap_end) {
                fprintf(stderr,
                        "Error: Invalid free block %p in free list %zu\n",
                        free_bp, i);
                error_count += 1;
                continue;
            }
            if (get_list_index(GET_SIZE(HDRP(free_bp))) != i) {
                fprintf(stderr,
                        "Error: Free block %p in wrong list (size: %llu, list: "
                        "%zu)\n",
                        free_bp, GET_SIZE(HDRP(free_bp)), i);
                error_count += 1;
            }
            if (free_bp->prev->next != free_bp ||
                free_bp->next->prev != free_bp) {
                fprintf(stderr,
                        "Error: Broken links for free block %p in list %zu\n",
                        free_bp, i);
                error_count += 1;
            }
        }
    }
    if (error_count > 0) {
        fprintf(stderr, "Heap consistency check failed with %d errors.\n",
                error_count);
        return 1;
    }
    return 0;
}
