#include <core/rval.h>

#include <stdio.h>

rval_t rval(int error, void *data
#if !defined(NDEBUG) && defined(DEBUG_RETFLOW)
            , const char *func, int line
#endif
            ) {

#if !defined(NDEBUG) && defined(DEBUG_RETFLOW)
  fprintf(stderr, "[%16s:%d]  return ", func, line);
  if (error!=0) fprintf(stderr, "ERROR(%d) ", error);
  fprintf(stderr, "\n");
#endif

  return (rval_t){.error=error, .data=data};
}
