#include <ds/array.h>
#include <math.h>

#include <core/tagging.h>

#include <assert.h>
#include <string.h>

#define array_dynamic(array) array_hdr(array)->prop.dynamic
#define array_stride(array)  array_hdr(array)->prop.stride

#include <common/logging.h>

void       *array__(size_t stride, size_t length, allocator_t *al) {
  size_t     allocd = length ? length : DS_ARRAY_ALLOC_MIN;
  array_hdr_t  *hdr = alloc(sizeof(*hdr) + allocd*stride, al);

  hdr->allocd       = allocd;
  hdr->length       = length;

  hdr->prop.dynamic = !hdr->length;
  hdr->prop.stride  = stride;

  return TAG(hdr+1, T_ARRAY); /* TAG() slightly overkill but stick to conventions */
}

void       *array_clone(void *tagged) {

  array_hdr_t * oldhdr = array_hdr(tagged);
  size_t allocd = oldhdr->allocd;
  size_t stride = oldhdr->prop.stride;
  allocator_t * al = allocator_of(oldhdr);
  
  //create a new 
  array_hdr_t  * hdr = alloc(sizeof(*hdr) + allocd*stride, al);

  hdr->allocd = allocd;
  hdr->length = oldhdr->length;

  // hdr->prop.dynamic = oldhdr->prop.dynamic;
  // hdr->prop.stride = stride;
  hdr->prop=oldhdr->prop;

  void * datadest = (void *)(hdr + 1); //décalage vers la droite de la tailel du header.
  
  //copie de la mémoire pour les éléments stockés, on économise un peu pour des petites listes
  memcpy(datadest, tagged, stride*hdr->length); 
  return TAG(datadest, T_ARRAY); //TAGGING SHOULD BE HERE TOO
}

void        array_delete__(void **taggedp) {
  if (*taggedp == NULL){
    return;
  }

  if (typeof(taggedp)!=T_ARRAY){
    //TODO ERROR HERE
  }

  void * tagged = *taggedp;
  void * untagged = UNTAG(tagged);
  array_hdr_t * hdr = array_hdr(untagged);
  dealloc((void**)&hdr);
  *taggedp = NULL;
  return;
}

int         array_fprint(FILE *fp, void *tagged, int (*elem_fprint)(FILE *fp, const void *elemp)) {
  int nchars = 0;

  assert(fp && "Missing output FILE*");

  nchars += fprintf(fp, "[ ");

  for (size_t idx = 0, len = array_length(tagged), stride = len ? array_stride(tagged) : 0 ;
       idx < len ; idx++) {
    void *elemp = (char*)tagged + idx*stride;
    nchars += elem_fprint ? elem_fprint(fp, elemp) + fprintf(fp, " ") : fprintf(fp, "## ");
  }

  nchars += fprintf(fp, "]");

  return nchars;
}

size_t      array_resize(void **taggedp, size_t newlength) {
  array_hdr_t * hdr = array_hdr(*taggedp);
  size_t resize_size;
  size_t allocd = hdr->allocd;
  if (newlength>hdr->allocd){
    resize_size = (size_t) ceil(((double) allocd)*DS_ARRAY_ALLOC_GEOM);
  } else{
    hdr->length = newlength;
    return newlength;
  }

  size_t tmp_allocd =  hdr->allocd;
  allocator_t * al = allocator_of(hdr);

  //dynamic resize
  hdr->allocd = resize_size;
  void* newarray = array_clone(*taggedp); 
  hdr->allocd = tmp_allocd;
  
  array_hdr_t * newhdr = array_hdr(newarray);
  newhdr->length = newlength;

  array_delete(taggedp);
  *taggedp = newarray;
  return newlength;
}

size_t      array_push__(void **taggedp, void *obj, size_t obj_size) {
  size_t len = 0;
  
  if ( !*taggedp ) *taggedp = array__(obj_size, 0, NULL);

  assert(array_stride(*taggedp)==(int)obj_size && "Wrong object size");

  len = array_grow(taggedp);

  memcpy((len-1)*obj_size+(char*)*taggedp, obj, obj_size);

  return len;
}

size_t      array_pop__(void **taggedp, void *obj, size_t obj_size) {
  size_t len = taggedp ? array_length(*taggedp) : 0;

  assert(len>0 && "Uninitialized/empty array");
  assert(array_stride(*taggedp)==(int)obj_size && "Wrong object size");

  memcpy(obj,(len-1)*obj_size+(char*)*taggedp, obj_size);

  len = array_shrink(taggedp);

  if (0==len) array_delete(taggedp);

  return len;
}

