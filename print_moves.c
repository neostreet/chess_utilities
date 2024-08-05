#include <stdio.h>
#include <string.h>
#include "chess.h"
#define MAKE_GLOBALS_HERE
#include "chess.glb"
#include "chess.fun"
#include "chess.mac"

static char usage[] = "usage: print_moves (-debug) (-binary_format) (-hex) filename\n";

int main(int argc,char **argv)
{
  int curr_arg;
  bool bDebug;
  bool bBinaryFormat;
  bool bHex;
  int initial_move;
  int retval;
  struct game curr_game;

  if ((argc < 2) || (argc > 5)) {
    printf(usage);
    return 1;
  }

  bDebug = false;
  bBinaryFormat = false;
  bHex = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = true;
    else if (!strcmp(argv[curr_arg],"-binary_format"))
      bBinaryFormat = true;
    else if (!strcmp(argv[curr_arg],"-hex"))
      bHex = true;
    else
      break;
  }

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  if (!bBinaryFormat) {
    retval = read_game(argv[curr_arg],&curr_game,err_msg);

    if (retval) {
      printf("read_game of %s failed: %d\n",argv[curr_arg],retval);
      printf("curr_move = %d\n",curr_game.curr_move);

      return 3;
    }
  }
  else {
    retval = read_binary_game(argv[curr_arg],&curr_game);

    if (retval) {
      printf("read_binary_game of %s failed: %d\n",argv[curr_arg],retval);
      printf("curr_move = %d\n",curr_game.curr_move);

      return 4;
    }
  }

  print_moves(curr_game.moves,curr_game.num_moves,bHex);

  return 0;
}
