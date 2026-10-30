#include <ds/array.h>

#include <unitest/unitest.h>
#include <stdlib.h>

static int int_fprint(FILE *fp, const void *intp) {
  return fprintf(fp, "%d", *(int*)intp);
}

static int int_compare(const void *leftp, const void *rightp) {
  int  left = *(int*)leftp;
  int right = *(int*)rightp;
  return left < right ? -1 : left != right;
}

void core(int N) {
  int    n;
  int *arr;

  test_suite("De/alloc");
  arr = array(int);

  test_assert(!array_length(arr),
              "A new dyn. array has length zero");
  test_assert(array_is_empty(arr),
              "A new dyn. array is empty");
  test_assert(arr,
              "A new dyn. array is not NULL");

  n = 0;
  for (int i = 0 ; i < N ; i++){
    printf("N:%d i:%d n:%d\n", N, i, n);
    n += array_push(&arr, &i);
  }

  test_assert(n==(N*(N+1))/2,
              "Can push() to array");

  fprintf(stderr, "Array: "); array_fprint(stderr, arr, int_fprint);
  fprintf(stderr, "\n");

  array_delete(&arr);
  test_assert(!arr,
              "Array is NULL after array_delete()");

  array_delete(&arr);
  test_assert(!arr,
              "array_delete() is idempotent");


  arr = NULL;

  test_assert(!array_length(arr),
              "NULL array has length zero");
  test_assert(array_is_empty(arr),
              "NULL array is empty");

  n = 0;
  for (int i = 0 ; i < N ; i++)
    n += array_push(&arr, &i);

  test_assert(n==(N*(N+1))/2 && N==(int)array_length(arr),
              "Can push() to NULL array");

  fprintf(stderr, "Pop from array: ");
  for(int popped ; !array_is_empty(arr) ; ) {
    array_pop(&arr,&popped);
    fprintf(stderr, "%d ", popped);
  }
  fprintf(stderr, "\n");

  test_assert(!arr,
              "Array is NULL again after last pop()");


  test_suite("C syntax");
  for (int i = 0 ; i < N ; i++) array_push(&arr, &i);

  n = 0;
  for (int i = 0 ; i < N ; i++) n += i != arr[i];

  test_assert(0==n,
              "Can access data with [] syntax");

  array_delete(&arr);
}

void get_rm(void **taggedp, int N, int RUNS,
            iterator_t (*get)(void *tagged, void *key, int (*compare)(const void *left, const void *right)),
            size_t (*rm)(void **taggedp, void *key, size_t obj_size, int all, int(*compare)(const void *left, const void *right))) {
  iterator_t found;
  int n = 0;

  for (int m = 0 ; m < RUNS ; m++) {
    int   v = rand() % (2*N);  /* Fuzz testing: 1000 runs */
    int   y = v < N;   /* Ground truth: y==1 iff v in arr */

    found = get(*taggedp, &v, int_compare);

    /* Check each random test against local ground truth: */
    n += (y != (found.status > 0));
    if (y != (found.status > 0)) {
      ERROR("v=%d, Array: ", v);
      array_fprint(stderr, *taggedp, int_fprint);
      fprintf(stderr, "\n");
    }
  }

  if (n) ERROR("%d/%d runs failed\n", n, RUNS);

  test_assert(0==n,
              "Can (not) find (non-)existing data");

  if (!rm) return;

  n = 0;
  for (int i = 0 ; i < N ; i++) {
    size_t length = array_length(*taggedp);

    rm(taggedp, &i, sizeof(i), 0, int_compare);

    n += (length-1) != array_length(*taggedp);

    rm(taggedp, &i, sizeof(i), 1, int_compare);

    found = get(*taggedp, &i, int_compare);

    if (found.status > 0) {
      ERROR("Not all %d's were removed (length=%zu).\n",
            i, array_length(*taggedp));
    }

    n += found.status > 0;
  }

  test_assert(0 == n, "Can remove any and all object");
}

void unsorted(int N, int LEN, int RUNS, int seed) {
  int *arr;

  test_suite("get/rm");
  arr = NULL;

  srand(seed);
  for (int i = 0 ; i < LEN ; i++) {
    int v = rand()%N;
    array_push(&arr, &v);
  }

  test_assert(LEN==(int)array_length(arr),
              "Can push() to NULL array");

  fprintf(stderr, "Array: "); array_fprint(stderr, arr, int_fprint);
  fprintf(stderr, "\n");

  get_rm((void**)&arr, N, RUNS, array_search, array_remove__);

  array_delete(&arr);


  test_suite("get/rm, unique");
  arr = NULL;

  srand(seed);
  for (int i = 0 ; i < LEN ; i++) {
    int v = rand()%N;
    array_push_unique(&arr, &v, int_compare);
  }

  test_assert(N==(int)array_length(arr),
              "Can push_unique() to NULL array");

  fprintf(stderr, "Array: "); array_fprint(stderr, arr, int_fprint);
  fprintf(stderr, "\n");

  get_rm((void**)&arr, N, RUNS, array_search, NULL);

  array_delete(&arr);
}

void sorted(int N, int LEN, int RUNS, int seed) {
  int *arr;

  test_suite("get/rm");
  arr = NULL;

  srand(seed);
  for (int i = 0 ; i < LEN ; i++) {
    int v = rand()%N;
    array_sorted_push(&arr, &v, int_compare);
  }

  test_assert(LEN==(int)array_length(arr) && array_is_sorted(arr, int_compare),
              "Can sorted_push() to NULL array");

  fprintf(stderr, "Array: "); array_fprint(stderr, arr, int_fprint);
  fprintf(stderr, "\n");

  get_rm((void**)&arr, N, RUNS, array_bsearch, array_sorted_remove__);

  array_delete(&arr);

  test_suite("get/rm, unique");
  arr = NULL;

  srand(seed);
  for (int i = 0 ; i < LEN ; i++) {
    int v = rand()%N;
    array_sorted_push_unique(&arr, &v, int_compare);
  }

  test_assert(N==(int)array_length(arr),
              "Can sorted_push_unique() to NULL array");

  fprintf(stderr, "Array: "); array_fprint(stderr, arr, int_fprint);
  fprintf(stderr, "\n");

  get_rm((void**)&arr, N, RUNS, array_bsearch, NULL);

  array_delete(&arr);
}

int main(int argc, char *argv[]) {
  size_t    N =   12;
  size_t  LEN =  100;
  size_t RUNS = 1000;
  int    seed =   42;

  unit_test(argc, argv);

  core(N);
  unsorted(N, LEN, RUNS, seed);
  sorted(N, LEN, RUNS, seed);

  exit(EXIT_SUCCESS);
}
