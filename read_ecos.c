#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef WIN32
#include <io.h>
#else
#include <unistd.h>
#ifndef CYGWIN
#define O_BINARY 0
#endif
#endif

static char usage[] = "usage: read_ecos (-verbose) ecos_file\n";
static char couldnt_open[] = "couldn't open %s\n";
static char couldnt_get_status[] = "couldn't get status of %s\n";

static char eco[4];

int read_ecos(char *ecos_file,char **ecos_pt,int *num_ecos_pt);

int main(int argc,char **argv)
{
  int m;
  int n;
  int p;
  int curr_arg;
  bool bVerbose;
  char *ecos;
  int num_ecos;
  int retval;

  if ((argc < 2) || (argc > 3)) {
    printf(usage);
    return 1;
  }

  bVerbose = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else
      break;
  }

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  retval = read_ecos(argv[curr_arg],&ecos,&num_ecos);

  if (retval) {
    printf("read_ecos() failed; retval = %d\n",retval);
    return 3;
  }

  printf("num_ecos = %d\n",num_ecos);

  if (bVerbose) {
    putchar(0x0a);
    eco[3] = 0;
    p = 0;

    for (n = 0; n < num_ecos; n++) {
      for (m = 0; m < 3; m++) {
        eco[m] = ecos[p++];
      }

      printf("%s\n",eco);
    }
  }

  free(ecos);

  return 0;
}

int read_ecos(char *ecos_file,char **ecos_pt,int *num_ecos_pt)
{
  struct stat statbuf;
  int bytes_to_read;
  int bytes_read;
  char *ecos;
  int fhndl;

  if (stat(ecos_file,&statbuf) == -1)
    return 1;

  bytes_to_read = (int)statbuf.st_size;

  if (bytes_to_read % 3)
    return 2;

  *num_ecos_pt = bytes_to_read / 3;

  if ((ecos = (char *)malloc(bytes_to_read)) == NULL)
    return 3;

  if ((fhndl = open(ecos_file,O_BINARY | O_RDONLY,0)) == -1) {
    free(ecos);
    return 4;
  }

  bytes_read = read(fhndl,ecos,bytes_to_read);

  if (bytes_read != bytes_to_read) {
    free(ecos);
    close(fhndl);
    return 5;
  }

  close(fhndl);

  *ecos_pt = ecos;

  return 0;
}
