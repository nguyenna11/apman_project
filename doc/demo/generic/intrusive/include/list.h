#ifndef _LIST_H_
#define _LIST_H_

#include <stddef.h> /* ptrdiff_t */
#include <assert.h>

typedef struct link_t {
  struct link_t *next;
} link_t;

typedef struct {
  link_t      head;
  ptrdiff_t length;
  ptrdiff_t offset;
} list_t;

void      list_reset( list_t *list_head );
void      link_init( link_t *list );
void      list_prepend( list_t *list_head, link_t *link );

#define   list_push( list_head_ptr, object )    \
  list_prepend( list_head_ptr, (link_t*)( (char*)(object) + (list_head_ptr)->offset ) )

#define   list_init( list_head_ptr, type, list_field ) do {         \
    link_init( &(list_head_ptr)->head );                            \
    (list_head_ptr)->offset = offsetof( type, list_field );         \
  } while ( 0 )

#define   list_foreach( list_head_ptr, element )                        \
  for ( void *element, *once = &once ; once ; once = NULL )             \
    for ( link_t *link = (list_head_ptr)->head.next, *next ; link && ( ( next = link->next ) || 1 ) && ( element = (char*)link - (list_head_ptr)->offset ) ; link = next )

#define   list_destroy( list_head_ptr, destructor ) do {            \
    list_foreach( list_head_ptr, element ) destructor( element );   \
    list_reset( list_head_ptr );                                    \
  } while ( 0 )

#endif
