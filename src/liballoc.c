#ifdef _POSIX_C_SOURCE
#undef _POSIX_C_SOURCE
#endif

#ifdef _XOPEN_SOURCE
#undef _XOPEN_SOURCE
#endif

#define _POSIX_C_SOURCE 200819L
#define _XOPEN_SOURCE	700
#define _DEFAULT_SOURCE

#include <unistd.h>
#include <pthread.h>

#include <stdlib.h>
#include <stddef.h>
#include <stdalign.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "liballoc.h"

#define MALLOC_LOCK()               (pthread_mutex_lock (&global_malloc_lock))
#define MALLOC_UNLOCK()             (pthread_mutex_unlock (&global_malloc_lock))
#define SBRK_FAILED                 ((void *) -1)

/* The C Standard states that the pointer returned if the allocation succeeds is 
 * suitably aligned so that it may be assigned to a pointer to any type of object
 * with a fundamental alignment requirement. 
 */
typedef struct header {
    _Alignas (max_align_t) struct header *next;
    size_t size;
    bool is_free;
} header_t;

static header_t *head, *tail;
static pthread_mutex_t global_malloc_lock;

static header_t *get_free_block(size_t size)
{
    /*
     * Ascertain if there's a free block that can accommodate requested size. 
     */
    for (header_t *curr = head; curr; curr = curr->next) {
        if (curr->is_free && curr->size >= size) {
            return curr;
        }
    }
    return NULL;
}

void *malloc(size_t size)
{
 
    /* The C Standard states that if the size of the space requested is zero, 
     * the behavior is implementation-defined: either a null pointer is returned
     * to indicate an error, or the behavior is as if the size were some non zero
     * value, except that the returned pointer shall not be used to access an object.
     * We shall opt for the former. 
     */
    if (!size) {
        return NULL;
    }

    /* The size requested needs to be a multiple of alignof (max_align_t). */
    size_t const padding = size % alignof (max_align_t);

    if (padding) {
        size += alignof (max_align_t) - padding;
    }

    if (size > SIZE_MAX - sizeof (header_t)) {  /* Prevent overflow. */
        return NULL;
    }
    
    MALLOC_LOCK();
    header_t *header = get_free_block(size);

    if (header) {
        /* We have an apt free block. */
        header->is_free = false;
        MALLOC_UNLOCK();
        return header + 1;
    }
    intptr_t const total_size = (intptr_t) (sizeof (header_t) + size);
    void *const chunk = sbrk(total_size);

    if (chunk == SBRK_FAILED) {
        MALLOC_UNLOCK();
        return NULL;
    }

    header = chunk;
    header->size = size;
    header->is_free = false;
    header->next = NULL;

    if (!head) {
        head = header;
    }

    if (tail) {
        tail->next = header;
    }

    tail = header;
    MALLOC_UNLOCK();
    return header + 1;
}

void *calloc(size_t nmemb, size_t size)
{
    if (!nmemb || !size) {
        return NULL;
    }

    if (size >= SIZE_MAX / nmemb) {     /* Allocation too big. */
        return NULL;
    }

    size *= nmemb;
    void *const chunk = malloc(size);

    if (chunk) {
        memset(chunk, 0, size);
    }

    return chunk;
}

void *realloc(void *ptr, size_t size)
{
    if (!ptr || !size) {
        return malloc(size);
    }

    const header_t *const header = (header_t *) ptr - 1;

    if (header->size >= size) {
        return ptr;
    }

    void *const ret = malloc(size);

    if (ret) {
        /* Relocate the contents to the new chunkier chunk and 
         * free the old chunk.
         */
        memcpy(ret, ptr, header->size);
        free(ptr);
    }

    return ret;
}

void free(void *ptr)
{
    if (!ptr) {
        return;
    }

    MALLOC_LOCK();
    header_t *const header = (header_t *) ptr - 1;

    /* Get a pointer to the current program break address. */
    const void *const prog_brk = sbrk(0);

    /* Ascertain whether the block to be freed is the last one in the
     * list. If so, shrink the size of the heap and release the memory
     * to the OS. Else, keep the block but mark it as free for use. 
     */
    if ((char *) ptr + header->size == prog_brk) {
        if (head == tail) {
            head = tail = NULL;
        } else {
            for (header_t *tmp = head; tmp; tmp = tmp->next) {
                if (tmp->next == tail) {
                    tmp->next = NULL;
                    tail = tmp;
                }
            }
        }

        /* sbrk() with a negative argument decrements the program break,
         * which releases memory the memory back to the OS.
         */
        sbrk((intptr_t) (0 - sizeof (header_t) - header->size));

        /* This lock does not ensure thread-safety, for sbrk() itself
         * is not thread-safe.
         */
        MALLOC_UNLOCK();
        return;
    }

    header->is_free = true;
    MALLOC_UNLOCK();
}
