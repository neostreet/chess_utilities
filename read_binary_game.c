#include <stdio.h>
#include <string.h>

#define MAX_FILENAME_LEN 1024

static char bin_filename[MAX_FILENAME_LEN];

#include "chess.h"
#define MAKE_GLOBALS_HERE
#include "chess.glb"
#include "chess.fun"
#include "chess.mac"

static char usage[] =
"usage: read_binary_game (-debug) filename\n";

static struct game curr_game;

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

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

  retval = read_binary_game(argv[1],&curr_game);

  if (retval) {
    printf("read_binary_game of %s failed: %d\n",argv[1],retval);
    return 3;
  }

  printf("%s\n",argv[1]);
  printf("orientation = %d\n",curr_game.orientation);
  printf("num_moves = %d\n",curr_game.num_moves);

  return 0;
}
