#include <stdio.h>
#include <string.h>

#include "chess.h"
#define MAKE_GLOBALS_HERE
#include "chess.glb"
#include "chess.fun"
#include "chess.mac"

static char usage[] =
"usage: elos (-terse) filename\n";

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bTerse;
  int retval;
  struct game curr_game;

  if ((argc < 2) || (argc > 3)) {
    printf(usage);
    return 1;
  }

  bTerse = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-terse"))
      bTerse = true;
    else
      break;
  }

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  bzero(&curr_game,sizeof (struct game));

  retval = read_binary_game(argv[curr_arg],&curr_game);

  if (retval) {
    printf("read_binary_game of %s failed: %d\n",argv[curr_arg],retval);
    printf("curr_move = %d\n",curr_game.curr_move);

    return 3;
  }

  if (!bTerse) {
    printf("%d %d %d %d %s\n",
      curr_game.my_elo_before,curr_game.my_elo_delta,
      curr_game.opponent_elo_before,curr_game.opponent_elo_delta,
      argv[curr_arg]);
  }
  else {
    printf("%d\n",curr_game.my_elo_before);
  }

  return 0;
}
