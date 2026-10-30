#include "list.h"

#include <stdlib.h> /* malloc(3), NULL */
#include <assert.h>

struct link_t {
  struct link_t *next;
  void         *datum;
};

#define NIL NULL
list_t list_new( void ) { return NIL; }
#undef NIL

int    list_is_empty( list_t list ) { return list_new() == list; }

void  *list_first( list_t list ) {
  assert( list && "List is empty!");

  return list->datum;
}

list_t list_next( list_t list ) {
  assert( list && "List is empty!");

  return list->next;
}

list_t list_prepend( void *value, list_t list ) {
  list_t new_head = malloc( sizeof *new_head );
  assert( new_head && "Allocation failed!" );

  new_head->datum = value;
  new_head->next  = list;

  return new_head;
}

list_t list_delete( list_t list, void (*destructor)( void* ) ) {
  while ( !list_is_empty( list ) ) {
    list_t next = list_next( list );
    if ( destructor ) destructor( list_first( list ) );
    free( list );
    list = next;
  }

  return list;
}
