#ifndef benchmark
#error "Missing benchmark name"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <assert.h>
#include <math.h>

#include <common/logging.h>
#include <core/opt.h>
#include <ds/array.h>
#include <ds/combinadics.h>

typedef enum {
  W_ASCEND, W_DESCEND, W_RANDOM
} workload_t;

static int *bench_keys(size_t N, workload_t workload) {
  iterator_t permute;
  size_t       *perm;
  int          *keys = array_of(2*N, *keys); /* Two halves: present/absent */

  switch(workload) {
  case W_ASCEND:  for (size_t i = 0 ; i < 2*N ; i++) keys[i] = i;
    break;
  case W_DESCEND: for (size_t i = 0 ; i < 2*N ; i++) keys[i] = 2*N-1 - i;
    break;
  case W_RANDOM:
    perm    = array_of(2*N, *perm);
    permute = permutations(0, 2*N);
    (void)next(&permute,perm);
    for (size_t i = 0 ; i < 2*N ; i++) keys[i] = perm[i];
    array_delete(&perm);
    break;
  }

  return keys;
}

#include <time.h>

double elapsed_ns(double *prev) {
  double diff;
  struct timespec now;

  clock_gettime(CLOCK_MONOTONIC, &now);

  diff   = now.tv_nsec + 1e9 * now.tv_sec - *prev;
  *prev += diff;

  return diff;
}

typedef enum {PUT, GET, RM} bench_op_t;

typedef struct {
  char          *impl;
  workload_t     workload;
  size_t         N;
  size_t         run;

  double         timing[2][3]; /* absent vs. present */
  double         abpkv;   /* Mem. structure overhead */
} bench_result_t;

static int bench_result_fprint(FILE *fp, bench_result_t *runs) {
  int              ret = 0;
  bench_result_t   run;

  for each(&run, in(runs)) {
      char *wload;
      switch(run.workload) {
      case W_ASCEND:  wload = "ascend";
        break;
      case W_DESCEND: wload = "descend";
        break;
      case W_RANDOM:
      default:
        wload = "random";
        break;
      }
      ret += fprintf(fp, "%s,%s,%zu,%zu,", run.impl, wload, run.N, run.run);

      ret += fprintf(fp, "%lg,%lg,%lg,",
                     run.timing[0][PUT], run.timing[0][GET], run.timing[0][RM]);

      ret += fprintf(fp, "%lg,%lg,%lg",
                     run.timing[1][PUT], run.timing[1][GET], run.timing[1][RM]);

      ret += fprintf(fp, ",%lg\n", run.abpkv);
    }

  return ret;
}

static inline int double_compare(const void *leftp, const void *rightp) {
  double left  = *(double*)leftp;
  double right = *(double*)rightp;
  return left < right ? -1 : left > right;
}

static int bench_run_stat(bench_result_t *runs, int present, bench_op_t op, double *cold, double *avg, double *med, double *std) {
  int        ret = 0;
  size_t  nstats = array_length(runs)-1;
  double  *stats = array_of(nstats, *stats);

  switch(op) {
  case PUT:
    break;
  case GET:
    break;
  case RM:
    break;
  }

  *cold = runs[0].timing[present][op];

  for (size_t i = 0 ; i < nstats ; i++) stats[i] = runs[i+1].timing[present][op];
  qsort(stats, nstats, sizeof(*stats), double_compare);

  *med  = stats[nstats / 2];

  *avg  = 0.;
  for (size_t i = 0 ; i < nstats ; i++) {
    *avg += stats[i];
  }
  *avg /= nstats;

  *std  = 0.;
  for (size_t i = 0 ; i < nstats ; i++) {
    double diff = *avg - stats[i];
    *std += diff * diff;
  }
  *std /= nstats-1;
  *std  = sqrt(*std);

  array_delete(&stats);

  return ret;
}

static int bench_results_fprint(FILE *fp, bench_result_t **results) {
  int ret = 0;
  bench_result_t *runs;

  assert(     fp && "Missing output FILE pointer");
  assert(results && "Missing benchmark results");

  ret += fprintf(fp, "impl,workload,N,run,put/abs,get/abs,rm/abs,put/pres,get/pres,rm/pres,abpkv\n");

  for each(&runs, in(results)) {
      bench_result_fprint(fp, runs);
      ret += fprintf(fp, "\n");
    }

  return ret;
}

