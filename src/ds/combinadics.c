#include <ds/combinadics.h>

#include <common/logging.h>

#include <assert.h>

/* range   : linear step on int's     */
/* linspace: linear step on double's  */

typedef enum { I_CURR, I_STEP, I_STOP } linear_step_parm_t;

#define LINEAR_STEP_MAKE_DEBUG(generator, fmt)      \
  (void)unused;                                     \
  DEBUG("  "generator"\n");                         \
  DEBUG("    step     "fmt"\n", parm[I_STEP]);      \
  DEBUG("    stop     "fmt"\n", parm[I_STOP]);      \
  DEBUG("    current  "fmt"\n", parm[I_CURR]);      \
  return 0

#define PRECOND_OR_RETURN(parm,ret)                 \
  if (parm[I_STEP] > 0) {                           \
    if (parm[I_CURR] > parm[I_STOP]) return ret;    \
  }                                                 \
  else {                                            \
    if (parm[I_CURR] < parm[I_STOP]) return ret;    \
  }                                                 \

#define LINEAR_STEP_MAKE_INIT(it,parm)              \
  it.delete     = NULL;                             \
  parm[I_CURR]  = start;                            \
  parm[I_STEP]  = step;                             \
  parm[I_STOP]  = stop;                             \
  assert(parm[I_STEP] && "I_STEP cannot be zero");  \
  PRECOND_OR_RETURN(parm,it);                       \
  it.status     = 1;                                \
  return it

#define LINEAR_STEP_MAKE_NEXT(parm)                 \
  PRECOND_OR_RETURN(parm,0);                        \
  *value        = parm[I_CURR];                     \
  parm[I_CURR] += parm[I_STEP];                     \
  return 1


static int range_debug(iterator_t *it, void *unused) {
  int *parm = (void*)it->on_stack;
  (void)parm; /* bc DEBUG undefined with codegen=release */
  LINEAR_STEP_MAKE_DEBUG("range", "%d");
}

static int range_next(iterator_t *it, void *itervarp) {
  int     *parm = (void*)it->on_stack;
  int    *value = itervarp;
  LINEAR_STEP_MAKE_NEXT(parm);
}

iterator_t range(int start, int step, int stop) {
  iterator_t it = { 0 };
  int     *parm = (void*)it.on_stack;
  it.debug      = range_debug;
  it.next       = range_next;
  LINEAR_STEP_MAKE_INIT(it, parm);
}

static int linspace_debug(iterator_t *it, void *unused) {
  double  *parm = (void*)it->on_stack;
  (void)parm; /* bc DEBUG undefined with codegen=release */
  LINEAR_STEP_MAKE_DEBUG("linspace", "%lg");
}

static int linspace_next(iterator_t *it, void *itervarp) {
  double *value = itervarp;
  double  *parm = (void*)it->on_stack;
  LINEAR_STEP_MAKE_NEXT(parm);
}

iterator_t linspace(double start, double step, double stop) {
  iterator_t it = { 0 };
  double  *parm = (void*)it.on_stack;
  it.debug      = linspace_debug;
  it.next       = linspace_next;
  LINEAR_STEP_MAKE_INIT(it, parm);
}

#undef LINEAR_STEP_MAKE_DEBUG
#undef LINEAR_STEP_MAKE_INIT
#undef LINEAR_STEP_MAKE_NEXT


/* Now for somewhat more cheesy stuff:

   # combinations: K out of N (lexicographical order)
     User provides size_t[K] indices array

   # permutations: K out of N
     User provides size_t[N] indices array

     If K>0, next permutation (lex order) is the K-prefix in the indices array
     If K==0, compute next random Fisher-Yates permutation, get N indices
 */


#include <core/mem.h>

typedef struct { /* Or we can slice the stack extra space with a parm struct: */
  size_t K;
  size_t N;
  size_t current;
} comb_state_t, perm_state_t;

static int combinations_debug(iterator_t *it, void *unused) {
  comb_state_t *s = (void*)it->on_stack;
  (void)unused;
  (void)s; /* bc DEBUG undefined with codegen=release */

  DEBUG("  combinations\n");
  DEBUG("    K        %zu\n", s->K);
  DEBUG("    N        %zu\n", s->N);
  DEBUG("    current  #%zu\n", s->current);

  return 0;
}

