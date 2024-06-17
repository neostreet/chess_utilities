#include <stdio.h>
#include <string.h>

#include "chess.h"
#define MAKE_GLOBALS_HERE
#include "chess.glb"
#include "chess.fun"
#include "chess.mac"

static char usage[] =
"usage: truncate_moves (-debug) num_moves infile outfile\n";

static struct game curr_game;

int main(int argc,char **argv)
{
  int curr_arg;
  int num_moves;
  bool bDebug;
  int retval;

  if ((argc < 4) || (argc > 5)) {
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

  if (argc - curr_arg != 3) {
    printf(usage);
    return 2;
  }

  sscanf(argv[curr_arg],"%d",&num_moves);

  bzero(&curr_game,sizeof (struct game));

  retval = read_binary_game(argv[curr_arg+1],&curr_game);

  if (retval) {
    printf("read_game of %s failed: %d\n",argv[curr_arg+1],retval);
    printf("curr_move = %d\n",curr_game.curr_move);
    return 3;
  }

  if (curr_game.num_moves < num_moves) {
    printf("can't truncate %s to %d moves, as it only contains %d\n",argv[curr_arg+1],num_moves,curr_game.num_moves);
    return 4;
  }

  curr_game.num_moves = num_moves;
  curr_game.curr_move = num_moves;

  retval = write_binary_game(argv[curr_arg+2],&curr_game);

  if (retval) {
    printf("write_binary_game of %s failed: %d\n",argv[curr_arg+2],retval);
    return 5;
  }

  return 0;
}
