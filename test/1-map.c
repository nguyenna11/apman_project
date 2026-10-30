#include <unitest/unitest.h>

#include <core/generics.h>
#include <core/math.h>

#include <stdint.h>
#include <stdlib.h>

/* In a rather unusual move, this file has two parts:
   1. Generate magic (== 3 mod 4) prime numbers with target growth factor;
   2. Actually test maps, this part follows right after the function below.

   Check main() if you need primes with a growth factor != 1.3 . Example:

   $ ./test/1-map --primes 1.7

   (and update include/ds/map.h accordingly.)

   These primes ensure complete buckets coverage with quadratic addressing.

   For powers-of-two table sizes, complete coverage with quadratic addressing
   relies on triangular numbers (no magic constants like the above needed).

   Hence, all our maps may always use all of their buckets, independently of:
   - their actual addressing scheme (linear/quadratic/chained),
   - the prime vs. power-of-two table size alternative.
 */

static inline size_t get_primes(uint64_t current, double growth) {
  size_t n = 1;

  printf("static uint64_t ht_primes[] = {\n");

  while (current < UINT64_MAX/2) {
    uint64_t rem = current;
    printf("%zuULL, ", current);
    for (size_t i = 0 ; rem == current ; i++) {
      current = (uint64_t)(growth*(double)(current+i));
    }
    rem = current % 4;
    if (rem <= 3) current += 3 - rem;
    else          current += 7 - rem;

    while (current < UINT64_MAX && !is_prime(current)) current += 4;

    if (!(n % 3)) printf("\n");

    n++;
  }

  printf("\n};\n\n/* %zu primes = 3 mod 4 with growth ~ %lg */\n", n, growth);

  return n;
}

/* Rest of the test is with integers.

   Test default table with floats to make sure all symbols are
   handled (#undef'd) correctly and we can instantiate any many
   map types for any key_t/value_t or variant we want.
*/
static inline int    float_fprint(FILE *fp, const void *floatp) {
  return fprintf(fp, "%f", *(float*)floatp);
}

static inline int    float_compare(const void *leftp, const void *rightp) {
  float  left = *(float*)leftp;
  float right = *(float*)rightp;
  return left < right ? -1 : left != right;
}

static inline size_t float_hash(float key) {
  return key;
}


#define impl             linear
#define key_t            float
#include "test/map-setup.h"
#include STRINGIFY(CONF_DIR/map/textbook/linear.h)
#include <ds/map.h>

#define key_t            float
#include "map-test.h"    /* Generate impl test code */


static inline int    int_fprint(FILE *fp, const void *intp) {
  return fprintf(fp, "%d", *(int*)intp);
}

static inline int    int_compare(const void *leftp, const void *rightp) {
  int  left = *(int*)leftp;
  int right = *(int*)rightp;
  return left < right ? -1 : left != right;
}

static inline size_t int_hash(int key) {
  return key;
}

/* Hashtables: Cover critical code paths */

#define impl             linear
#define key_t            int
#include "test/map-setup.h"
#include STRINGIFY(CONF_DIR/map/textbook/linear.h)
#include <ds/map.h>
#define key_t            int
#include "map-test.h"    /* Generate impl test code */


#define impl             quadratic
#define key_t            int
#include "test/map-setup.h"
#include STRINGIFY(CONF_DIR/map/textbook/quadratic.h)
#include <ds/map.h>
#define key_t            int
#include "map-test.h"    /* Generate impl test code */


#define impl             chained
#define key_t            int
#include "test/map-setup.h"
#include STRINGIFY(CONF_DIR/map/textbook/chained.h)
#include <ds/map.h>
#define key_t            int
#include "map-test.h"    /* Generate impl test code */


/* #define impl             robin_hood */
/* #define key_t            int */
/* #include "test/map-setup.h" */
/* #include STRINGIFY(CONF_DIR/map/textbook/robin-hood.h) */
/* #include <ds/map.h> */
/* #define key_t            int */
/* #include "map-test.h"    /\* Generate impl test code *\/ */


/* #define impl             linear_pow */
/* #define key_t            int */
/* #include "test/map-setup.h" */
/* #include STRINGIFY(CONF_DIR/map/textbook/linear-pow.h) */
/* #include <ds/map.h> */
/* #define key_t            int */
/* #include "map-test.h"    /\* Generate impl test code *\/ */


/* #define impl             linear_pow_fast */
/* #define key_t            int */
/* #include "test/map-setup.h" */
/* #include STRINGIFY(CONF_DIR/map/textbook/linear-pow-fast.h) */
/* #include <ds/map.h> */
/* #define key_t            int */
/* #include "map-test.h"    /\* Generate impl test code *\/ */


/* #define impl             linear_sec */
/* #define key_t            int */
/* #include "test/map-setup.h" */
/* #include STRINGIFY(CONF_DIR/map/textbook/linear-sec.h) */
/* #include <ds/map.h> */
/* #define key_t            int */
/* #include "map-test.h"    /\* Generate impl test code *\/ */


/* #define impl             linear_filtered */
/* #define key_t            int */
/* #include "test/map-setup.h" */
/* #include STRINGIFY(CONF_DIR/map/textbook/linear-filtered.h) */
/* #include <ds/map.h> */
/* #define key_t            int */
/* #include "map-test.h"    /\* Generate impl test code *\/ */


