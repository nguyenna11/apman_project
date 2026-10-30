#include <core/mem.h>
#include <core/fileutils.h>

#include <unitest/unitest.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {

  size_t length;
  char *filename = argc>1 ? argv[1] : NULL;
  rval_t contents = file_contents(filename, &length);

  if (IS_ERR(contents)) {
    fprintf(stderr, "%s: FAILED (error %d).\n", filename, RCODE(contents));
    exit(RCODE(contents));
  }

  printf("%s: loaded %zu bytes.\n", filename, length);
  del(&RVAL(contents));

  exit(EXIT_SUCCESS);
}
