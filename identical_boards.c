#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#ifndef CYGWIN
#define O_BINARY 0
#endif

#include "chess.h"
#define MAKE_GLOBALS_HERE
#include "chess.glb"
#include "chess.fun"
#include "chess.mac"

static char usage[] =
"usage: identical_boards (-debug) filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

static char malloc_failed[] = "malloc of %d bytes failed\n";
static char read_failed[] = "%s: read of %d bytes failed\n";

int main(int argc,char **argv)
{
  int m;
  int n;
  int o;
  int curr_arg;
  bool bDebug;
  struct stat statbuf;
  off_t mem_amount;
  char *filenames_buf;
  int fhndl;
  int bytes_to_io;
  char **filenames;
  unsigned char *boards;
  unsigned char *unique_boards;
  int *orientations;
  int num_files;
  int retval;
  struct game curr_game;
  int match;
  int filename_len;
  int successful_reads;

  if ((argc < 2) || (argc > 3)) {
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

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  if (stat(argv[curr_arg],&statbuf) == -1) {
    printf(couldnt_get_status,argv[curr_arg]);
    return 3;
  }

  mem_amount = (size_t)statbuf.st_size;

  if ((filenames_buf = (char *)malloc(mem_amount)) == NULL) {
    printf(malloc_failed,mem_amount);
    return 4;
  }

  if ((fhndl = open(argv[curr_arg],O_BINARY | O_RDONLY,0)) == -1) {
    printf(couldnt_open,argv[curr_arg]);
    free(filenames_buf);
    return 5;
  }

  bytes_to_io = (int)mem_amount;

  if (read(fhndl,filenames_buf,bytes_to_io) != bytes_to_io) {
    printf(read_failed,argv[1],bytes_to_io);
    free(filenames_buf);
    close(fhndl);
    return 6;
  }

  // count the number of files
  num_files = 0;

  for (n = 0; n < bytes_to_io; n++) {
    if (filenames_buf[n] == '\n') {
      num_files++;
      filenames_buf[n] = 0;
    }
  }

  mem_amount = (size_t)num_files * sizeof (char *);

  if ((filenames = (char **)malloc(mem_amount)) == NULL) {
    printf(malloc_failed,mem_amount);
    free(filenames_buf);
    close(fhndl);
    return 7;
  }

  mem_amount = (size_t)num_files * CHARS_IN_BOARD;

  if ((boards = (unsigned char *)malloc(mem_amount)) == NULL) {
    printf(malloc_failed,mem_amount);
    free(filenames);
    free(filenames_buf);
    close(fhndl);
    return 8;
  }

  if ((unique_boards = (unsigned char *)malloc(mem_amount)) == NULL) {
    printf(malloc_failed,mem_amount);
    free(boards);
    free(filenames);
    free(filenames_buf);
    close(fhndl);
    return 9;
  }

  mem_amount = (size_t)num_files * sizeof (int);

  if ((orientations = (int *)malloc(mem_amount)) == NULL) {
    printf(malloc_failed,mem_amount);
    free(unique_boards);
    free(boards);
    free(filenames);
    free(filenames_buf);
    close(fhndl);
    return 19;
  }

  filenames[0] = &filenames_buf[0];
  m = 0;

  for (n = 1; n < num_files; n++) {
    for ( ; m < bytes_to_io; m++) {
      if (!filenames_buf[m])
        break;
    }

    if (m < bytes_to_io) {
      m++;
      filenames[n] = &filenames_buf[m];
    }
    else {
      // should never happen
      printf("couldn't parse filenames\n");
      free(orientations);
      free(unique_boards);
      free(boards);
      free(filenames);
      free(filenames_buf);
      close(fhndl);
      return 11;
    }
  }

  successful_reads = 0;
  m = 0;

  for (n = 0; n < num_files; n++) {
    retval = read_game(filenames[n],&curr_game,err_msg);

    if (retval) {
      printf("read_game of %s failed: %d\n",filenames[n],retval);
      printf("curr_move = %d\n",curr_game.curr_move);
    }
    else {
      for (o = 0; o < CHARS_IN_BOARD; o++) {
        boards[m++] = curr_game.board[o];
      }

      orientations[n] = curr_game.orientation;

      successful_reads++;
    }
  }

  if (bDebug) {
    for (n = 0; n < num_files; n++) {
      printf("%s\n",filenames[n]);
      print_bd0(&boards[n * CHARS_IN_BOARD],orientations[n]);
    }

    printf("%d successful reads\n",successful_reads);
  }

  free(orientations);
  free(unique_boards);
  free(boards);
  free(filenames);
  free(filenames_buf);
  close(fhndl);

  return 0;
}
