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
"usage: fprint_force_diff_lite (-i_am_white) (-i_am_black)\n"
"  (-only_wins) (-only_draws) (-only_losses) filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

int main(int argc,char **argv)
{
  int curr_arg;
  bool bIAmWhite;
  bool bIAmBlack;
  bool bOnlyWins;
  bool bOnlyDraws;
  bool bOnlyLosses;
  int retval;
  FILE *fptr;
  int filename_len;
  struct game curr_game;
  int force_diff;

  if ((argc < 2) || (argc > 7)) {
    printf(usage);
    return 1;
  }

  bIAmWhite = false;
  bIAmBlack = false;
  bOnlyWins = false;
  bOnlyDraws = false;
  bOnlyLosses = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-i_am_white"))
      bIAmWhite = true;
    else if (!strcmp(argv[curr_arg],"-i_am_black"))
      bIAmBlack = true;
    else if (!strcmp(argv[curr_arg],"-only_wins"))
      bOnlyWins = true;
    else if (!strcmp(argv[curr_arg],"-only_draws"))
      bOnlyDraws = true;
    else if (!strcmp(argv[curr_arg],"-only_losses"))
      bOnlyLosses = true;
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

  if (bOnlyWins + bOnlyDraws + bOnlyLosses > 1) {
    printf("can only speciry one of -only_wins, -only_draws, and -only_losses\n");
    return 4;
  }

  if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    return 5;
  }

  for ( ; ; ) {
    GetLine(fptr,filename,&filename_len,MAX_FILENAME_LEN);

    if (feof(fptr))
      break;

    bzero(&curr_game,sizeof (struct game));

    retval = read_game(filename,&curr_game);

    if (retval) {
      printf("read_game of %s failed: %d\n",filename,retval);
      printf("curr_move = %d\n",curr_game.curr_move);

      continue;
    }

    if (bIAmWhite && curr_game.orientation)
      continue;

    if (bIAmBlack && !curr_game.orientation)
      continue;

    if (bOnlyWins) {
      if (curr_game.result != RESULT_WIN)
        continue;
    }
    else if (bOnlyDraws) {
      if (curr_game.result != RESULT_DRAW)
        continue;
    }
    else if (bOnlyLosses) {
      if (curr_game.result != RESULT_LOSS)
        continue;
    }

    if (!curr_game.orientation) {
      force_diff = force_count[WHITE] - force_count[BLACK];
      printf("%d (%d %d) %s %s\n",force_diff,force_count[WHITE],force_count[BLACK],filename,curr_game.date);
    }
    else {
      force_diff = force_count[BLACK] - force_count[WHITE];
      printf("%d (%d %d) %s %s\n",force_diff,force_count[BLACK],force_count[WHITE],filename,curr_game.date);
    }
  }

  fclose(fptr);

  return 0;
}