static void bench_results_delete(bench_result_t ***results) {
  bench_result_t *result;

  for each(&result, in(*results)) array_delete(&result);

  array_delete(results);

  return;
}


typedef struct {
  int        status;
  int        verbose;
  char      *impl;

  size_t     runs;
  struct {
    size_t     curr;
    size_t     step;
    size_t     stop;
  }          N;
  workload_t workload;

  int        primes;

} bench_args_t;

static bench_result_t  **bench_results(bench_args_t *args) {
  bench_result_t **results = NULL;
  int                    N;

  for each(&N, range(args->N.curr, args->N.step, args->N.stop)) {
      bench_result_t  *runs = array_of(args->runs, *runs);
      for (size_t r = 0 ; r < args->runs ; r++) {
        bench_result_t *run = runs+r;
        run->impl           = args->impl;
        run->N              = N;
        run->run            = r;
        run->workload       = args->workload;
      }
      array_push(&results, &runs);
    }

  return results;
}

static int bench_args_fprint(FILE *fp, bench_args_t *parms) {
  int ret = 0;

  ret += fprintf(fp, "Benchmarking %s\n", parms->impl);
  ret += fprintf(fp, "  Runs          %zu (+ cold cache)\n", parms->runs-1);
  ret += fprintf(fp, "  #Keys         %zu:%zu:%zu\n",
                 parms->N.curr, parms->N.step, parms->N.stop);
  ret += fprintf(fp, "  Distribution  ");
  switch(parms->workload) {
  case W_ASCEND : ret += fprintf(fp, "[0 -> %zu]", parms->N.curr); break;
  case W_DESCEND: ret += fprintf(fp, "[%zu -> 0]", parms->N.curr); break;
  case W_RANDOM : ret += fprintf(fp, "random %% %zu", parms->N.curr); break;
  }
  ret += fprintf(fp, "\n");

  return ret;
}

static bench_args_t bench_args_parse(int argc, char *argv[]) {
  bench_args_t parms = { 0 };
  char           *arg;

  parms.impl = basename(argv[0]) + strlen("set-bench-");

  arg = opt_string(argc, argv, "-v", "--verbose");
  if (arg) {
    parms.verbose = 1;
    sscanf(arg, "%d", &parms.verbose);
  }
  else if(opt_flag(argc, argv, "-v", "--verbose")) parms.verbose = 1;

  if (!opt_size_t(argc, argv, "-f", "--from", &parms.N.curr)) {
    ERROR("Missing initial #keys\n");
    parms.status++;
  }

  if (!opt_size_t(argc, argv, "-t", "--to", &parms.N.stop)) {
    ERROR("Missing final #keys\n");
    parms.status++;
  }

  if (!opt_size_t(argc, argv, "-s", "--step", &parms.N.step)) {
    ERROR("Missing step for #keys\n");
    parms.status++;
  }

  if (!opt_size_t(argc, argv, "-r", "--runs", &parms.runs)) {
    ERROR("Missing #runs\n");
    parms.status++;
  }
  parms.runs++; /* First is for cold cache */

  arg = opt_string(argc, argv, "-d", "--data");
  if (!arg) {
    ERROR("Missing data distribution (ascend, descend, random)\n");
    parms.status++;
  }
  else {
    if (!strcmp(arg, "ascend"))       parms.workload = W_ASCEND;
    else if (!strcmp(arg, "descend")) parms.workload = W_DESCEND;
    else if (!strcmp(arg, "random"))  parms.workload = W_RANDOM;
    else {
      ERROR("Unknown data distribution %s (ascend, descend, random)\n", arg);
      parms.status++;
    }
  }

  return parms;
}


#define elem_t           int

/* Rebuild local symbols, incorporating 'impl' */
#include <core/generics.h>

#define elem_compare     CAT(elem_t,_compare)
#define elem_fprint      CAT(elem_t,_fprint)

