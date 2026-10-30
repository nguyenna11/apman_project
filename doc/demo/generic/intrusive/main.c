#include <stdlib.h> /*  exit(3), EXIT_SUCCESS */
#include <stdio.h>

#include "list.h"

typedef struct {
  link_t  list;
  int    value;
} int_datum_t;

int      int_datum_print( int_datum_t *datum )  { return printf( "%d ", datum->value ); }
void     int_datum_delete( int_datum_t *datum ) { free( datum ); }

int_datum_t *int_datum_new( int value ) {
  int_datum_t *datum = malloc( sizeof *datum );
  assert( datum && "Allocation failed!" );

  link_init( &datum->list );
  datum->value = value;

  return datum;
}

typedef struct {
  double  value;
  link_t   list;
} double_datum_t;

int      double_datum_print( double_datum_t *datum )  { return printf( "%lf ", datum->value ); }
void     double_datum_delete( double_datum_t *datum ) { free( datum ); }

double_datum_t *double_datum_new( int value ) {
  double_datum_t *datum = malloc( sizeof *datum );
  assert( datum && "Allocation failed!" );

  link_init( &datum->list );
  datum->value = value;

  return datum;
}

int main () {
  list_t ilist;
  list_t dlist;

  printf( " >> Genericity by intrusive definition\n" );

  list_init( &ilist, int_datum_t   , list );
  list_init( &dlist, double_datum_t, list );

  for ( int i = 0 ; i < 6 ; i++ ) {
    int_datum_t    *ith_int    = int_datum_new( i );
    double_datum_t *ith_double = double_datum_new( i );

    list_push( &ilist, ith_int );
    list_push( &dlist, ith_double );
  }

  list_foreach( &ilist, ith )  {
    printf( "INT    " ); int_datum_print( ith ); printf( "\n" );
  }

  list_foreach( &dlist, ith ) {
    printf( "DOUBLE " ); double_datum_print( ith ); printf( "\n" );
  }

  list_destroy( &ilist, int_datum_delete );
  list_destroy( &dlist, double_datum_delete );

  exit( EXIT_SUCCESS );
}
