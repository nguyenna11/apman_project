#include <ds/iterator.h>

#include <core/tagging.h>
#include <common/logging.h>

#include <ds/array.h>
#include <ds/map/common.h>

#include <assert.h>

typedef enum { I_CURR, I_STOP } map_fwd_parm_t;

int        map_iterator_debug(iterator_t *it, void *unused) {
  size_t *parm = (void*)it->on_stack;
  (void)unused;
  (void)parm; /* bc DEBUG undefined with codegen=release */

  DEBUG("  Map struct %p\n", it->ds);
  DEBUG("  Iteration info\n");
  DEBUG("    stop     %zu\n", parm[I_STOP]);
  DEBUG("    current  %zu\n", parm[I_CURR]);

  return 0;
}

iterator_t in(void *tagged) { /* Visit any known data structure */
  iterator_t  it = { 0 };
  void *untagged;

  if (!tagged) return it; /* NULL -> finished iteration */

  untagged = UNTAG(tagged);

  switch(typeof(tagged)) {
  case T_ARRAY:
    it = array_forward(untagged);
    break;
  case T_TABLE:
    it = ((map_header_t*)untagged)->iterate(untagged, 0, ((map_header_t*)untagged)->next);
    break;
  default:
    assert(0 && "Unusupported object type");
    break;
  }

  return it;
}

void       iterator_debug(iterator_t *iter) {
  (void)iter; /* bc DEBUG undefined with codegen=release */

  DEBUG("Iterator %p [status=%d]\n", iter, iter->status);
  DEBUG("  VTable\n");
  DEBUG("    next     %p\n", iter->next);
  DEBUG("    delete   %p\n", iter->delete);
  DEBUG("    debug    %p\n", iter->debug);

  if (iter->debug) iter->debug(iter, NULL);
}

/* Iterations to be passed around functions: stack -> heap */
iterator_t *iterator(iterator_t iter) {
  return memcpy(block(sizeof(iter)), &iter, sizeof(iter));
}

void        iterator_delete(iterator_t **iterp) {
  if (!iterp || !*iterp) return;

  if ((*iterp)->delete) (*iterp)->delete(*iterp, NULL);

  del(iterp);
}
