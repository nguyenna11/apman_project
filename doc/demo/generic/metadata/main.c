#include <stdlib.h> /* exit(3), EXIT_SUCCESS */
#include <stdio.h>

#include "list.h"

int main () {

  int    *ilist = list_new();
  double *dlist = list_new();

  printf( " >> Genericity by metadata hiding\n" );

  for ( int i = 0 ; i < 6 ; i++ ) {
    ilist = list_prepend( i, ilist );
    dlist = list_prepend( (double)i, dlist );
  }

  for ( int *ith = ilist ; !list_is_empty( ith ) ; ith = list_next( ith ) ) {
    printf( "INT    %d\n", *ith );
  }

  for ( double *ith = dlist ; !list_is_empty( ith ) ; ith = list_next( ith ) ) {
    printf( "DOUBLE %lf\n", *ith );
  }

  list_delete( ilist, NULL );
  list_delete( dlist, NULL );

  exit( EXIT_SUCCESS );
}
