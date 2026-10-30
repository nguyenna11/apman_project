#include <ds/map/common.h>

#include <assert.h>

int         mapdescr_fprint(FILE *fp, map_descr_t *descr) {
  int ret = 0;

  assert(fp && "Missing output FILE pointer");
  assert(descr && "Missing map description");

  ret += fprintf(fp, "Map: %s\n", descr->name ? descr->name : "*NONAME*");
  ret += fprintf(fp, "  Pointers      %d bits (internal)\n", descr->huge ? 64:32);
  ret += fprintf(fp, "  Secure        %s\n", descr->secure ? "Yes":"NO");
  ret += fprintf(fp, "  Keys          %sordered (%zu bytes)\n",
                 descr->elem_is_ordered ? "":"un", descr->size_of.key);
  ret += fprintf(fp, "  Values        ");
  if (descr->values) ret += fprintf(fp, "%zu bytes\n", descr->size_of.value);
  else               ret += fprintf(fp, "None (map is a set)\n");
  if (descr->size_of.key + descr->size_of.value != descr->size_of.mapping)
    ret += fprintf(fp, "  Mappings     %zu bytes\n", descr->size_of.mapping);

  ret += fprintf(fp, "  Backend       ");

  switch(descr->backend) {
  case MAP_BACKEND_ARRAY: ret += fprintf(fp, "Array\n");
    ret += fprintf(fp, "  Search        %s\n", descr->elem_is_ordered ? "O(log N)":"O(N)" );
    /* Nothing more to print for this backend */
    break;
  case MAP_BACKEND_TABLE: ret += fprintf(fp, "Hashtable\n");
    ret += fprintf(fp, "  Search        O(1) amortized\n");
    break;
  case MAP_BACKEND_NODES:
    switch (descr->descr.nodes.number) {
    case 0:
      ret += fprintf(fp, "List\n");
      ret += fprintf(fp, "  Search        O(N)\n");
      break;
    case 1:
      if (descr->elem_is_ordered) {
        ret += fprintf(fp, "%s\n", descr->descr.nodes.is_treap ? "BST" : "Treap");
        ret += fprintf(fp, "  Search        O(log N)\n");
      }
      else {
        ret += fprintf(fp, "Hashtrie (2^%zu)\n", descr->descr.nodes.number);
        ret += fprintf(fp, "  Search        O(log N)\n");
      }
      break;
    default:
      ret += fprintf(fp, "Hashtrie (2^%zu)\n", descr->descr.nodes.number);
      ret += fprintf(fp, "  Search        O(log N)\n");
      break;
    }
    break;
  default:
    fprintf(stderr, "Unsupported map backend (%d)\n", descr->backend);
    abort();
    break;
  }

  if (descr->uses_hash) {
    ret += fprintf(fp, "  Hash\n" );
    ret += fprintf(fp, "    Finalizer    ");
    switch(descr->hash.finalized) {
    case 0: ret += fprintf(fp, "  None\n");
      break;
    case 1: ret += fprintf(fp, "  Fibonacci\n");
      break;
    default: /* More finalizers here */
      fprintf(stderr, "Unknown hash finalizer (%d)\n", descr->hash.finalized);
      abort();
      break;
    }
    ret += fprintf(fp, "    Filtered       %s\n", descr->hash.filtered?"Yes":"No");
  }

  if (MAP_BACKEND_TABLE == descr->backend) {
    ret += fprintf(fp, "    Salted         %s\n", descr->descr.table.opt.salted?"Yes":"No");
    ret += fprintf(fp, "  Sizes         %s",
                   descr->descr.table.primes?"primes == 3 mod 4\n":"powers of 2 ");
    if (!descr->descr.table.primes)
      ret += fprintf(fp, "(%s)\n", descr->descr.table.fast_modulo?"bitmask":"modulo");

    ret += fprintf(fp, "  Load target   %.2lf-%.2lf\n",
                   descr->descr.table.lf.min, descr->descr.table.lf.max);

    ret += fprintf(fp, "  Addressing    ");
    switch(descr->descr.table.addressing) {
    case 0:
      ret += fprintf(fp, "chained %s\n", descr->descr.table.opt.mtf?"(+MTF)":"");
      break;
    case 1:
      ret += fprintf(fp, "quadratic\n");
      ret += fprintf(fp, "    Tombstones  Yes\n");
      break;
    case 2:
      ret += fprintf(fp, "linear %s\n", descr->descr.table.opt.rh?"(Robin-Hood)":"");
      ret += fprintf(fp, "    Tombstones  ");
      if (0 == descr->descr.table.opt.tombstones)
        ret += fprintf(fp, "None (backward-shift deletion)\n");
      else
        ret += fprintf(fp, "Yes %s\n", 1 == descr->descr.table.opt.tombstones? "(reused)":"");
      break;
    default: /* More fancy addressings here */
      fprintf(stderr, "Unknown addressing (%d)\n", descr->descr.table.addressing);
      abort();
    }
  }
  else if (MAP_BACKEND_NODES == descr->backend) {
    /* Treated above */
  }

  return ret;
}
