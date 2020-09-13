#include <vector>

using namespace std;

#include <stdio.h>
#include <string.h>

#include "chess.h"
#define MAKE_GLOBALS_HERE
#include "chess.glb"
#include "chess.fun"
#include "chess.mac"

static char usage[] =
"usage: ch_skeleton (-debug) filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

static unsigned char match_board1[CHARS_IN_BOARD];

void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
void print_space_and_force(struct game *game_pt,bool bSpace,bool bForce);

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

  retval = read_game(argv[curr_arg],&curr_game,err_msg);

  if (retval) {
    printf("read_game of %s failed: %d\n",argv[argc-1],retval);
    printf("curr_move = %d\n",curr_game.curr_move);
    return 3;
  }

  return 0;
}