/* Run benchmark on integer sets */
static int    elem_fprint(FILE *fp, const void *elemp) {
  return fprintf(fp, "%d", *(elem_t*)elemp);
}

static int    elem_compare(const void *_left, const void *_right) {
  elem_t  left = *(elem_t*)_left;
  elem_t right = *(elem_t*)_right;
  return left < right ? -1 : left != right;
}

#if defined(ds_backend_table) || defined(ds_nd_treap) || defined(ds_nd_hashtrie)
#define elem_hash        CAT(elem_t,_hash)
static size_t elem_hash(elem_t key) { return key; }
#endif

#define tag              CAT(set_,benchmark)
#define ds_export_code
#include <ds/set.h>

#define set_bench    CAT(bench_,benchmark)
#define set          CAT(set_,benchmark)
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
#define set_stat     CAT(set,_stat)
#define set_stats    CAT(set,_stats)

#define SAMPLE_STATS 1000

#define bench_status(fmt, ...) do {                                     \
    int nchrs = fprintf(stderr, "\r\033[%dC"fmt, ret, ##__VA_ARGS__);   \
    fprintf(stderr, "%*s\r", 80-(nchrs), " ");                          \
  } while (0)

#if defined(bench_ht_debug)
#define bench_status_ext(descrp, fmt, ...) do {                         \
    char buff[81];                                                      \
    int nchrs, nht;                                                     \
    size_t probes = (descrp)->stats.probes;                             \
    set_stats(s, descrp);                                               \
    nht = snprintf(buff, sizeof buff, "apl=%.1f lf=%.2lf | +%zu/-%zu",  \
                   ((descrp)->stats.probes-probes)/(float)SAMPLE_STATS, \
                   (descrp)->stats.lf,                                  \
                   (descrp)->stats.rehash_up,                           \
                   (descrp)->stats.rehash_down);                        \
    nchrs = fprintf(stderr, "\r\033[%dC"fmt, ret, ##__VA_ARGS__);       \
    fprintf(stderr, "%*s%s\r", 70-(nchrs+nht), " ", buff);              \
  } while (0)
#else
#define bench_status_ext(descr, fmt, ...) do {                          \
    int nchrs = fprintf(stderr, "\r\033[%dC"fmt, ret, ##__VA_ARGS__);   \
    fprintf(stderr, "%*s\r", 80-(nchrs), " ");                          \
  } while (0)
#endif

#define res_width 5

#define disp_timing(t)                                                  \
  if (t >= 100)                                                         \
    nchrs = fprintf(stderr, "%4u", (unsigned)t);                        \
  else                                                                  \
    nchrs = fprintf(stderr, "%3.1lf", t);                               \
  if (nchrs < res_width) fprintf(stderr, "%*s", res_width-nchrs, " ")

