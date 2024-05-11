#include <stdio.h>
#include <string.h>

#include "chess.h"
#define MAKE_GLOBALS_HERE
#include "chess.glb"
#include "chess.fun"
#include "chess.mac"

static char usage[] =
"usage: patchbin (-debug) infile move_number special_move_info outfile\n";

static struct game curr_game;

int main(int argc,char **argv)
{
  int curr_arg;
  int move_number;
  int special_move_info;
  bool bDebug;
  int retval;

  if ((argc < 5) || (argc > 6)) {
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

  if (argc - curr_arg != 4) {
    printf(usage);
    return 2;
  }

  sscanf(argv[curr_arg+1],"%d",&move_number);
  sscanf(argv[curr_arg+2],"%x",&special_move_info);

  bzero(&curr_game,sizeof (struct game));

  retval = read_binary_game(argv[curr_arg],&curr_game);

  if (retval) {
    printf("read_game of %s failed: %d\n",argv[curr_arg],retval);
    printf("curr_move = %d\n",curr_game.curr_move);
    return 3;
  }

  if (move_number >= curr_game.num_moves) {
    printf("invalid move number\n");
    return 4;
  }

  curr_game.moves[move_number].special_move_info = special_move_info;

  retval = write_binary_game(argv[curr_arg+3],&curr_game);

  if (retval) {
    printf("write_binary_game of %s failed: %d\n",argv[curr_arg+3],retval);
    return 5;
  }

  return 0;
}