static int combinations_next(iterator_t *it, void *itervarp) {
  comb_state_t *s = (void*)it->on_stack;
  size_t *indices = itervarp;

  if (s->K > s->N || 0 == s->K) return 0;

  if (0 == s->current)
    for (size_t i = 0 ; i < s->K ; i++) indices[i] = i;
  else {
    size_t i = s->K;

    for ( ; i > 0 && indices[i - 1] == s->N + (i-1) - s->K ; i--);

    if (i == 0) return 0;

    indices[i - 1]++;
    for (size_t j = i; j < s->K; j++)
      indices[j] = indices[j - 1] + 1;
  }

  s->current++;

  return 1;
}

iterator_t combinations(size_t K, size_t N) {
  iterator_t   it = { 0 };
  comb_state_t *s = (void*)it.on_stack;

  s->K      = K;
  s->N      = N;

  it.debug  = combinations_debug;
  it.next   = combinations_next;

  it.status = !(s->K > s->N || 0 == s->K);

  return it;
}


static int permutations_debug(iterator_t *it, void *unused) {
  perm_state_t *s = (void*)it->on_stack;
  (void)unused;
  (void)s; /* bc DEBUG undefined with codegen=release */

  DEBUG("  permutations\n");
  DEBUG("    K        %zu %s\n", s->K, !s->K?"(Fisher-Yates)":"");
  DEBUG("    N        %zu\n", s->N);
  DEBUG("    current  #%zu\n", s->current);

  return 0;
}


/* Source: @rici (see https://stackoverflow.com/a/51292710) */

/* Helper functions */
static void swap(size_t* indices, size_t a, size_t b) {
  size_t tmp = indices[a]; indices[a] = indices[b]; indices[b] = tmp;
}

static void flip(size_t* indices, size_t lo, size_t hi) {
  for (; lo + 1 < hi; ++lo, --hi) swap(indices, lo, hi - 1);
}

/* Given an array of n elements, finds the next permutation in
 * lexicographical order with a different k-prefix; in effect, it
 * generates all k-permutations of the array.
 * It is required that the suffix be sorted in ascending order. This
 * invariant will be maintained by the function.
 * Before the first call, the array must be sorted in ascending order.
 * Returns true unless the input is the last k-permutation.
 */

static int permutations_next(iterator_t *it, void *itervarp) {
  perm_state_t *s = (void*)it->on_stack;
  size_t *indices = itervarp;
  size_t     tail = 1;

  if (s->K > s->N || 0 == s->K) return 0;

  if (s->current) {
    // Find the rightmost element which is strictly less than some element to its
    // right.
    size_t tailmax = indices[s->N - 1];
    tail = s->K;
    while (tail && indices[tail - 1] >= tailmax)
      tailmax = indices[--tail];
    // If no pivot was found, the given permutation is the last one.
    if (tail) {
      size_t swap_in;
      size_t   pivot = indices[tail - 1];
      // Find the smallest element strictly greater than the pivot, either
      // by searching forward from the pivot or backwards from the end.
      if (pivot >= indices[s->N - 1])
        for (swap_in = tail; swap_in + 1 < s->K && indices[swap_in + 1] > pivot; ++swap_in);
      else
        for (swap_in = s->N - 1; swap_in > s->K && indices[swap_in - 1] > pivot; --swap_in);
      // Swap the pivots
      indices[tail - 1] = indices[swap_in];
      indices[swap_in]  = pivot;
      // Flip the tail.
      flip(indices, s->K, s->N);
      flip(indices, tail, s->N);
    }
  }
  else for (size_t i = 0 ; i < s->N ; i++) indices[i] = i;

  s->current++;

  return tail;
}

#include <stdlib.h>

static int permutations_rand_next(iterator_t *it, void *itervarp) {
  perm_state_t *s = (void*)it->on_stack;
  size_t *indices = itervarp;

  if (!s->N) return 0;

  if (!s->current)
    for (size_t i = 0 ; i < s->N ; i++) indices[i] = i;

  for (size_t i = 1 ; i < s->N ; i++) {
    size_t    j = random() % (i+1);
    size_t    t = indices[j];
    indices[j]  = indices[i];
    indices[i]  = t;
  }

  s->current++;

  return 1;
}

iterator_t permutations(size_t K, size_t N) {
  iterator_t   it = { 0 };
  perm_state_t *s = (void*)it.on_stack;

  s->K      = K;
  s->N      = N;

  it.debug  = permutations_debug;
  it.next   = s->K ? permutations_next : permutations_rand_next;

  it.status = s->K ? s->K <= s->N : !!s->N;

  return it;
}
