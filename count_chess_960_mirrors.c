#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

static char usage[] = "usage: count_chess_960_mirrors baselines\n";

static char couldnt_open[] = "couldn't open %s\n";
static char couldnt_get_status[] = "couldn't get status of %s\n";

static char malloc_failed[] = "malloc of %d bytes failed\n";
static char read_failed[] = "%s: read of %d bytes failed\n";

#define NUM_BASELINES 960
#define NUM_FILES 8
char baselines[NUM_BASELINES][NUM_FILES+1];

int main(int argc,char **argv)
{
  int m;
  int n;
  struct stat statbuf;
  off_t mem_amount;
  char *in_buf;
  int fhndl;
  int bytes_to_io;
  char mirror[NUM_FILES+1];
  int mirror_count;
  int total_mirrors;

  if (argc != 2) {
    printf(usage);
    return 1;
  }

  if (stat(argv[1],&statbuf) == -1) {
    printf(couldnt_get_status,argv[1]);
    return 2;
  }

  mem_amount = (size_t)statbuf.st_size;

  if (mem_amount != sizeof (baselines)) {
    printf("baselines file has the wrong size\n");
    return 3;
  }

  if ((fhndl = open(argv[1],O_BINARY | O_RDONLY,0)) == -1) {
    printf(couldnt_open,argv[1]);
    return 4;
  }

  bytes_to_io = (int)mem_amount;

  if (read(fhndl,baselines,bytes_to_io) != bytes_to_io) {
    printf(read_failed,argv[1],bytes_to_io);
    close(fhndl);
    return 5;
  }

  // replace linefeeds with NULLs

  for (n = 0; n < NUM_BASELINES; n++)
    baselines[n][NUM_FILES] = 0;

  mirror[NUM_FILES] = 0;
  total_mirrors = 0;

  for (n = 0; n < NUM_BASELINES; n++) {
    for (m = 0; m < NUM_FILES; m++)
      mirror[m] = baselines[n][NUM_FILES - 1 - m];

    mirror_count = 0;

    for (m = 0; m < NUM_BASELINES; m++) {
      if (m == n)
        continue;

      if (!strcmp(mirror,baselines[m]))
        mirror_count++;
    }

    printf("%s %d\n",baselines[n],mirror_count);
    total_mirrors += mirror_count;
  }

  close(fhndl);

  printf("\n%d total mirrors\n",total_mirrors);

  return 0;
}
