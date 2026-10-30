#ifndef _LIST_H_
#define _LIST_H_

#include <stddef.h>

void   *list_new( void );
int     list_is_empty( void *list );
void   *list_prepend_link( ptrdiff_t value_size, void **list );
#define list_prepend( value, list )                                     \
  ( list_prepend_link( sizeof(value), (void**)&(list) ) ,               \
    ( *(list) = (value) ), (list) )
void   *list_next( void *list );
void   *list_delete( void *list, void (*destructor)( void* ) );

#endif
