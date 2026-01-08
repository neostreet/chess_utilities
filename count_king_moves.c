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
"usage: count_king_moves (-verbose) (-i_am_white) (-i_am_black)\n"
"  (-only_wins) (-only_draws) (-only_losses) [white | black] filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bVerbose;
  bool bIAmWhite;
  bool bIAmBlack;
  bool bOnlyWins;
  bool bOnlyDraws;
  bool bOnlyLosses;
  int by_white;
  int retval;
  FILE *fptr;
  int filename_len;
  struct game curr_game;
  char result;

  if ((argc < 3) || (argc > 9)) {
    printf(usage);
    return 1;
  }

  bVerbose = false;
  bIAmWhite = false;
  bIAmBlack = false;
  bOnlyWins = false;
  bOnlyDraws = false;
  bOnlyLosses = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else if (!strcmp(argv[curr_arg],"-i_am_white"))
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

  if (argc - curr_arg != 2) {
    printf(usage);
    return 2;
  }

  if (bIAmWhite && bIAmBlack) {
    printf("can't specify both -i_am_white and -i_am_black\n");
    return 3;
  }

  if (bOnlyWins + bOnlyDraws + bOnlyLosses > 1) {
    printf("can only speciry one of -only_wins, -only_draws, and -only_losses\n");
    return 4;
  }

  by_white = -1;

  if (!strcmp(argv[curr_arg],"white"))
    by_white = 1;
  else if (!strcmp(argv[curr_arg],"black"))
    by_white = 0;

  if (by_white == -1) {
    printf(usage);
    return 5;
  }

  if ((fptr = fopen(argv[curr_arg+1],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg+1]);
    return 6;
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

    if (!bVerbose) {
      if (by_white)
        printf("%d %s\n",curr_game.white_pieces[4].move_count,filename);
      else
        printf("%d %s\n",curr_game.black_pieces[12].move_count,filename);
    }
    else {
      switch(curr_game.result) {
        case RESULT_WIN:
          result = 'W';

          break;
        case RESULT_DRAW:
          result = 'D';

          break;
        case RESULT_LOSS:
          result = 'L';

          break;
      }

      if (by_white) {
        printf("%d %c %s %s\n",curr_game.white_pieces[4].move_count,
          result,filename,curr_game.date);
      }
      else {
        printf("%d %c %s %s\n",curr_game.black_pieces[12].move_count,
          result,filename,curr_game.date);
      }
    }
  }

  fclose(fptr);

  return 0;
}
