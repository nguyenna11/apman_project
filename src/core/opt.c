#include <core/opt.h>

#include <common/logging.h>

#include <assert.h>

int           opt_flag(int argc, char *argv[], char *shrt, char *lng) {
  assert(2 == strlen(shrt) && '-' == shrt[0] && '-' != shrt[1] && "Invalid short opt");
  assert(2 <  strlen(lng)  && '-' ==  lng[0] && '-' ==  lng[1] && "Invalid long opt");

  for (int i = 1 ; i < argc ; i++)
    if (!strcmp(argv[i], shrt)) return i;
    else if (!strcmp(argv[i], lng)) return i;

  return 0;
}

char         *opt_string(int argc, char *argv[], char *shrt, char *lng) {
  int    pos = opt_flag(argc, argv, shrt, lng);
  return pos && argc >= pos+1 ? argv[pos + 1] : NULL;
}

static int    opt_type__(int argc, char *argv[], char *shrt, char *lng, char *fmt, void *out) {
  assert( fmt && "Missing arg format string");
  assert( out && "Missing output pointer");
  assert(shrt && 2 == strlen(shrt) && '-' == shrt[0] && '-' != shrt[1] && "Invalid short opt");
  assert( lng && 2 <  strlen(lng)  && '-' ==  lng[0] && '-' ==  lng[1] && "Invalid long opt");

  for (int i = 1 ; i < argc ; i++)
    if (!strcmp(argv[i], shrt) || !strcmp(argv[i], lng)) {
      if (argc < i+1) {
        ERROR("Missing arg for %s\n", lng);
        return -1;
      }
      if (0 == sscanf(argv[i+1], fmt, out)) {
        ERROR("Could not read %s arg from %s\n", lng+2, argv[i+1]);
        return -1;
      }
      return 1;
    }

  return 0;
}

#define opt_make_parser(T,fmt)                                           \
  int   opt_##T(int argc, char *argv[], char *shrt, char *lng, T *out) { \
    return opt_type__(argc, argv, shrt, lng, fmt, out);                  \
  }

opt_make_parser(char    , "%c" );
opt_make_parser(int     , "%d" );
opt_make_parser(size_t  , "%zu");
opt_make_parser(unsigned, "%u" );
opt_make_parser(float   , "%f" );
opt_make_parser(double  , "%lg");

#undef opt_make_parser
