#include <stdio.h>
#include <string.h>

#include "chess.h"
#define MAKE_GLOBALS_HERE
#include "chess.glb"
#include "chess.fun"
#include "chess.mac"

static char usage[] =
"usage: capture_count (-mine) (-opponent) filename (filename ...)\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

int main(int argc,char **argv)
{
  int m;
  int n;
  int curr_arg;
  bool bMine;
  bool bOpponent;
  int capture_count;
  int retval;
  struct game curr_game;

  bMine = false;
  bOpponent = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-mine"))
      bMine = true;
    else if (!strcmp(argv[curr_arg],"-opponent"))
      bOpponent = true;
    else
      break;
  }

  if (argc - curr_arg < 1) {
    printf(usage);
    return 1;
  }

  if (bMine && bOpponent) {
    printf("can't specify both -mine and -opponent\n");
    return 2;
  }

  for (n = curr_arg; n < argc; n++) {
    retval = read_game(argv[n],&curr_game);

    if (retval) {
      printf("read_game of %s failed: %d\n",argv[n],retval);
      printf("curr_move = %d\n",curr_game.curr_move);
      continue;
    }

    capture_count = 0;

    for (m = 0; m < curr_game.num_moves; m++) {
      if (bMine) {
        if (!curr_game.orientation) {
          if (m % 2)
            continue;
        }
        else {
          if (!(m % 2))
            continue;
        }
      }
      else if (bOpponent) {
        if (!curr_game.orientation) {
          if (!(m % 2))
            continue;
        }
        else {
          if (m % 2)
            continue;
        }
      }

      if (curr_game.moves[m].special_move_info & SPECIAL_MOVE_CAPTURE)
        capture_count++;
    }

    printf("%2d %s\n",capture_count,argv[n]);
  }

  return 0;
}
