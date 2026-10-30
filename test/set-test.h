#ifndef impl
#error "Missing implementation name"
#endif

#include <core/generics.h>

#define set_test     CAT(test_,impl)

/* Rebuild local symbols, incorporating 'impl' */
#define set          CAT(set_,impl)
#define set_t        CAT(set,_t)
#define set_delete   CAT(set,_delete)
#define set_clone    CAT(set,_clone)
#define set_fprint   CAT(set,_fprint)
#define set_length   CAT(set,_length)
#define set_insert   CAT(set,_insert)
#define set_member   CAT(set,_member)
#define set_remove   CAT(set,_remove)
#define set_is_empty CAT(set,_is_empty)
#define set_union    CAT(set,_union)
#define set_inter    CAT(set,_inter)
#define set_diff     CAT(set,_diff)
#define set_symdiff  CAT(set,_symdiff)

#include <unitest/unitest.h>

#include <stddef.h> /* size_t */

int set_test(int N) {
  set_t  test_set = NULL;
  set_t odd, even;
  int           n; /* In general, #errors in current test */

  test_suite("De/alloc");
  test_set = set(NULL);

  test_assert(!set_length(test_set),
              "A new set has length zero");
  test_assert(set_is_empty(test_set),
              "A new set is empty");
  test_assert(test_set,
              "A new set is not NULL");

  n = 0;
  for (int i = 0 ; i < N ; i++)
    n += set_insert(&test_set, i);

  test_assert(n==(N*(N+1))/2,
              "Can insert() into set");

  fprintf(stderr, "Set: "); set_fprint(stderr, test_set);
  fprintf(stderr, "\n");

  set_delete(&test_set);
  test_assert(!test_set,
              "Set is NULL after set_delete()");

  set_delete(&test_set);
  test_assert(!test_set,
              "set_delete() is idempotent");


  test_set = NULL;

  test_assert(!set_length(test_set),
              "NULL set has length zero");
  test_assert(set_is_empty(test_set),
              "NULL set is empty");

  n = 0;
  for (int i = 0 ; i < N ; i++)
    n += set_insert(&test_set, i);

  test_assert(n==(N*(N+1))/2 && N==(int)set_length(test_set),
              "Can insert() into NULL set");

  set_delete(&test_set);


  test_suite("Set ops");

  test_set = NULL;
  odd      = NULL;
  even     = NULL;

  n = set_is_empty(odd);

  for (int i = 0 ; i < N ; i++) {
    int e = 2*i;
    int o = 2*i+1;
    set_insert(&odd , o);
    set_insert(&even, e);
  }

  test_assert(n && !set_is_empty(odd),
              "Can test for empty set");

  test_set = set_union(odd, even);

  n = 0;
  for (int i = 0 ; i < N ; i++) {
    iterator_t found;
    int e = 2*i;
    int o = 2*i+1;

    found = set_member(test_set, e);
    n += !!found.status;
    found = set_member(test_set, o);
    n += !!found.status;
  }

  test_assert(n == 2*N && (int)set_length(test_set) == n,
              "Can compute union");

  test_assert(!set_inter(odd, even),
              "Can compute inter");

  set_delete(&odd);

  odd = set_diff(test_set, even);

  n = 0;
  for (int i = 0 ; i < N ; i++) {
    iterator_t found;
    int o = 2*i+1;
    found = set_member(test_set, o);
    n += !!found.status;
  }

  test_assert(n == N && (int)set_length(odd) == n,
              "Can compute diff");

  set_delete(&test_set);

  test_set = set_symdiff(odd, even);

  n = 0;
  for (int i = 0 ; i < N ; i++) {
    iterator_t found;
    int e = 2*i;
    int o = 2*i+1;
    found = set_member(test_set, e);
    n += !!found.status;
    found = set_member(test_set, o);
    n += !!found.status;
  }

  test_assert(n == 2*N && (int)set_length(test_set) == n,
              "Can compute symdiff");

  set_delete(&test_set);

  set_delete(&odd);
  set_delete(&even);

  return 0;
}

#undef impl
#undef set_test

#undef set
#undef set_delete
#undef set_clone
#undef set_fprint
#undef set_length
#undef set_insert
#undef set_member
#undef set_remove
#undef set_is_empty
#undef set_union
#undef set_inter
#undef set_diff
#undef set_symdiff
