#include <stdio.h>
#include <string.h>

#include "chess.h"
#define MAKE_GLOBALS_HERE
#include "chess.glb"
#include "chess.fun"
#include "chess.mac"

static char usage[] = "usage: game_info (-debug) filename\n";

static struct game curr_game;

int main(int argc,char **argv)
{
  int curr_arg;
  bool bDebug;
  int retval;

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

  retval = read_game(argv[curr_arg],&curr_game);

  if (retval) {
    printf("read_game of %s failed: %d\n",argv[curr_arg],retval);
    printf("curr_move = %d\n",curr_game.curr_move);

    return 3;
  }

  printf("%s\n",argv[curr_arg]);
  printf("title = %s\n",curr_game.title);
  printf("orientation = %d\n",curr_game.orientation);
  printf("num_moves = %d\n",curr_game.num_moves);
  printf("curr_move = %d\n",curr_game.curr_move);

  return 0;
}
