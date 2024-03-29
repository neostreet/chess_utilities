#include <stdio.h>

#include "chess.h"
#define MAKE_GLOBALS_HERE
#include "chess.glb"
#include "chess.fun"
#include "chess.mac"

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

int main(int argc,char **argv)
{
  int m;
  int n;
  int capture_count;
  int retval;
  struct game curr_game;

  if (argc < 2) {
    printf("usage: capture_count filename (filename ...)\n");
    return 1;
  }

  for (n = 1; n < argc; n++) {
    retval = read_game(argv[n],&curr_game,err_msg);

    if (retval) {
      printf("read_game of %s failed: %d\n",argv[n],retval);
      printf("curr_move = %d\n",curr_game.curr_move);
      continue;
    }

    capture_count = 0;

    for (m = 0; m < curr_game.num_moves; m++) {
      if (curr_game.moves[m].special_move_info & SPECIAL_MOVE_CAPTURE)
        capture_count++;
    }

    printf("%2d %s\n",capture_count,argv[n]);
  }

  return 0;
}
