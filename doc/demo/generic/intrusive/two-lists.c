#include <stdlib.h> /*  exit(3), EXIT_SUCCESS */
#include <stdio.h>

#include "list.h"

typedef struct {
  link_t      list;
  link_t      even;
  int        value;
} datum_t;

int      datum_print( datum_t *datum )  { return printf( "%d ", datum->value ); }
void     datum_delete( datum_t *datum ) { free( datum ); }

datum_t *datum_new( int value ) {
  datum_t *datum = malloc( sizeof *datum );
  assert( datum && "Allocation failed!" );

  link_init( &datum->list );
  link_init( &datum->even );
  datum->value = value;

  return datum;
}

int main () {
  list_t  all_ints;
  list_t even_ints;

  list_init(  &all_ints, datum_t, list );
  list_init( &even_ints, datum_t, even );

  for ( int i = 0 ; i < 6 ; i++ ) {
    datum_t *ith_integer = datum_new( i );

    list_push( &all_ints, ith_integer );

    if ( 0 == ith_integer->value % 2 )
      list_push( &even_ints, ith_integer );
  }

  printf( " ALL ints: " );
  list_foreach( &all_ints, integer )  { datum_print( integer ); }
  printf( "\n" );

  printf( "EVEN ints: " );
  list_foreach( &even_ints, integer ) { datum_print( integer ); }
  printf( "\n" );

  list_destroy( &all_ints, datum_delete );
  list_reset( &even_ints );

  exit( EXIT_SUCCESS );
}
