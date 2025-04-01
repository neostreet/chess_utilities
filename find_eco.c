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

static char usage[] = "usage: find_eco (-debug) eco ecos_file\n";
static char couldnt_open[] = "couldn't open %s\n";
static char couldnt_get_status[] = "couldn't get status of %s\n";

int read_ecos(char *ecos_file,char **ecos_pt,int *num_ecos_pt);
int find_eco(char *eco,char *ecos,int num_ecos);

int main(int argc,char **argv)
{
  int m;
  int n;
  int p;
  int curr_arg;
  bool bDebug;
  char *ecos;
  int num_ecos;
  int retval;

  if ((argc < 3) || (argc > 4)) {
    printf(usage);
    return 1;
  }

  bDebug = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = true;
    else
      break;
  }

  if (argc - curr_arg != 2) {
    printf(usage);
    return 2;
  }

  retval = read_ecos(argv[curr_arg+1],&ecos,&num_ecos);

  if (retval) {
    printf("read_ecos() failed; retval = %d\n",retval);
    return 3;
  }

  retval = find_eco(argv[curr_arg],ecos,num_ecos);

  if (retval == -1) {
    printf("find_eco() failed for eco %s\n",argv[curr_arg]);
    return 4;
  }

  printf("the index of eco %s is %d\n",argv[curr_arg],retval);

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

int find_eco(char *eco,char *ecos,int num_ecos)
{
  int m;
  int n;
  int offset;

  for (n = 0, offset = 0; n < num_ecos; n++, offset += 3) {

    for (m = 0; m < 3; m++) {
      if (eco[m] != ecos[offset + m])
        break;
    }

    if (m == 3)
      return n;
  }

  return -1;
}