/* #define impl             linear_finalized */
/* #define key_t            int */
/* #include "test/map-setup.h" */
/* #include STRINGIFY(CONF_DIR/map/textbook/linear-finalized.h) */
/* #include <ds/map.h> */
/* #define key_t            int */
/* #include "map-test.h"    /\* Generate impl test code *\/ */


/* #define impl             linear_finalized_filtered */
/* #define key_t            int */
/* #include "test/map-setup.h" */
/* #include STRINGIFY(CONF_DIR/map/textbook/linear-finalized-filtered.h) */
/* #include <ds/map.h> */
/* #define key_t            int */
/* #include "map-test.h"    /\* Generate impl test code *\/ */


/* #define impl             none_linear */
/* #define key_t            int */
/* #include "test/map-setup.h" */
/* #include STRINGIFY(CONF_DIR/map/textbook/none-linear.h) */
/* #include <ds/map.h> */
/* #define key_t            int */
/* #include "map-test.h"    /\* Generate impl test code *\/ */


/* #define impl             none_robin_hood */
/* #define key_t            int */
/* #include "test/map-setup.h" */
/* #include STRINGIFY(CONF_DIR/map/textbook/none-robin-hood.h) */
/* #include <ds/map.h> */
/* #define key_t            int */
/* #include "map-test.h"    /\* Generate impl test code *\/ */


/* #define impl             reuse_linear */
/* #define key_t            int */
/* #include "test/map-setup.h" */
/* #include STRINGIFY(CONF_DIR/map/textbook/reuse-linear.h) */
/* #include <ds/map.h> */
/* #define key_t            int */
/* #include "map-test.h"    /\* Generate impl test code *\/ */


/* #define impl             reuse_quadratic */
/* #define key_t            int */
/* #include "test/map-setup.h" */
/* #include STRINGIFY(CONF_DIR/map/textbook/reuse-quadratic.h) */
/* #include <ds/map.h> */
/* #define key_t            int */
/* #include "map-test.h"    /\* Generate impl test code *\/ */


/* Nodes */

#define impl             list
#define key_t            int
#include "test/map-setup.h"
#include STRINGIFY(CONF_DIR/map/textbook/list.h)
#include <ds/map.h>
#define key_t            int
#include "map-test.h"    /* Generate impl test code */


#define impl             list_sorted
#define key_t            int
#include "test/map-setup.h"
#include STRINGIFY(CONF_DIR/map/textbook/list-sorted.h)
#include <ds/map.h>
#define key_t            int
#include "map-test.h"    /* Generate impl test code */


/* #define impl             bst */
/* #define key_t            int */
/* #include "test/map-setup.h" */
/* #include STRINGIFY(CONF_DIR/map/textbook/bst.h) */
/* #include <ds/map.h> */
/* #define key_t            int */
/* #include "map-test.h"    /\* Generate impl test code *\/ */


/* #define impl             treap */
/* #define key_t            int */
/* #include "test/map-setup.h" */
/* #include STRINGIFY(CONF_DIR/map/textbook/treap.h) */
/* #include <ds/map.h> */
/* #define key_t            int */
/* #include "map-test.h"    /\* Generate impl test code *\/ */


/* #define impl             hashtrie */
/* #define key_t            int */
/* #include "test/map-setup.h" */
/* #include STRINGIFY(CONF_DIR/map/textbook/hashtrie-2.h) */
/* #include <ds/map.h> */
/* #define key_t            int */
/* #include "map-test.h"    /\* Generate impl test code *\/ */


/* Tests */

void test_basic(size_t N) {
  test_float_linear(N);
  test_int_linear(N);
  test_int_quadratic(N);
  test_int_chained(N);
}

/* void test_homeslot(size_t N) { */
/*   test_int_linear_pow(N); */
/*   test_int_linear_pow_fast(N); */
/* } */

/* void test_hashing(size_t N) { */
/*   test_int_linear_filtered(N); */
/*   test_int_linear_finalized(N); */
/*   test_int_linear_finalized_filtered(N); */
/* } */

/* void test_tombstones(size_t N) { */
/*   test_int_reuse_linear(N); */
/*   test_int_reuse_quadratic(N); */
/*   /\* Those last two are really hard: *\/ */
/*   test_int_none_linear(N); */
/*   test_int_none_robin_hood(N); */
/* } */

void test_hashtables(size_t N) {
  test_basic(N);
  /* test_homeslot(N); */
  /* test_int_linear_sec(N); */
  /* test_hashing(N); */
  /* test_tombstones(N); */
}

void test_nodes(size_t N) {
  test_int_list(N);
  test_int_list_sorted(N);
  /* test_int_hashtrie(N); */
  /* test_int_bst(N); */
  /* /\* This one may be hard: *\/ */
  /* test_int_treap(N); */
}

int main(int argc, char *argv[]) {

  if (argc > 1 && !strcmp(argv[1], "--primes")) {
    double growth = 1.3;
    if (argc > 2 && !sscanf(argv[2], "%lg", &growth)) {
      WARNING("Could not read float from %s, defaulting to %lg\n", argv[2], growth);
    }
    get_primes(7, growth);
  }
  else {
    size_t N = 12;

    unit_test(argc, argv);

    N += N%2; /* Test assumes N even */

    /* Hashtables */
    test_hashtables(N);

    /* Node-based structs */
    test_nodes(N);
  }

  exit(EXIT_SUCCESS);
}
