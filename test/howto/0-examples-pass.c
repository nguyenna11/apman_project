/**
 * @file 0-examples-pass.c
 * @author François Cayre <cayre@uvolante.org>
 * @brief Basic test examples.
 *
 * Basic test examples.
 */

#include <unitest/unitest.h>

#include <assert.h> /* Check pre-/post-conditions */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static int sum( int *array, int n ) {
  int i, ret = 0;

  /*
    More often than not, a function may only operate on restricted
    sets of input parameters, which should therefore be checked
    beforehand.

    These restricted sets of valid parameter values are called
    *pre-conditions*.

    Pre-conditions must be subject to assert()'s and are tested
    with test_abort() below:
   */
  assert( NULL != array );
  assert( n >= 0 );

  for ( i = 0 ; i < n ; i++ ) {
    ret += array[ i ];
  }

  /*
    A function result may also be subject to *post-conditions* in
    order to check that it behaved correctly.

    Here, we have no post-conditions.

    Checking pre-/post-conditions is called *contract programming*.

    This should be your default design pattern when writing functions.
   */

  return ret;
}

static int use_sum( int *array, int n ) {
  int ret = sum( array, n );

  printf( "The sum is: %d\n", ret );

  return ret;
}

static int f( int v ) {
  printf( "Computing f(%d) -> %d\n", v, v+1 );

  return v+1;
}

int main( int argc, char *argv[] ) {

  int array[ 4 ] = { 1, 2, 3, 4 };
  int  a = 10, b = 4;

  /* All your test programs must start with: */
  unit_test( argc, argv );


  test_suite( "Basic features" );

  /* Suppose you want to check something is true: */
  test_assert( 6 == sum( array, 3 ),
               "Positive test that passes" );

  /* Suppose you want to check something is false: */
  test_assert( 4 != sum( array, 2 ),
               "Negative test that passes" );

  test_abort( sum( NULL, 3 ),
              "Can check for abort()'s (pre-/post-conditions)" );

  /* Sometimes, you want to check something that you print: */
  test_oracle_start( stdout );
  use_sum( array, 4 );
  test_oracle_check( "An inline oracle that passes", "The sum is: 10\n" );

  test_oracle_start( stdout );
  use_sum( array, 4 );
  test_oracle_check( "Trailing \\n's are ignored in oracle and output", "The sum is: 10" );


  test_suite( "Advanced features" );

  test_assert( a == sum( array, b ),
               "test_assert() comment strings are interpreted printf()-like, a=%d", a );

  /* Oracle files are searched in `oracle_dir` that may be set with
     `--test-oracles=oracle_dir`. By default, `oracle_dir` is `"."` (the
     current working directory), so the following test is equivalent to:

     ```C
     test_oracle( "Oracles can be stored on disk, check test/data/howto/oracle/test.txt",
                  "test.txt" );
     ```
     Executed with:
     ```bash
     $ ./test/howto/0-examples-pass --test-oracles=test/data/howto/oracle
     ```
   */
  test_oracle_start( stdout );
  use_sum( array, b );
  test_oracle_check_file( "Oracles can be stored on disk, check test/data/howto/oracle/test.txt", DATA_DIR"howto/test.txt" );

  test_oracle_start( stdout );
  f( 2 );
  test_oracle_check( "Inline oracle are interpreted printf()-like", "Computing f(%d) -> %d", 2, 3 );

  test_oracle_start( stdout );
  f( 2 );
  test_oracle_check_file( "File oracles are interpreted printf()-like, too", DATA_DIR"howto/test.parms.txt", 2, 3 );


  test_suite( "One more thing..." );

  INFO( "\nUse this kind of blocks for informative output!\nThey behave like printf( \"%s\" )\n", "my format string" );

  VERBOSE( "This one is printed whenever option -v is present!\n" );

  WARNING( "\nUse this kind of blocks for warnings!\n\n" );

  ERROR( "And print errors\nwith this kind of blocks!\n\n" );

  exit( EXIT_SUCCESS );
}
