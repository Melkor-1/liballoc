#define _DEFAULT_SOURCE

#include <unistd.h>
#include <pthread.h>

#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "liballoc.h"

#define ALIGN 16
#define MALLOC_LOCK()               (pthread_mutex_lock (&global_malloc_lock))
#define MALLOC_UNLOCK()             (pthread_mutex_unlock (&global_malloc_lock))
#define CHUNK_SIZE(chunk)           ((chunk)->s.size)
#define NEXT_CHUNK(chunk)           ((chunk)->s.next)
#define IS_CHUNK_FREE(chunk)        ((chunk)->s.is_free)
#define MARK_CHUNK_FREE(chunk)      ((chunk)->s.is_free = true)
#define MARK_CHUNK_NONFREE(chunk)   ((chunk)->s.is_free = false)
#define SET_CHUNK_SIZE(chunk, size) ((chunk)->s.size = (size)) 
#define SET_NEXT_CHUNK(chunk, val)  ((chunk)->s.next = (val))

union header {
    struct {
        size_t size;
        bool is_free;
        union header *next;
    } s;
    /* Force the header to be aligned to 16 bytes. */
    char stub[ALIGN];
};

typedef union header header_t;

static header_t *head, *tail;
static pthread_mutex_t global_malloc_lock;

static header_t *get_free_block (size_t size)
{
    /* Ascertain if there's a free block that can accomodate requested size. */
    for (header_t *curr = head; curr; curr = NEXT_CHUNK(curr)) {
        if (IS_CHUNK_FREE(curr) && CHUNK_SIZE(curr) >= size) {
            return curr;
        }
    }
    return NULL;
}

void *malloc (size_t size)
{
    /*
     * The C Standard states that if the size of the space requested is zero, 
     * the behavior is implementation-defined: either a null pointer is returned
     * to indicate an error, or the behavior is as if the size were some non zero
     * value, except that the returned pointer shall not be used to access an object.
     * We shall opt for the former.
     */
     if (!size) {
        return NULL;
    }
    
    MALLOC_LOCK();
    header_t *header = get_free_block (size);

    if (header) {
        /* We have an apt free block. */
        MARK_CHUNK_NONFREE (header);
        MALLOC_UNLOCK();
        return header + 1;
    }

    intptr_t const total_size = (intptr_t) (sizeof (header_t) + size);
    void *const chunk = sbrk (total_size);
    
    if (chunk == (void *) -1) {
        MALLOC_UNLOCK();
        return NULL;
    }

    header = chunk;
    SET_CHUNK_SIZE (header, size);
    MARK_CHUNK_NONFREE (header);
    SET_NEXT_CHUNK (header, NULL);

    if (!head) {
        head = header;
    } 

    if (tail) {
        SET_NEXT_CHUNK (tail, header);
    }

    tail = header;
    MALLOC_UNLOCK();
    return header + 1;
}
    
void *calloc (size_t nmemb, size_t size)
{
    if (!nmemb || !size) {
        return NULL;
    }
#if 0
    Write a header file that will see if we are using C2X and define 
    CKD_ADD(), CKD_SUB(), and CKD_MUL() macros to ckd_add(), ckd_sub(),
    and ckd_mul() standard functions.
    If we are not using C2X and __clang__ or __GNUC__ is defined, then 
    define CKD_ADD, CKD_SUB, and CKD_MUL to __builtin_mul_overflow(),
    __builtin_sub_overflow, and __builtin_mul_overflow. 
    If neither C2X, nor clang, or gcc can be detected, define them to:
#endif

    if (size > SIZE_MAX / nmemb) { /* nmemb * size would wrap-around. */
        return NULL;
    }
    
    size_t const total_size = nmemb * size;
    void *chunk = malloc (total_size);

    if (!chunk) {
        return NULL;
    }

    memset (chunk, 0X00, total_size);
    return chunk;
}

void *realloc (void *ptr, size_t size)
{
    if (!ptr || !size) {
        return malloc (size);
    }

    const header_t *const header = (header_t *) ptr - 1;
    
    if (CHUNK_SIZE (header) >= size) {
        return ptr;
    }

    void *const ret = malloc (size);

    if (ret) {
        /* Relocate the contents to the new chunkier chunk and 
         * free the old chunk.
         */
        memcpy (ret, ptr, CHUNK_SIZE (header));
        free (ptr);
    }

    return ret;
}

void free (void *ptr)
{
    if (!ptr) {
        return;
    }

    MALLOC_LOCK();
    header_t *const header = (header_t *) ptr - 1;

    /* Get a pointer to the current program break address. 
     */
    const void *const prog_brk = sbrk (0);
    
    /* Ascertain whether the block to be freed is the last one in the
     * list. If so, shrink the size of the heap and release the memory
     * to the OS. Else, keep the block but mark it as free for use. 
     */
    if ((char *) ptr + CHUNK_SIZE (header) == prog_brk) {
        if (head == tail) {
            head = tail = NULL;
        } else {
            for (header_t *tmp = head; tmp; tmp = NEXT_CHUNK (tmp)) {
                SET_NEXT_CHUNK (tmp, NULL);
                tail = tmp;
            }
        }

        /* sbrk() with a negative argument decrements the program break,
         * which releases memory the memory back to the OS.
         */
        sbrk ((intptr_t) (0 - sizeof (header_t) - CHUNK_SIZE (header)));
        
        /* This lock does not ensure thread-safety, for sbrk() itself
         * is not thread-safe.
         */
        MALLOC_UNLOCK();
        return;
    }

    MARK_CHUNK_FREE (header);
    MALLOC_UNLOCK();
}

