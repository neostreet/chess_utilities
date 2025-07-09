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
"usage: find_losses (-i_am_white) (-i_am_black) (-century) (-verbose) filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

int main(int argc,char **argv)
{
  int curr_arg;
  bool bIAmWhite;
  bool bIAmBlack;
  bool bCentury;
  bool bVerbose;
  int retval;
  FILE *fptr;
  int filename_len;
  struct game curr_game;

  if ((argc < 2) || (argc > 6)) {
    printf(usage);
    return 1;
  }

  bIAmWhite = false;
  bIAmBlack = false;
  bCentury = false;
  bVerbose = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-i_am_white"))
      bIAmWhite = true;
    else if (!strcmp(argv[curr_arg],"-i_am_black"))
      bIAmBlack = true;
    else if (!strcmp(argv[curr_arg],"-century"))
      bCentury = true;
    else if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else
      break;
  }

  if (bIAmWhite and bIAmBlack) {
    printf("can't specify both -i_am_white and -i_am_black\n");
    return 2;
  }

  if (argc - curr_arg != 1) {
    printf(usage);
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

    retval = read_binary_game(filename,&curr_game);

    if (retval) {
      printf("read_binary_game of %s failed: %d\n",filename,retval);
      printf("curr_move = %d\n",curr_game.curr_move);

      continue;
    }

    if (curr_game.result != RESULT_LOSS)
      continue;

    if (bIAmWhite && curr_game.orientation)
      continue;

    if (bIAmBlack && !curr_game.orientation)
      continue;

    if (bCentury) {
      if (curr_game.my_elo_before - curr_game.opponent_elo_before < 100)
        continue;
    }

    if (!bVerbose || !bCentury)
      printf("%s\n",filename);
    else {
      printf("%d (%d %d) %s\n",
        curr_game.my_elo_before - curr_game.opponent_elo_before,
        curr_game.my_elo_before,curr_game.opponent_elo_before,
        filename);
    }
  }

  fclose(fptr);

  return 0;
}
