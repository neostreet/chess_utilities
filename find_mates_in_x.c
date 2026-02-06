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
"usage: find_mates_in_x (-mine) (-not_mine) (-i_am_white) (-i_am_black) (-exact) moves filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bMine;
  bool bNotMine;
  bool bIAmWhite;
  bool bIAmBlack;
  bool bExact;
  int moves;
  bool bFound;
  int ix;
  int retval;
  FILE *fptr;
  int filename_len;
  struct game curr_game;

  if ((argc < 3) || (argc > 8)) {
    printf(usage);
    return 1;
  }

  bMine = false;
  bNotMine = false;
  bIAmWhite = false;
  bIAmBlack = false;
  bExact = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-mine"))
      bMine = true;
    else if (!strcmp(argv[curr_arg],"-not_mine"))
      bNotMine = true;
    else if (!strcmp(argv[curr_arg],"-i_am_white"))
      bIAmWhite = true;
    else if (!strcmp(argv[curr_arg],"-i_am_black"))
      bIAmBlack = true;
    else if (!strcmp(argv[curr_arg],"-exact"))
      bExact = true;
    else
      break;
  }

  if (argc - curr_arg != 2) {
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

  sscanf(argv[curr_arg],"%d",&moves);

  if ((fptr = fopen(argv[curr_arg+1],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg+1]);
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

    bFound = false;

    if (curr_game.moves[curr_game.num_moves-1].special_move_info & SPECIAL_MOVE_MATE) {
      if (moves == 1)
        bFound = true;
      else {
        ix = curr_game.num_moves-3;

        for (n = 0; n < moves - 1; n++) {
          if (!(curr_game.moves[ix].special_move_info & SPECIAL_MOVE_CHECK))
            break;

          ix -= 2;
        }

        if (n == moves - 1)
          bFound = true;
      }

      if (bFound && bExact) {
        if (moves == 1) {
          if (curr_game.moves[curr_game.num_moves-3].special_move_info & SPECIAL_MOVE_CHECK)
            bFound = false;
        }
        else {
          if (curr_game.moves[ix].special_move_info & SPECIAL_MOVE_CHECK)
            bFound = false;
        }
      }
    }

    if (bFound) {
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

      printf("%s\n",filename);
    }
  }

  return 0;
}