int set_bench(bench_args_t *parms) {
  bench_result_t **results = bench_results(parms);
  map_descr_t        descr = set_stat(parms->impl);

  if (parms->verbose) {
    fprintf(stderr, "\n");
    mapdescr_fprint(stderr, &descr);

    fprintf(stderr, "\n");
    bench_args_fprint(stderr, parms);

    fprintf(stderr, "\n");
    fprintf(stderr, "%-10s %11s %19s %22s\n", " ", "* put [ns/op]", "* get [ns/op]", "* remove [ns/op]");
    fprintf(stderr, "%6s%5s", "N", " ");
    for (int i = 0 ; i < 3 ; i++)
      fprintf(stderr, "%4s %-4s %-4s %-4s " , "cold", "med", "avg", "std");
    fprintf(stderr, "\n");
  }

  for (size_t n = 0 , nn = array_length(results) ; n < nn ; n++) {

    size_t errs = 0;

    for (size_t r = 0 ; r < parms->runs ; r++) {
      double timing = 0;
      size_t      N = results[n]->N;
#if defined(bench_ht_debug)
      int       ret = fprintf(stderr, "%s [N=%zu, run %2zu/%2zu] ",
                              parms->impl, results[n]->N, r, parms->runs);
#endif
      set_t       s = set(NULL);
      int     *keys;

#if defined(bench_ht_debug)
      bench_status("Generating keys");
#endif
      keys = bench_keys(N, parms->workload);


      elapsed_ns(&timing);
      for (size_t k = 0 ; k < N ; k++) {
#if defined(bench_ht_debug)
        if (0 == k%SAMPLE_STATS) bench_status_ext(&descr, "Put absent");
#endif
        set_insert(&s, keys[k]);
      }
      results[n][r].timing[0][PUT] = elapsed_ns(&timing) / N;

      set_stats(s, &descr);
      results[n][r].abpkv = descr.stats.abpkv; /* Mem overhead after inserts */

      errs = 0;
      elapsed_ns(&timing);
      for (size_t k = 0 ; k < N ; k++) {
#if defined(bench_ht_debug)
        if (0 == k%SAMPLE_STATS) bench_status_ext(&descr, "Get present");
#endif
        errs += !set_member(s, keys[k]).status;
      }
      results[n][r].timing[1][GET] = elapsed_ns(&timing) / N;
      if (errs) ERROR("%zu keys not retrieved\n", errs);


      errs = 0;
      elapsed_ns(&timing);
      for (size_t k = N ; k < 2*N ; k++) {
#if defined(bench_ht_debug)
        if (0 == k%SAMPLE_STATS) bench_status_ext(&descr, "Get absent");
#endif
        errs += set_member(s, keys[k]).status;
      }
      results[n][r].timing[0][GET] = elapsed_ns(&timing) / N;
      if (errs) ERROR("%zu absent keys found\n", errs);


      elapsed_ns(&timing);
      for (size_t k = 0 ; k < N ; k++) {
#if defined(bench_ht_debug)
        if (0 == k%SAMPLE_STATS) bench_status_ext(&descr, "Put present");
#endif
        set_insert(&s, keys[k]);
      }
      results[n][r].timing[1][PUT] = elapsed_ns(&timing) / N;


      elapsed_ns(&timing);
      for (size_t k = N ; k < 2*N ; k++) {
#if defined(bench_ht_debug)
        if (0 == k%SAMPLE_STATS) bench_status_ext(&descr, "Remove absent");
#endif
        set_remove(&s, keys[k]);
      }
      results[n][r].timing[0][RM] = elapsed_ns(&timing) / N;


      elapsed_ns(&timing);
      for (size_t k = 0 ; k < N ; k++) {
#if defined(bench_ht_debug)
        if (0 == k%SAMPLE_STATS) bench_status_ext(&descr, "Remove present");
#endif
        set_remove(&s, keys[k]);
      }
      results[n][r].timing[1][RM] = elapsed_ns(&timing) / N;


#if defined(bench_ht_debug)
      bench_status("Deleting keys");
#endif
      array_delete(&keys);

#if defined(bench_ht_debug)
      fprintf(stderr, "\r%80s\r", " ");
#endif
      set_delete(&s);
    }

    if (parms->verbose) {
      for (int present = 0 ; present < 2 ; present++) {
        fprintf(stderr, "%8zu %c ", results[n]->N, !present?' ':'X');
        for (int op = 0 ; op < 3 ; op++) {
          double cold, med, avg, std;
          int   nchrs;

          bench_run_stat(results[n], present, op, &cold, &avg, &med, &std);

          disp_timing(cold);
          disp_timing(med);
          disp_timing(avg);
          disp_timing(std);
        }

        fprintf(stderr, "\n");
      }
      fprintf(stderr, "\n");
    }
  }

  bench_results_fprint(stdout, results);

  bench_results_delete(&results);

  return 0;
}

#undef res_width
#undef bench_status_ext
#undef bench_status
#undef disp_timing
#undef SAMPLE_STATS

int main(int argc, char *argv[]) {
  bench_args_t parms = bench_args_parse(argc, argv);

  if (parms.status) exit(EXIT_FAILURE);

  set_bench(&parms);

  exit(EXIT_SUCCESS);
}

#undef benchmark
#undef set_bench

#undef set
#undef set_t
#undef set_stats
#undef set_stat
#undef set_delete
#undef set_clone
#undef set_fprint
#undef set_length
#undef set_insert
#undef set_member
#undef set_is_empty
#undef set_union
#undef set_inter
#undef set_diff
#undef set_symdiff
