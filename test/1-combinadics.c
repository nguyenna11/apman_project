#include <ds/combinadics.h>

#include <core/math.h>
#include <ds/array.h>

#include <unitest/unitest.h>
#include <stdlib.h>

static int double_fprint(FILE *fp, const void *doublep) {
  return fprintf(fp, "%lg", *(double*)doublep);
}

static int int_fprint(FILE *fp, const void *intp) {
  return fprintf(fp, "%d", *(int*)intp);
}

void ranges(size_t N) {
  double    *x;
  int *indices;
  int      idx;
  double     v;
  size_t     n;

  N *= 2;

  test_suite("range [indices]");

  indices = array(*indices);

  n = 0;
  for each(&idx, range(-N,2,N)) {
    if (!n) iterator_debug(&it);
    array_push(&indices, &idx);
    n++;
  }

  INFO("Indices: ");
  array_fprint(stderr, indices, int_fprint);
  fprintf(stderr, "\n");

  test_assert(n == N+1,
              "Generate range of %zu indices", n);

  array_delete(&indices);


  test_suite("linspace [reals]");

  x = array(*x);

  n = 0;
  for each(&v, linspace(N/-2., 0.5, N/2.)) {
    if (!n) iterator_debug(&it);
    array_push(&x, &v);
    n++;
  }

  INFO("Values: ");
  array_fprint(stderr, x, double_fprint);
  fprintf(stderr, "\n");

  test_assert(n == 2*N+1,
              "Generate linspace of %zu values", n);

  array_delete(&x);
}


void combines(size_t N) {
  size_t *comb;
  size_t     K = N / 2;
  size_t     n = 0;

  test_suite("combinations [lex]");

  comb = array_of(K, size_t);

  n = 0;
  for each(comb, combinations(K, N)) {
    if (!n) iterator_debug(&it);
    fprintf(stderr, " %3zu :: ", n);
    for (size_t i = 0 ; i < K ; i++) fprintf(stderr, "%zu ", comb[i]);
    fprintf(stderr, "\n");
    n++;
  }

  test_assert(n == Ckn(K, N),
              "Generate C(%zu,%zu)=%zu lex combinations", K, N, Ckn(K, N));

  array_delete(&comb);
}


void permutes(size_t N) {
  size_t *perm;
  size_t     K = N / 2;
  size_t     n = 0;

  test_suite("permutations [lex]");

  perm = array_of(N, size_t);

  n = 0;
  for each(perm, permutations(K, N)) {
    if (!n) iterator_debug(&it);
    fprintf(stderr, " %3zu :: ", n);
    for (size_t i = 0 ; i < K ; i++) fprintf(stderr, "%zu ", perm[i]);
    fprintf(stderr, "\n");
    n++;
  }

  test_assert(n == Pkn(K, N),
              "Generate P(%zu,%zu)=%zu lex permutations", K, N, Pkn(K, N));


  test_suite("permutations [rnd]");

  n = 0;
  for each(perm, permutations(0, N)) {
    if (2*N == n) break;
    if (!n) iterator_debug(&it);
    fprintf(stderr, " %3zu :: ", n);
    for (size_t i = 0 ; i < N ; i++) fprintf(stderr, "%zu ", perm[i]);
    fprintf(stderr, "\n");
    n++;
  }

  test_assert(n == 2*N,
              "Generate %zu random permutations", 2*N);


  array_delete(&perm);
}


int main(int argc, char *argv[]) {
  size_t N = 4;

  unit_test(argc, argv);

  ranges(N);
  combines(N);
  permutes(N);

  exit(EXIT_SUCCESS);
}
