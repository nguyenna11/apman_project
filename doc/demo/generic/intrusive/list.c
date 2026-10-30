#include <assert.h>

#include "list.h"

void      list_reset( list_t *list ) {
  assert( list && "List head is NULL!" );

  list->head.next = NULL;
  list->length    = 0;
  list->offset    = 0;

  return;
}

void      link_init( link_t *link ) {
  assert( link && "Link is NULL!" );

  link->next = NULL;

  return;
}

void      list_prepend( list_t *list, link_t *link ) {
  assert( list && "List is NULL!" );
  assert( link && "Link is NULL!");

  link->next           = list->head.next;
  list->head.next = link;
  list->length++;

  return;
}
