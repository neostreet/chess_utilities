#include <vector>

using namespace std;

#include <stdio.h>
#include <string.h>

#include "chess.h"
#define MAKE_GLOBALS_HERE
#include "chess.glb"
#include "chess.fun"
#include "chess.mac"

static char usage[] = "usage: print_force_diff (-debug) filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int n;
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

  retval = read_game(argv[argc-1],&curr_game,err_msg);

  if (retval) {
    printf("read_game of %s failed: %d\n",argv[argc-1],retval);
    printf("curr_move = %d\n",curr_game.curr_move);
    return 2;
  }

  set_initial_board(&curr_game);

  printf("%d\n",curr_game.force_count[WHITE] - curr_game.force_count[BLACK]);

  for (curr_game.curr_move = 0; curr_game.curr_move < curr_game.num_moves; curr_game.curr_move++) {
    update_board(&curr_game,false);
    refresh_force_count(&curr_game);
    printf("%d\n",curr_game.force_count[WHITE] - curr_game.force_count[BLACK]);
  }

  return 0;
}
