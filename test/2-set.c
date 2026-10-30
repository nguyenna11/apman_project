#include <unitest/unitest.h>
#include <stdlib.h>

static inline int int_fprint(FILE *fp, const void *intp) {
  return fprintf(fp, "%d", *(int*)intp);
}

static inline int int_compare(const void *_left, const void *_right) {
  int  left = *(int*)_left;
  int right = *(int*)_right;
  return left < right ? -1 : left != right;
}

static inline size_t int_hash(int key) {
  return key;
}

/* Apply set-test.h template to all possible implementations.
*/

#define impl             array /* self-test.h only parameter */
#define tag              CAT(set_,impl)
#define elem_t           int
#define elem_compare     int_compare
#define elem_fprint      int_fprint
#define ds_backend_array
#define ds_secure_removal
#include <ds/set.h>      /* Global template */
#include "set-test.h"    /* Local template  */

#define impl             sorted_array
#define tag              CAT(set_,impl)
#define elem_t           int
#define elem_compare     int_compare
#define elem_fprint      int_fprint
#define ds_backend_array
#define ds_elem_ordered
#define ds_secure_removal
#include <ds/set.h>      /* Global template */
#include "set-test.h"    /* Local template  */

#define impl             ht_dflt
#define tag              CAT(set_,impl)
#define elem_t           int
#define elem_compare     int_compare
#define elem_hash        int_hash
#define elem_fprint      int_fprint
#define ds_backend_table
#include <ds/set.h>      /* Global template */
#include "set-test.h"    /* Local template  */

int main(int argc, char *argv[]) {
  size_t    N =   12;

  unit_test(argc, argv);

  test_array(N);
  test_sorted_array(N);
  test_ht_dflt(N);

  exit(EXIT_SUCCESS);
}
