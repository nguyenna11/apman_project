#ifndef datum_t
#error "Undefined datum type!"
#endif

#ifndef tag
#error "Undefined tag!"
#endif

#include "generics.h"

#define lst           CAT(tag, CAT(_,datum_t))
#define T             CAT(lst, _t)
#define link_t        CAT(link, CAT(_, CAT(datum_t, _t)))
#define list_new      CAT(lst, _new)
#define list_is_empty CAT(lst, _is_empty)
#define list_first    CAT(lst, _first)
#define list_next     CAT(lst, _next)
#define list_prepend  CAT(lst, _prepend)
#define list_delete   CAT(lst, _delete)

#include <stdlib.h> /* malloc(3), NULL */
#include <assert.h>

struct link_t {
  struct link_t   *next;
  datum_t          datum;
};

typedef struct link_t *T;

#define NIL NULL
T       list_new( void ) { return NIL; }
#undef NIL

int     list_is_empty( T list ) {
  return list_new() == list;
}

datum_t list_first( T list ) {
  assert( !list_is_empty( list ) && "List is empty!" );

  return list->datum;
}

T       list_next( T list ) {
  assert( !list_is_empty( list ) && "List is empty!" );

  return list->next;
}

T       list_prepend( datum_t value, T list ) {
  T new_head = malloc( sizeof *new_head );
  assert( new_head && "Allocation failed!" );

  new_head->datum = value;
  new_head->next  = list;

  return new_head;
}

T       list_delete( T list, void (*destructor)( datum_t ) ) {
  while ( !list_is_empty( list ) ) {
    T next = list_next( list );
    if ( destructor ) destructor( list_first( list ) );
    free( list );
    list = next;
  }

  return list;
}

#undef T
#undef lst
#undef link_t
#undef tag
#undef datum_t
#undef list_new
#undef list_is_empty
#undef list_first
#undef list_next
#undef list_prepend
#undef list_delete
