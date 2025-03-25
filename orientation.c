#include <stdio.h>
#include <string.h>

#include "chess.h"
#define MAKE_GLOBALS_HERE
#include "chess.glb"
#include "chess.fun"
#include "chess.mac"

static char usage[] =
"usage: orientation (-binary_format) filename (filename ...)\n";

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bBinaryFormat;
  int retval;
  struct game curr_game;

  if (argc < 2) {
    printf(usage);
    return 1;
  }

  bBinaryFormat = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-binary_format"))
      bBinaryFormat = true;
    else
      break;
  }

  if (argc - curr_arg < 1) {
    printf(usage);
    return 2;
  }

  for ( ; curr_arg < argc; curr_arg++) {
    bzero(&curr_game,sizeof (struct game));

    if (!bBinaryFormat) {
      retval = read_game(argv[curr_arg],&curr_game);

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

    printf("%d %d %s\n",curr_game.orientation,curr_game.num_moves,argv[curr_arg]);
  }

  return 0;
}
