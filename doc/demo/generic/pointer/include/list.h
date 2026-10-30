#ifndef _LIST_H_
#define _LIST_H_

typedef struct link_t *list_t;

list_t list_new( void );
int    list_is_empty( list_t list );
void  *list_first( list_t list );
list_t list_next( list_t list );
list_t list_prepend( void *value, list_t list );
list_t list_delete( list_t list, void (*destructor)( void* ) );

#endif
