#include <core/fileutils.h>
#include <core/rval.h>
#include <core/mem.h>

#include <assert.h>
#include <stdio.h>
#include <sys/stat.h>

rval_t file_contents(char *filename, size_t *sizep) {
  FILE *fp;
  char *bytes;
  struct stat sb;

  assert(filename && "Missing filename");

  if (-1==stat(filename, &sb)) return ERR(errno);

  fp = fopen(filename, "rt");
  if (!fp) return ERR(errno);

  bytes = block(sb.st_size);

  for (size_t r=0, rr=0 ; r < (size_t)sb.st_size ; r += rr ) {
    rr = fread(bytes+r, 1, sb.st_size-r, fp);
    if (ferror(fp)) {
      del(&bytes);
      fclose(fp);
      return ERR(ENODATA);
    }
  }
  fclose(fp);
  if (sizep) *sizep = sb.st_size;
  return OK(bytes);
}
