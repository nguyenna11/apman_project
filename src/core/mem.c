#include <core/mem.h>
#include <common/logging.h>

#include <assert.h>
#include <stdlib.h>

void   *alloc__(size_t size, allocator_t *allocator
#if !defined(NDEBUG) && defined(DEBUG_MEM)
                , const char *func, int line
#endif
                ) {
  allocator_t **ptr = allocator ?
    allocator->allocate(size+sizeof(ptr), allocator->state)
    : calloc(1, size+sizeof(*ptr));
  assert(ptr && "Alloc failed!"); /* Univ. precond. */
  if (!ptr) {
    ERROR("Allocation failed\n");
    abort();
  }
  *ptr = allocator;

#if !defined(NDEBUG) && defined(DEBUG_MEM)
  fprintf(stderr, "[%16s:%d]  alloc %zu bytes at %p\n", func, line, size, ptr+1);
#endif

  return ptr+1;
}

void    dealloc__(void **objp
#if !defined(NDEBUG) && defined(DEBUG_MEM)
                , const char *func, int line
#endif
                ) {
  if (objp && *objp) {
    allocator_t **allocatorp = ((allocator_t**)*objp)-1;
#if !defined(NDEBUG) && defined(DEBUG_MEM)
    fprintf(stderr, "[%16s:%d]  dealloc at %p\n", func, line, *objp);
#endif

    if (*allocatorp)
      (*allocatorp)->deallocate(allocatorp, (*allocatorp)->state);
    else free(allocatorp);
    *objp = NULL;
  }
}
