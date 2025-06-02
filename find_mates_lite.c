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
"usage: find_mates_lite (-debug) (-no_preceding_mate_in_one) (-with_preceding_check) (-mine) (-not_mine)\n"
"  (-i_am_white) (-i_am_black) (-miniature) filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bDebug;
  bool bNoPrecedingMateInOne;
  bool bWithPrecedingCheck;
  bool bMine;
  bool bNotMine;
  bool bIAmWhite;
  bool bIAmBlack;
  bool bMiniature;
  int retval;
  FILE *fptr;
  int filename_len;
  struct game curr_game;

  if ((argc < 2) || (argc > 10)) {
    printf(usage);
    return 1;
  }

  bDebug = false;
  bNoPrecedingMateInOne = false;
  bWithPrecedingCheck = false;
  bMine = false;
  bNotMine = false;
  bIAmWhite = false;
  bIAmBlack = false;
  bMiniature = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = true;
    else if (!strcmp(argv[curr_arg],"-no_preceding_mate_in_one"))
      bNoPrecedingMateInOne = true;
    else if (!strcmp(argv[curr_arg],"-with_preceding_check"))
      bWithPrecedingCheck = true;
    else if (!strcmp(argv[curr_arg],"-mine"))
      bMine = true;
    else if (!strcmp(argv[curr_arg],"-not_mine"))
      bNotMine = true;
    else if (!strcmp(argv[curr_arg],"-i_am_white"))
      bIAmWhite = true;
    else if (!strcmp(argv[curr_arg],"-i_am_black"))
      bIAmBlack = true;
    else if (!strcmp(argv[curr_arg],"-miniature"))
      bMiniature = true;
    else
      break;
  }

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  if (bMine && bNotMine) {
    printf("can't specify both -mine and -not_mine\n");
    return 3;
  }

  if (bIAmWhite && bIAmBlack) {
    printf("can't specify both -i_am_white and -i_am_black\n");
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

    retval = read_binary_game(filename,&curr_game);

    if (retval) {
      printf("read_binary_game of %s failed: %d\n",filename,retval);
      printf("curr_move = %d\n",curr_game.curr_move);

      continue;
    }

    if (bIAmWhite && curr_game.orientation)
      continue;

    if (bIAmBlack && !curr_game.orientation)
      continue;

    if (bMiniature && (curr_game.num_moves > 40))
      continue;

    if (curr_game.moves[curr_game.num_moves-1].special_move_info & SPECIAL_MOVE_MATE) {
      if (bMine) {
        if (curr_game.num_moves % 2) {
          if (!curr_game.orientation)
            ;
          else
            continue;
        }
        else {
          if (curr_game.orientation)
            ;
          else
            continue;
        }
      }
      else if (bNotMine) {
        if (curr_game.num_moves % 2) {
          if (curr_game.orientation)
            ;
          else
            continue;
        }
        else {
          if (!curr_game.orientation)
            ;
          else
            continue;
        }
      }

      if (bWithPrecedingCheck) {
        if (curr_game.num_moves >= 3) {
          if (curr_game.moves[curr_game.num_moves-3].special_move_info & SPECIAL_MOVE_CHECK) {
            printf("%s\n",filename);
          }
        }
      }
      else if (!bNoPrecedingMateInOne) {
        printf("%s\n",filename);
      }
      else {
        if (curr_game.num_moves >= 2) {
          if (!(curr_game.moves[curr_game.num_moves-2].special_move_info & SPECIAL_MOVE_MATE_IN_ONE)) {
            printf("%s\n",filename);
          }
        }
      }
    }
  }

  return 0;
}
