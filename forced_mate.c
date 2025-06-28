#include <stdio.h>
#include <string.h>

#include "chess.h"
#define MAKE_GLOBALS_HERE
#include "chess.glb"
#include "chess.fun"
#include "chess.mac"

static char usage[] =
"usage: forced_mate (-debug) filename\n";

int main(int argc,char **argv)
{
  int curr_arg;
  bool bDebug;
  int retval;
  struct game curr_game;

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

  if (!(curr_game.moves[curr_game.num_moves-1].special_move_info & SPECIAL_MOVE_MATE))
    return 4;

  position_game(&curr_game,curr_game.num_moves - 2);

  legal_moves_count =  0;

  get_legal_moves(&curr_game,legal_moves,&legal_moves_count);

  if (legal_moves_count != 1)
    return 5;

  printf("%s\n",argv[curr_arg]);

  return 0;
}
