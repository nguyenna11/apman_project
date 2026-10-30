/*
  @seealso ZZ-implementation-working.c, ZZ-implementation-buggy.c.


  # Laziness hacking: Fast and reliable prototyping

  The following is for the lazy hacker in you. If you feel too lazy to become a
  lazy hacker, you can safely skip it.

  We assume the same running example as in `01_test-howto.c`.

  Now imagine we have two implementations of a sorting function:
  - `int sorting_fun1( int*, int ); // One easy shot that already works`
  - `int sorting_fun2( int*, int ); // An allegedly better, yet still buggy replacement`

  Any code that only relies on checked properties of our sorting function will
  continue to work just as is if we replaced it by another sorting function that
  would enforce the same properties.

  This code only makes sense if included from a test:
*/

#ifdef UNITEST_H

/* We will need to compare integers below: */
static int compare_integers( const void *p1, const void *p2 ) {
  int i1 = *(int*)p1;
  int i2 = *(int*)p2;

  if ( i1 == i2 ) return 0; else return i1 < i2 ? -1 : 1;
}

/* And also to check whether an array of integers is sorted: */
static int array_is_sorted( int *array, int n ) {
  int i;
  for ( i = 1 ; n > 1 && i < n ; i++ ) { /* Ensure 0 <= i-1 < i < n */
    if ( array[ i-1 ] > array[ i ] ) return 0;
  }

  return 1;
}

/*
  This is key for fast prototyping:
  1. You first write tests and a simple implementation that passes the tests,
     and you continue to build the other parts of the project similarly!
  2. Once the whole project works as planned, progressively improve selected
     parts by providing an alternate implementation.

  Then you can use the tests to quickly tell if you have a valid replacement,
  or how far you are (by concentrating on the failed tests).

  So you want to be able to check a set of defining properties against any
  arbitrary piece of code you might write -- so as to validate it as an
  alternate, checked implementation.

  This is done with function pointers as parameters like this:
*/

int test_pre_post_sorting( int(*fun)(int*,int) ) {
  /* Now `fun` is the name of any sorting function */
  int array[ 4 ] = { 1, 3, 2, 4 };

  test_abort( !fun( array, -1 ),
              "Pre: Array size must be non-negative" );

  test_assert( !fun( NULL, 3 ),
               "Pre: Array must be non-NULL" );


  test_assert( !fun( array, 4 ),
               "Post: Sorting finishes on valid inputs" );

  return 0;
}

int test_prop_sorting( int(*fun)(int*,int) ) {
  int array[ 4 ] = { 1, 3, 2, 4 }; // Local testing data

  /* Notice how testing whether the array is actally sorted, is moved at
     this level for more generality and robustness:
  */
  fun( array, 4 );
  test_assert( array_is_sorted( array, 4 ),
           "Prop: Sorting sorts" );

  fun( array, 4 );
  test_assert( array_is_sorted( array, 4 ),
           "Prop: Sorting is idempotent" );

  test_assert( !fun( array, 0 ),
           "Prop: Array size can be zero" );

  /* Expand with any other property you might want to check: stable sorting,
     etc.
  */

  return 0;
}

void sorting_implementation( int(*sorting_fun)(int*, int) ) {

  test_suite( "Sorting: pre/post-conditions" );
  test_pre_post_sorting( sorting_fun );

  test_suite( "Sorting: properties" );
  test_prop_sorting( sorting_fun );
}

#else /* Dummy stub if not included in a test */
#include <stdio.h>
int main () {
  return fprintf( stderr, "I was supposed to be included in a test!\n" );
}
#endif
