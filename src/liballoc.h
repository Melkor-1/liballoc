#ifndef LIBALLOC_H
#define LIBALLOC_H

#include <stddef.h>

/** 
*   \brief      The **malloc** function allocates space for an object whose size
*               is specified by *size* and whose value is indeterminate.
*   \param      size -  The size of the object to be allocated.
*   \return     The **malloc** function returns a pointer to the allocated space
*               or a NULL pointer on failure. Requesting zero bytes always fails.
*/
void *malloc(size_t size);

/**
*   \brief      The **calloc** function allocates space for an array of *nmemb*
*               objects, each of whose size is *size*. The space is initialized
*               to all bits zero.      
*   \param      nmemb - The number of objects.
*   \param      size  - The size of each object.
*   \return     The **calloc** function returns a pointer to the allocated space
*               or a NULL pointer on failure.  Requesting zero bytes always fails.
*/
void *calloc(size_t nmemb, size_t size);

/**
*   \brief      The **realloc** function deallocates the old object pointed to
*               by *ptr* and returns a pointer to a new object that has the 
*               size specified by *size*. The contents of the new object shall
*               be the same as that of the old object prior to deallocation,
*               upto the lesser of the new and old sizes.  Any bytes in the new
*               object beyond the size of the old object have indeterminate
*               values.
*
*       If *ptr* is a null pointer,the **realloc** function behaves like the
*       **malloc** function for the specified *size*.  Otherwise, if *ptr* does
*       not match a pointer earlier returned by a memory management function, or
*       if the space has been deallocated by a call to the **free** function,
*       the behavior is undefined.  If *size* is nonzero and memory for the new
*       object is not allocated, the old object is not deallocated.  If *size*
*       is zero, a NULL pointer is returned to indicate failure, and the old
*       object is not deallocated, and its value shall be unchanged.
*
*   \param      ptr   - A pointer to the object to be reallocated.
*   \param      size  - The new size.
*   \return     The **realloc** function returns a pointer to the new object, or
*               a null pointer if the new object has not been allocated. 
*               Requesting zero bytes always fails.
*/
void *realloc(void *ptr, size_t size);

/**
*   \brief      The **free** function causes the space pointed to by *ptr* to be
*               deallocated, that is, made unavailable for further allocation. 
*               If *ptr* is a null pointer, no action occurs. Otherwise, if the
*               argument does not match a pointer earlier returned by a memory 
*               management function, or if the space has been deallocated by a
*               call to **free**, the behaviour is undefined.
*   \param      ptr - A pointer to the object to be deallocated.
*   \return     The **free** function returns no value.
*/
void free(void *ptr);

#endif      /* LIBALLOC_H */
