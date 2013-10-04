#include <vector>

using namespace std;

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
  int retval;
  struct game curr_game;

  if (argc != 2) {
    printf("usage: pr_from_to filename\n");
    return 1;
  }

  retval = read_game(argv[1],&curr_game,err_msg);

  if (retval) {
    printf("read_game of %s failed: %d\n",argv[1],retval);
    printf("curr_move = %d\n",curr_game.curr_move);
    return 2;
  }

  print_from_to(&curr_game);

  return 0;
}
