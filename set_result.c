#include <stdio.h>
#include <string.h>

#include "chess.h"
#define MAKE_GLOBALS_HERE
#include "chess.glb"
#include "chess.fun"
#include "chess.mac"

static char usage[] =
"usage: set_result result filename (filename ...)\n";

int main(int argc,char **argv)
{
  int curr_arg;
  int retval;
  struct game curr_game;
  int result;

  if (argc < 3) {
    printf(usage);
    return 1;
  }

  sscanf(argv[1],"%d",&result);

  for (curr_arg = 2; curr_arg < argc; curr_arg++) {
    retval = read_game(argv[curr_arg],&curr_game);

    if (retval) {
      printf("read_game of %s failed: %d\n",argv[curr_arg],retval);
      printf("curr_move = %d\n",curr_game.curr_move);

      return 2;
    }

    curr_game.result = result;

    retval = write_binary_game(argv[curr_arg],&curr_game);

    if (retval) {
      printf("write_binary_game of %s failed: %d\n",argv[curr_arg],retval);
      return 3;
    }
  }

  return 0;
}
