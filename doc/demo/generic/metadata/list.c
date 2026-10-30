#include <stdlib.h> /* malloc(3), NULL */
#include <stddef.h> /* ptrdiff_t */
#include <assert.h>

typedef struct list_header_t {
  struct list_header_t *next;
} list_header_t;

#define LIST_HEADER( list ) ((list_header_t*)(list) - 1)

#define NIL NULL
void *list_new( void ) { return NIL; }
#undef  NIL

int   list_is_empty( void *list ) { return list_new() == list; }

void  list_prepend_link( ptrdiff_t value_size, void **list ) {
  list_header_t *header;

  assert( value_size > 0 && "Value size is negative!" );

  header = malloc( sizeof *header + value_size );
  assert( header && "Allocation failed!" );

  header->next = LIST_HEADER( *list );

  *list = header+1;

  return;
}

void *list_next( void *list ) {
  list_header_t *next;

  assert( !list_is_empty( list ) && "List is empty!" );

  next = LIST_HEADER( list )->next;

  return next + ( list_is_empty( next ) ? 0 : 1 );
}

void *list_delete( void *list, void (*destructor)( void* ) ) {
  while ( !list_is_empty( list ) ) {
    void *next = list_next( list );
    if ( destructor ) destructor( list );
    free( LIST_HEADER( list ) );
    list = next;
  }

  return list;
}

#undef LIST_HEADER
