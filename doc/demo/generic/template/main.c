#include <stdlib.h> /*  exit(3), EXIT_SUCCESS */
#include <stdio.h>

#include "type/list-int.h"
#include "type/list-double.h"

int main () {

  list_int_t    ilist = list_int_new();
  list_double_t dlist = list_double_new();

  printf( " >> Genericity by code templating\n" );

  for ( int i = 0 ; i < 6 ; i++ ) {
    ilist = list_int_prepend( i, ilist );
    dlist = list_double_prepend( (double)i, dlist );
  }

  for ( list_int_t ith = ilist ; !list_int_is_empty( ith ) ; ith = list_int_next( ith ) ) {
    printf( "INT    %d\n", list_int_first( ith ) );
  }

  for ( list_double_t ith = dlist ; !list_double_is_empty( ith ) ; ith = list_double_next( ith ) ) {
    printf( "DOUBLE %lf\n", list_double_first( ith ) );
  }

  list_int_delete( ilist, NULL );
  list_double_delete( dlist, NULL );

  exit( EXIT_SUCCESS );
}
