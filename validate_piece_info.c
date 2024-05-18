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
"usage: validate_piece_info (-verbose) (-show_matches) filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bVerbose;
  bool bShowMatches;
  int retval;
  FILE *fptr;
  int filename_len;
  struct game curr_game;
  bool bPrintedFilename;
  unsigned char board[CHARS_IN_BOARD];

  if ((argc < 2) || (argc > 4)) {
    printf(usage);
    return 1;
  }

  bVerbose = false;
  bShowMatches = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else if (!strcmp(argv[curr_arg],"-show_matches"))
      bShowMatches = true;
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

    bPrintedFilename = false;

    for (curr_game.curr_move = 0; curr_game.curr_move < curr_game.num_moves; curr_game.curr_move++) {
      update_board(&curr_game,NULL,NULL);
      update_piece_info(&curr_game);
      populate_board_from_piece_info(&curr_game,board);

      retval = compare_boards(curr_game.board,board);

      if ((!bShowMatches && !retval) || (bShowMatches && retval)) {
        if (!bPrintedFilename) {
          printf("%s\n",filename);
          bPrintedFilename = true;
        }

        if (!bShowMatches) {
          printf("boards differ on move %d\n",curr_game.curr_move);

          if (bVerbose) {
            print_bd0(curr_game.board,curr_game.orientation);
            print_bd0(board,curr_game.orientation);
          }
        }
        else
          printf("boards match on move %d\n",curr_game.curr_move);
      }
    }
  }

  return 0;
}
