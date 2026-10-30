/**
 * @file ZZ-implementation-buggy.c
 * @author François Cayre <cayre@uvolante.org>
 * @brief Implementation testing howto.
 *
 * Implementation testing howto.
 *
 * @seealso ZZ-implementation-working.c, sorting.impl.test.c.
 */

#include <unitest/unitest.h>

#include <stdlib.h>

/*
  We start by importing common test code for defining implementation properties:
*/
#include "sorting.impl.test.c"

/*
  This is the test for the buggy implementation sorting_fun2 below:
*/
int sorting_fun2( int *array, int n ) {
  /* An allegedly better, yet still buggy replacement for sorting_fun1 */

  qsort( array, sizeof( *array ), n, compare_integers );

  return 0;
}

/*
  This code will segfault on testing the implementation `sorting_fun2`.

  Now try this example in a debugger:

  ```bash
  $ make clean && make tests/howto/ZZ_test-implementation-buggy.exe@debug [with=asan]
  ```
 */

int main ( int argc, char *argv[] ) {

  test_implementation( sorting_implementation, sorting_fun2 );
}
