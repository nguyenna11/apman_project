#include <core/tagging.h>

#include <stddef.h> /* NULL */
#include <assert.h>

#define CHECK_TYPE(T) case T: return #T

const char *typename(type_t T) {
  switch(T) {
    CHECK_TYPE(T_ARRAY);
    CHECK_TYPE(T_LIST);
    CHECK_TYPE(T_STACK);
    CHECK_TYPE(T_QUEUE);
    CHECK_TYPE(T_TABLE);
  default:
    assert(0 && "Wrong type_t");
    return NULL;
  }
}

#undef CHECK_TYPE /* Clean macros namespace */
