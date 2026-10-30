#include <stdlib.h> /* exit(3), EXIT_SUCCESS */
#include <stdio.h>
#include <assert.h>

#include "list.h"

int   int_print( void *value )  { return printf( "%d ", *(int*)value ); }
void  int_delete( void *value ) { free( value ); }

void *int_new( int value ) {
  int *int_on_heap = malloc( sizeof *int_on_heap );
  assert( int_on_heap && "Allocation failed!" );

  *int_on_heap = value;

  return int_on_heap;
}

int   double_print( void *value )  { return printf( "%lf ", *(double*)value ); }
void  double_delete( void *value ) { free( value ); }

void *double_new( double value ) {
  double *double_on_heap = malloc( sizeof *double_on_heap );
  assert( double_on_heap && "Allocation failed!" );

  *double_on_heap = value;

  return double_on_heap;
}


int main () {

  list_t ilist = list_new();
  list_t dlist = list_new();

  printf( " >> Genericity by void* pointers\n" );

  for ( int i = 0 ; i < 6 ; i++ ) {
    ilist = list_prepend( int_new( i ), ilist );
    dlist = list_prepend( double_new( (double)i ), dlist );
  }

  for ( list_t ith = ilist ; !list_is_empty( ith ) ; ith = list_next( ith ) ) {
    printf( "INT    "  ); int_print( list_first( ith ) );    printf( "\n" );
  }

  for ( list_t ith = dlist ; !list_is_empty( ith ) ; ith = list_next( ith ) ) {
    printf( "DOUBLE "  ); double_print( list_first( ith ) ); printf( "\n" );
  }

  list_delete( ilist, int_delete );
  list_delete( dlist, double_delete );

  exit( EXIT_SUCCESS );
}
