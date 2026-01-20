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
"usage: find_resignations (-i_am_white) (-i_am_black) (-before_move) (-after_move) [mine | opponent] filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bIAmWhite;
  bool bIAmBlack;
  bool bBeforeMove;
  bool bAfterMove;
  bool bMine;
  int retval;
  FILE *fptr;
  int filename_len;
  struct game curr_game;

  if ((argc < 3) || (argc > 7)) {
    printf(usage);
    return 1;
  }

  bIAmWhite = false;
  bIAmBlack = false;
  bBeforeMove = false;
  bAfterMove = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-i_am_white"))
      bIAmWhite = true;
    else if (!strcmp(argv[curr_arg],"-i_am_black"))
      bIAmBlack = true;
    else if (!strcmp(argv[curr_arg],"-before_move"))
      bBeforeMove = true;
    else if (!strcmp(argv[curr_arg],"-after_move"))
      bAfterMove = true;
    else
      break;
  }

  if (argc - curr_arg != 2) {
    printf(usage);
    return 2;
  }

  if (bIAmWhite and bIAmBlack) {
    printf("can't specify both -i_am_white and -i_am_black\n");
    return 3;
  }

  if (bBeforeMove && bAfterMove) {
    printf("can't specify both -before_move and -after_move\n");
    return 4;
  }

  if (!strcmp(argv[curr_arg],"mine"))
    bMine = true;
  else if (!strcmp(argv[curr_arg],"opponent"))
    bMine = false;
  else  {
    printf(usage);
    return 5;
  }

  if ((fptr = fopen(argv[curr_arg + 1],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg + 1]);
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

    if (curr_game.moves[curr_game.num_moves-1].special_move_info & SPECIAL_MOVE_MATE)
      continue;

    if (curr_game.time_forfeit)
      continue;

    if (curr_game.result == RESULT_DRAW)
      continue;

    if (bMine) {
      if (curr_game.result == RESULT_WIN)
          continue;
    }
    else {
      if (curr_game.result == RESULT_LOSS)
          continue;
    }

    if (bIAmWhite && curr_game.orientation)
      continue;

    if (bIAmBlack && !curr_game.orientation)
      continue;

    if (bBeforeMove) {
      if (bMine) {
        if (!curr_game.orientation) {
          if (curr_game.num_moves % 2)
            continue;
        }
        else {
          if (!(curr_game.num_moves % 2))
            continue;
        }
      }
      else {
        if (curr_game.orientation) {
          if (curr_game.num_moves % 2)
            continue;
        }
        else {
          if (!(curr_game.num_moves % 2))
            continue;
        }
      }
    }
    else if (bAfterMove) {
      if (bMine) {
        if (curr_game.orientation) {
          if (curr_game.num_moves % 2)
            continue;
        }
        else {
          if (!(curr_game.num_moves % 2))
            continue;
        }
      }
      else {
        if (!curr_game.orientation) {
          if (curr_game.num_moves % 2)
            continue;
        }
        else {
          if (!(curr_game.num_moves % 2))
            continue;
        }
      }
    }

    printf("%s\n",filename);
  }

  fclose(fptr);

  return 0;
}
