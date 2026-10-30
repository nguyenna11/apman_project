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

typedef struct link_t *T;

T       list_new( void );
int     list_is_empty( T list );
datum_t list_first( T list );
T       list_next( T list );
T       list_prepend( datum_t value, T list );
T       list_delete( T list, void (*destructor)( datum_t ) );

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
