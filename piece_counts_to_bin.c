#include <vector>

using namespace std;

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
#endif

#include "chess.h"
#define MAKE_GLOBALS_HERE
#include "chess.glb"
#include "chess.fun"
#include "chess.mac"

static char usage[] =
"usage: piece_counts_to_bin infile outfile\n";

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

char couldnt_open[] = "couldn't open %s\n";

int write_piece_counts_to_binfile(int *piece_counts,char *filename);

int main(int argc,char **argv)
{
  FILE *fptr;
  int line_len;
  int piece_counts[NUM_PIECE_TYPES_0 * 2];

  if (argc != 3) {
    printf(usage);
    return 1;
  }

  if (!strcmp(argv[1],argv[2])) {
    printf("outfile must differ from infile\n");
    return 2;
  }

  if ((fptr = fopen(argv[1],"r")) == NULL) {
    printf(couldnt_open,argv[1]);
    return 3;
  }

  GetLine(fptr,line,&line_len,MAX_LINE_LEN);

  if (feof(fptr)) {
    printf("couldn't read first line of %s\n",argv[1]);
    fclose(fptr);
    return 4;
  }

  fclose(fptr);

  sscanf(line,"p %d r %d n %d b %d q %d k %d P %d R %d N %d B %d Q %d K %d",
    &piece_counts[0],
    &piece_counts[1],
    &piece_counts[2],
    &piece_counts[3],
    &piece_counts[4],
    &piece_counts[5],
    &piece_counts[6],
    &piece_counts[7],
    &piece_counts[8],
    &piece_counts[9],
    &piece_counts[10],
    &piece_counts[11]);

  write_piece_counts_to_binfile(piece_counts,argv[2]);

  return 0;
}

int write_piece_counts_to_binfile(int *piece_counts,char *filename)
{
  int fhndl;
  unsigned int bytes_to_write;
  unsigned int bytes_written;

  if ((fhndl = open(filename,O_CREAT | O_TRUNC | O_WRONLY | O_BINARY,
      S_IREAD | S_IWRITE)) == -1)
    return 1;

  bytes_to_write = (NUM_PIECE_TYPES_0 * 2) * sizeof (int);

  bytes_written = write(fhndl,(char *)piece_counts,bytes_to_write);

  if (bytes_written != bytes_to_write) {
    close(fhndl);
    return 2;
  }

  close(fhndl);

  return 0;
}
