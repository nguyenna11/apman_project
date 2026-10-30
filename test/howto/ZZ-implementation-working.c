/**
 * @file ZZ-implementation-working.c
 * @author François Cayre <cayre@uvolante.org>
 * @brief Implementation testing howto.
 *
 * Implementation testing howto.
 *
 * @seealso ZZ-implementation-buggy.c, sorting.impl.test.c.
 */

#include <unitest/unitest.h>

#include <stdlib.h>

/*
  We start by importing common test code for defining implementation properties:
*/
#include "sorting.impl.test.c"

/*
  This is the test for the working implementation sorting_fun1 below:
*/
#include <assert.h>

int sorting_fun1( int *array, int n ) {
  /* This one already works as expected */

  if ( NULL == array ) return 0;

  assert( n >= 0 );

  qsort( array, sizeof( *array ), n, compare_integers );

  return 0;
}

int main ( int argc, char *argv[] ) {

  test_implementation( sorting_implementation, sorting_fun1 );
}