iterator_t  array_search(void *tagged, void *obj, int (*compare)(const void *left, const void *right)) {
  iterator_t notfound = { 0 };

  for (size_t idx = 0, len = array_length(tagged), stride = len ? array_stride(tagged) : 0 ;
       idx < len ; idx++) {
    void *elemp = (char*)tagged + idx*stride;
    if (0 == compare(elemp, obj))
      return array_iter(tagged, idx, array_length(tagged), array_forward_next);
  }

  return notfound;
}

size_t      array_push_unique__(void **taggedp, void *obj, size_t obj_size, int (*compare)(const void *left, const void *right)) {
  iterator_t found;

  assert(taggedp && "Missing array address");

  found = array_search(*taggedp, obj, compare);

  if (found.status < 1)
    array_push__(taggedp, obj, obj_size);

  return array_length(*taggedp);
}

size_t      array_remove__(void **taggedp, void *obj, size_t obj_size, int all, int(*compare)(const void *left, const void *right)) {
  size_t    length;
  iterator_t found;

  assert(taggedp && !array_is_empty(*taggedp) && "Uninitialized/empty array");
  assert(obj && "Nothing to remove");
  assert(obj_size==(size_t)array_stride(*taggedp) && "Wrong object size");

  length = array_length(*taggedp);

  do {
    found = array_search(*taggedp, obj, compare);

    if (found.status > 0) {
      (void)next(&found, NULL);
      memmove(found.ds, found.ds+obj_size, (length-(((char*)found.ds-(char*)*taggedp)/obj_size))*obj_size);
      array_shrink(taggedp);
      length--;
#if defined(ds_secure_removal)
      memset((char*)*taggedp + obj_size*(length-1), 0, obj_size);
#endif
    }
  } while (found.ds && all);

  return length;
}


/*
 * Sorted arrays.
 */

int         array_is_sorted(void *tagged, int (*compare)(const void *left, const void *right)) {
  size_t   length = array_length(tagged);

  if (length>1) {
    size_t stride = array_stride(tagged);
    char    *left = tagged;
    char   *right = tagged + stride;
    length--;
    for (size_t i = 0 ; i < length ; left = right , right += stride , i++)
      if (0 < compare(left, right)) return 0;
  }

  return 1;
}

size_t      array_lower_bound(void *tagged, void *key, int (*compare)(const void *left, const void *right)) {
  size_t    low = 0;
  size_t   high = array_length(tagged);
  size_t stride = high ? array_stride(tagged) : 0;

  while (low < high) {
    size_t mid = low + (high - low) / 2;

    if (compare(mid*stride + (char*)tagged, key)<0)
      low  = mid + 1;
    else
      high = mid;
  }

  return low;
}

size_t      array_upper_bound(void *tagged, void *key, int (*compare)(const void *left, const void *right)) {
  size_t    low = 0;
  size_t   high = array_length(tagged);
  size_t stride = high ? array_stride(tagged) : 0;

  while (low < high) {
    size_t mid = low + (high - low) / 2;

    if (compare(mid*stride + (char*)tagged, key)<=0)
      low  = mid + 1;
    else
      high = mid;
  }

  return low;
}

iterator_t  array_bsearch(void *tagged, void *key, int (*compare)(const void *left, const void *right)) {
  size_t          pos;
  iterator_t notfound = { 0 };

  if (!tagged) return notfound;

  pos = array_lower_bound(tagged, key, compare);

  if (pos < array_length(tagged)) {
    void *found = array_stride(tagged)*pos + (char*)tagged;
    if (0 == compare(key, found))
      return array_iter(tagged, pos, array_length(tagged), array_forward_next);
  }

  return notfound;
}

size_t      array_sorted_push__(void **taggedp, void *obj, size_t obj_size, int(*compare)(const void *left, const void *right)) {
  size_t pos, length;

  assert(taggedp && "Missing array address");

  if (!*taggedp) *taggedp = array__(obj_size, 0, NULL);

  assert( (int)obj_size==array_stride(*taggedp) && "Wrong object size" );

  length = array_length(*taggedp);

  pos    = array_lower_bound(*taggedp, obj, compare);

  array_grow(taggedp);

  memmove((pos+1)*obj_size+(char*)*taggedp, pos*obj_size+(char*)*taggedp, (length-pos)*obj_size);

  memmove(pos*obj_size+(char*)*taggedp, obj, obj_size);

  return length+1;
}

