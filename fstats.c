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
"usage: fstats (-debug) (-binary_format) (-white) (-black) filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bDebug;
  bool bBinaryFormat;
  bool bWhite;
  bool bBlack;
  int retval;
  FILE *fptr;
  int filename_len;
  struct game curr_game;
  int wins;
  int losses;
  int draws;

  if ((argc < 2) || (argc > 6)) {
    printf(usage);
    return 1;
  }

  bDebug = false;
  bBinaryFormat = false;
  bWhite = false;
  bBlack = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = true;
    else if (!strcmp(argv[curr_arg],"-binary_format"))
      bBinaryFormat = true;
    else if (!strcmp(argv[curr_arg],"-white"))
      bWhite = true;
    else if (!strcmp(argv[curr_arg],"-black"))
      bBlack = true;
    else
      break;
  }

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  if (bWhite && bBlack) {
    printf("can't specify both -white and -black\n");
    return 3;
  }

  if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    return 4;
  }

  wins = 0;
  losses = 0;
  draws = 0;

  for ( ; ; ) {
    GetLine(fptr,filename,&filename_len,MAX_FILENAME_LEN);

    if (feof(fptr))
      break;

    bzero(&curr_game,sizeof (struct game));

    if (!bBinaryFormat) {
      retval = read_game(filename,&curr_game,err_msg);

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

    if (bWhite) {
      if (!curr_game.orientation) {
        if (curr_game.result == WHITE_WIN)
          wins++;
        else if (curr_game.result == DRAW)
          draws++;
        else if (curr_game.result == BLACK_WIN)
          losses++;
      }
    }
    else if (bBlack) {
      if (curr_game.orientation) {
        if (curr_game.result == BLACK_WIN)
          wins++;
        else if (curr_game.result == DRAW)
          draws++;
        else if (curr_game.result == WHITE_WIN)
          losses++;
      }
    }
    else if ((!curr_game.orientation && (curr_game.result == WHITE_WIN)) ||
        (curr_game.orientation && (curr_game.result == BLACK_WIN))) {

      wins++;
    }
    else if (curr_game.result == DRAW) {
      draws++;
    }
    else if ((!curr_game.orientation && (curr_game.result == BLACK_WIN)) ||
        (curr_game.orientation && (curr_game.result == WHITE_WIN))) {

      losses++;
    }
  }

  fclose(fptr);

  printf("%d %d %d\n",wins,draws,losses);

  return 0;
}
