#include <stdio.h>
#include <string.h>

#include "chess.h"
#define MAKE_GLOBALS_HERE
#include "chess.glb"
#include "chess.fun"
#include "chess.mac"

static char usage[] =
"usage: copybin (-debug) infile outfile\n";

static struct game curr_game;

int main(int argc,char **argv)
{
  int curr_arg;
  bool bDebug;
  int retval;

  if ((argc < 3) || (argc > 4)) {
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

  if (argc - curr_arg != 2) {
    printf(usage);
    return 2;
  }

  bzero(&curr_game,sizeof (struct game));

  retval = read_binary_game(argv[1],&curr_game);

  if (retval) {
    printf("read_game of %s failed: %d\n",argv[1],retval);
    printf("curr_move = %d\n",curr_game.curr_move);
    return 3;
  }

  retval = write_binary_game(argv[2],&curr_game);

  if (retval) {
    printf("write_binary_game of %s failed: %d\n",argv[2],retval);
    return 4;
  }

  return 0;
}