size_t      array_sorted_push_unique__(void **taggedp, void *obj, size_t obj_size, int (*compare)(const void *left, const void *right)) {
  size_t pos, length;

  assert(taggedp && "Missing array address");

  if (!*taggedp) *taggedp = array__(obj_size, 0, NULL);

  assert( (int)obj_size==array_stride(*taggedp) && "Wrong object size" );

  length = array_length(*taggedp);

  pos    = array_lower_bound(*taggedp, obj, compare);

  if (pos == length || compare(obj, pos*obj_size+(char*)*taggedp)) {
    array_grow(taggedp);

    memmove((pos+1)*obj_size+(char*)*taggedp, pos*obj_size+(char*)*taggedp, (length-pos)*obj_size);

    memmove(pos*obj_size+(char*)*taggedp, obj, obj_size);

    length++;
  }

  return length;
}

size_t      array_sorted_remove__(void **taggedp, void *obj, size_t obj_size, int all, int(*compare)(const void *left, const void *right)) {
  iterator_t has_one;

  assert(taggedp && !array_is_empty(*taggedp) && "Uninitialized/empty array");
  assert(obj && "Nothing to remove");
  assert(obj_size==(size_t)array_stride(*taggedp) && "Wrong object size");

  has_one = array_bsearch(*taggedp, obj, compare);

  if (has_one.status) {
    int        found = !all;
    char *first_elem;

    (void)next(&has_one, NULL);
    first_elem = has_one.ds;

    for (char *elem = first_elem ; all ; elem = has_one.ds ) {
      if (compare(obj, elem)) break;
      else found++;
      if (!next(&has_one, NULL)) break;
    }

    memmove(first_elem, first_elem+obj_size*found,
            array_length(*taggedp)*obj_size+(char*)(*taggedp)-(first_elem+obj_size*found));

#if defined(ds_secure_removal)
    memset((char*)*taggedp + obj_size*(array_length(*taggedp)-found), 0, obj_size*found);
#endif

    array_resize(taggedp, array_length(*taggedp)-found);
  }

  return array_length(*taggedp);
}

/*
 * Iterators.
 */

typedef enum { I_CURR, I_STOP, I_STRIDE } array_forward_parm_t;

int array_forward_next(iterator_t *it, void *itervarp) {
  size_t *parm = (void*)it->on_stack;

  if (parm[I_CURR] >= parm[I_STOP]) return 0;

  parm[I_CURR]++;
  it->ds = (char*)it->ds + parm[I_STRIDE];

  if (itervarp)
    memcpy(itervarp, (char*)it->ds, parm[I_STRIDE]);

  return 1; /* Next iteration is ready */
}

int array_backward_next(iterator_t *it, void *itervarp) {
  size_t *parm = (void*)it->on_stack;

  if (parm[I_CURR] < parm[I_STOP]) return 0;

  parm[I_CURR]--;
  it->ds = (char*)it->ds - parm[I_STRIDE];

  if (itervarp)
    memcpy(itervarp, it->ds, parm[I_STRIDE]);

  return 1; /* Next iteration is ready */
}

static int array_iterator_debug(iterator_t *it, void *unused) {
  size_t *parm = (void*)it->on_stack;
  (void)unused;
  (void)parm; /* bc DEBUG undefined with codegen=release */

  DEBUG("  Array info\n");
  DEBUG("    stride   %zu\n", parm[I_STRIDE]);
  DEBUG("  %sward iteration info\n", it->next == array_forward_next? "For":"Back");
  DEBUG("    curslot  %p\n", it->ds);
  DEBUG("    stop     %zu\n", parm[I_STOP]);
  DEBUG("    current  %zu\n", parm[I_CURR]);

  return 0;
}

iterator_t  array_iter(void *_array, size_t start, size_t stop, iteration_vte_t iterate) {
  iterator_t  it = { 0 };
  size_t   *parm = (void*)it.on_stack;

  it.delete      = NULL;           /* We have no heap-allocated stuff to free */
  it.debug       = array_iterator_debug;
  it.next        = iterate;
  parm[I_CURR]   = start;
  parm[I_STOP]   = stop;

  if (0 == array_length(_array)) return it;

  parm[I_STRIDE] = array_stride(_array);
  it.ds          = (char*)_array + (start-(iterate==array_forward_next))*parm[I_STRIDE];

  if (array_forward_next == iterate) {
    if (parm[I_CURR] >= parm[I_STOP]) return it;
  }
  else {
    if (parm[I_CURR] <  parm[I_STOP]) return it;
  }

  it.status      = 1;

  return it;
}

#undef array_dynamic /* Clean macros namespace */
#undef array_stride
#undef array_allocd
