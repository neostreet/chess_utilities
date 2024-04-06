#include <stdio.h>
#include <string.h>

#include "chess.h"
#define MAKE_GLOBALS_HERE
#include "chess.glb"
#include "chess.fun"
#include "chess.mac"

#define MAX_FILENAME_LEN 256
static char filename[MAX_FILENAME_LEN];

static char usage[] =
"usage: count_checks (-verbose) filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bVerbose;
  int retval;
  FILE *fptr;
  int filename_len;
  struct game curr_game;
  bool bBlack;
  int num_white_checks;
  int num_black_checks;

  if ((argc < 2) || (argc > 3)) {
    printf(usage);
    return 1;
  }

  bVerbose = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else
      break;
  }

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  if ((fptr = fopen(argv[argc-1],"r")) == NULL) {
    printf(couldnt_open,argv[argc-1]);
    return 3;
  }

  for ( ; ; ) {
    GetLine(fptr,filename,&filename_len,MAX_FILENAME_LEN);

    if (feof(fptr))
      break;

    retval = read_game(filename,&curr_game,err_msg);

    if (retval) {
      printf("read_game of %s failed: %d\n",filename,retval);
      printf("curr_move = %d\n",curr_game.curr_move);
    }

    set_initial_board(&curr_game);

    num_white_checks = 0;
    num_black_checks = 0;

    for (curr_game.curr_move = 0; curr_game.curr_move < curr_game.num_moves; curr_game.curr_move++) {
      bBlack = curr_game.curr_move & 0x1;
      update_board(curr_game.board,&curr_game.moves[curr_game.curr_move],bBlack);

      if (curr_game.moves[curr_game.curr_move].special_move_info & SPECIAL_MOVE_CHECK) {
        if (bBlack)
          num_black_checks++;
        else
          num_white_checks++;
      }
    }

    if (bVerbose) {
      printf("%d white %d black %d total %s\n",
        num_white_checks,num_black_checks,num_white_checks + num_black_checks,filename);
    }
    else
      printf("%d %s\n",num_white_checks + num_black_checks,filename);
  }

  return 0;
}
