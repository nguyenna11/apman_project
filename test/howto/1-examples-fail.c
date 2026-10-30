/**
 * @file 1-examples-fail.c
 * @author François Cayre <cayre@uvolante.org>
 * @brief Test examples.
 *
 * Test examples.
 */

#include <unitest/unitest.h>

#include <stdlib.h>
#include <assert.h>

static int infinite_loop( void ) {

  printf( "Entering infinite loop\n" );

  while ( 1 ) sleep( 1 );

  /* Not reached: */
  return 0;
}

static int sum( int *array, int n ) {
  int i, ret = 0;

  assert( NULL != array );
  assert( n >= 0 );

  for ( i = 0 ; i < n ; i++ ) {
    ret += array[ i ];
  }
  return ret;
}

static int use_sum( int *array, int n ) {
  int ret = sum( array, n );

  printf( "The sum is: %d\n", ret );

  return ret;
}


int main ( int argc, char *argv[] ) {

  int array[ 4 ] = { 1, 2, 3, 4 };

  /* All your test programs must start with: */
  unit_test( argc, argv );


  /* Introduce a group a tests like this: */
  test_suite( "Basic features" );

  test_assert( 5 == sum( array, 3 ),
               "Positive test that fails" );

  test_assert( 3 != sum( array, 2 ),
               "Negative test that fails" );

  test_assert( 3 != sum( NULL, 2 ),
               "Assertions fail on abort()'s" );

  test_oracle_start( stdout );
  use_sum( array, 4 );
  test_oracle_check( "An inline oracle that fails", "The sum is very high" );


  test_suite( "Advanced features" );

  /* See the output for a missing / misspelled oracle file: */
  test_oracle_start( stdout );
  use_sum( array, 4 );
  test_oracle_check_file( "Oracles can be stored on disk, trying with missing file", "test.orc" );

  /* A time-out will interrupt each test after two seconds.
     Set arbitrary `timeout` value (in seconds) with `--test-timeout=timeout`.
  */
  test_assert( infinite_loop(),
           "Infinite loops stopped by time-out" );

  /* You can also hit Ctrl-C to stop a test: */
  test_assert( infinite_loop(),
           "\n\nSECOND INFINITE LOOP IN A ROW, NOW PRESS Ctrl-C BEFORE TIME-OUT!!!" );


  exit( EXIT_SUCCESS );
}
