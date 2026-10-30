#ifndef impl
#error "Missing implementation name"
#endif

#ifndef key_t
#error "Missing key type"
#endif

#define tag              CAT(key_t,CAT(_,impl))
#define key_fprint       CAT(key_t,_fprint)
#define value_t          key_t

#include <core/generics.h>

#define map               tag
#define map_test          CAT(test_,map)
#define map_stress_test   CAT(stress_test_,map)
#define map_stress        CAT(stress_,map)

/* Rebuild local symbols, incorporating 'impl' */
#define map_t             CAT(tag,_t)
#define maping_t          CAT(tag,_mapping_t)
#define map_delete        CAT(tag,_delete)
#define map_debug         CAT(tag,_debug)
#define map_clone         CAT(tag,_clone)
#define map_fprint        CAT(tag,_fprint)
#define map_length        CAT(tag,_length)
#define map_put           CAT(tag,_put)
#define map_get           CAT(tag,_get)
#define map_remove        CAT(tag,_remove)
#define map_is_empty      CAT(tag,_is_empty)

#include <unitest/unitest.h>

#include <ds/array.h>
#include <ds/combinadics.h>

#include <stdlib.h> /* size_t, random() */

static int map_stress(int N, key_t *keys) {
  map_t    map = NULL;
  int        K = 3;
  size_t *perm = array_of(N, *perm);
  size_t  errs = 0;
  size_t count = 0;

  for each(perm, permutations(K, N)) {

    for (int k = 0 ; k < K ; k++) {
      map_put(&map, keys[perm[k]], keys[perm[k]]+99);
    }

    for (int k = 0 ; k < K ; k++) {
      iterator_t found;
      found = map_get(map, keys[perm[k]]);
      if (!found.status) {
        ERROR("Inserted key "); key_fprint(stderr, keys + perm[k]);
        fprintf(stderr, " NOT FOUND\n");
        getchar();
        errs++;
      }
      else {
        mapping_t mapping;
        (void)next(&found, &mapping);
        if (mapping.value != keys[perm[k]] + 99) {
          ERROR("Inserted key "); key_fprint(stderr, keys + perm[k]);
          fprintf(stderr, " HAS WRONG VALUE "); key_fprint(stderr, &mapping.value);
          fprintf(stderr, "\n");
          errs ++;
          getchar();
        }
      }
    }

    for (int k = 0 ; k < K ; k++) {
      iterator_t found;
      map_remove(&map, keys[perm[k]]);

      found = map_get(map, keys[perm[k]]);
      if (found.status) {
        ERROR("Removed key "); key_fprint(stderr, keys + perm[k]);
        fprintf(stderr, " STILL FOUND\n");
        errs++;
      }
    }

    count++;
  }

  array_delete(&perm);
  map_delete(&map);

  test_assert(!errs,
              "Completed %zu %d-out-of-%d put/get/rm", count, K, N);

  return errs;
}

static int map_stress_test(int N) {
  key_t *keys = array_of(N, *keys);
  int    errs = 0;

  test_suite("Ascending keys");

  for (int i = 0 ; i < N ; i++) keys[i] = i;
  errs += map_stress(N, keys);


  test_suite("Descending keys");

  for (int i = 0 ; i < N ; i++) keys[i] = N-i-1;
  errs += map_stress(N, keys);


  test_suite("Random keys");

  for (int i = 0 ; i < N ; i++) keys[i] = random()%1000000;
  errs += map_stress(N, keys);

  array_delete(&keys);

  return errs;
}

int map_test(int N) {
  mapping_t  mapping;
  map_t     test_map = NULL;
  int           m, n;

  test_suite("De/alloc");
  test_map = map(NULL);

  test_assert(!map_length(test_map),
              "A new map has length zero");
  test_assert(map_is_empty(test_map),
              "A new map is empty");
  test_assert(test_map,
              "A new map is not NULL");

  n = 0;
  for (key_t i = 0 ; i < N ; i++)
    n += map_put(&test_map, i, i);

  test_assert(n==(N*(N+1))/2,
              "Can put() into map");

  map_delete(&test_map);
  test_assert(!test_map,
              "Map is NULL after map_delete()");


  test_map = NULL;

  test_assert(!map_length(test_map),
              "NULL map has length zero");
  test_assert(map_is_empty(test_map),
              "NULL map is empty");

  n = 0;
  for (key_t i = 0 ; i < N ; i++)
    n += map_put(&test_map, i, i);

  test_assert(n==(N*(N+1))/2 && N==(int)map_length(test_map),
              "Can put() into NULL map");

  test_suite("Insertion order");

  n = 0;
  m = 0;
  for each(&mapping, in(test_map)) {
    key_t v = m;
    iterator_t found = map_get(test_map, v);

    n += !found.status;
    if (!found.status) ERROR("Key %d not found\n", (int)v);

    n += (v != mapping.key);
    if (v != mapping.key) ERROR("Wrong key %d\n", (int)v);

    m++;
  }

  test_assert(0 == n && N == (int)map_length(test_map),
              "Can insert keys, preserving insertion order (n=%d,len=%d)", n, (int)map_length(test_map));

  test_suite("Removal");

  n = 0;
  for (key_t i = 0 ; i < N/2 ; i++) {
    key_t v = 2*i;
    n += map_remove(&test_map, v);
  }


  n = 0;
  m = 0;
  for each(&mapping, in(test_map)) {
    key_t v = 2*m;
    iterator_t found;
    found = map_get(test_map, v);
    n += !!found.status;
    if (!!found.status) ERROR("Found removed key %d\n", (int)v);

    v++;
    found = map_get(test_map, v);
    n +=  !found.status;
    if (!found.status) ERROR("Key %d not found\n", (int)v);
    m++;
  }

  test_assert(0==n && N/2 == (int)map_length(test_map),
              "Can remove keys, preserving insertion order (n=%d,len=%d)",
              n, (int)map_length(test_map));

  n = 0;
  for (key_t i = 0 ; i < N/2 ; i++) {
    key_t v = 2*i+1;
    n += map_remove(&test_map, v);
  }

  test_suite("Deletion");

  test_assert(n == ((N/2-1)*N/2)/2 && !test_map,
              "Map is deleted after last remove (n=%d,map=%p)", n, test_map);

  map_delete(&test_map);

  test_assert(!test_map,
              "map_delete() is idempotent");

  map_stress_test(2*N);

  return 0;
}

#undef impl
#undef map_test
#undef tag
#undef key_t
#undef value_t
#undef key_fprint

#undef map
#undef mapping_t
#undef map_debug
#undef map_delete
#undef map_clone
#undef map_fprint
#undef map_length
#undef map_insert
#undef map_member
#undef map_is_empty
