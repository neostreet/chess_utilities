#include <stdio.h>
#include <string.h>

#include "chess.h"
#define MAKE_GLOBALS_HERE
#include "chess.glb"
#include "chess.fun"
#include "chess.mac"

#define MAX_FILENAME_LEN 256
static char filename[MAX_FILENAME_LEN];

static char usage[] =
"usage: fprint_space_counts (-binary_format) (-i_am_white) (-i_am_black) (-my_count_first) (-diff_first) filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bBinaryFormat;
  bool bIAmWhite;
  bool bIAmBlack;
  bool bMyCountFirst;
  bool bDiffFirst;
  int retval;
  FILE *fptr;
  int filename_len;
  struct game curr_game;
  int first_ix;
  int second_ix;

  if ((argc < 2) || (argc > 7)) {
    printf(usage);
    return 1;
  }

  bBinaryFormat = false;
  bIAmWhite = false;
  bIAmBlack = false;
  bMyCountFirst = false;
  bDiffFirst = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-binary_format"))
      bBinaryFormat = true;
    else if (!strcmp(argv[curr_arg],"-i_am_white"))
      bIAmWhite = true;
    else if (!strcmp(argv[curr_arg],"-i_am_black"))
      bIAmBlack = true;
    else if (!strcmp(argv[curr_arg],"-my_count_first"))
      bMyCountFirst = true;
    else if (!strcmp(argv[curr_arg],"-diff_first"))
      bDiffFirst = true;
    else
      break;
  }

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  if (bIAmWhite and bIAmBlack) {
    printf("can't specify both -i_am_white and -i_am_black\n");
    return 3;
  }

  if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    return 4;
  }

  for ( ; ; ) {
    GetLine(fptr,filename,&filename_len,MAX_FILENAME_LEN);

    if (feof(fptr))
      break;

    bzero(&curr_game,sizeof (struct game));

    if (!bBinaryFormat) {
      retval = read_game(filename,&curr_game);

      if (retval) {
        printf("read_game of %s failed: %d\n",filename,retval);
        printf("curr_move = %d\n",curr_game.curr_move);

        continue;
      }
    }
    else {
      retval = read_binary_game(filename,&curr_game);

      if (retval) {
        printf("read_binary_game of %s failed: %d\n",filename,retval);
        printf("curr_move = %d\n",curr_game.curr_move);

        continue;
      }
    }

    if (bIAmWhite && curr_game.orientation)
      continue;

    if (bIAmBlack && !curr_game.orientation)
      continue;

    if (!bMyCountFirst) {
      first_ix = WHITE;
      second_ix = BLACK;
    }
    else {
      if (!curr_game.orientation) {
        first_ix = WHITE;
        second_ix = BLACK;
      }
      else {
        first_ix = BLACK;
        second_ix = WHITE;
      }
    }

    if (!bDiffFirst) {
      printf("%d %d (%d) %s %s\n",
        seirawan_count[first_ix],
        seirawan_count[second_ix],
        seirawan_count[first_ix] - seirawan_count[second_ix],
        filename,
        curr_game.date);
    }
    else {
      printf("%d %d %d %s %s\n",
        seirawan_count[first_ix] - seirawan_count[second_ix],
        seirawan_count[first_ix],
        seirawan_count[second_ix],
        filename,
        curr_game.date);
    }
  }

  fclose(fptr);

  return 0;
}
