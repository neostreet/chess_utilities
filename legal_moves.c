#include <stdio.h>
#include <string.h>

#include "chess.h"
#define MAKE_GLOBALS_HERE
#include "chess.glb"
#include "chess.fun"
#include "chess.mac"

static char usage[] =
"usage: legal_moves (-debug) filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

int main(int argc,char **argv)
{
  int curr_arg;
  bool bDebug;
  int retval;
  struct game curr_game;
  bool bWhiteToMove;
  struct piece_info *info_pt;
  unsigned char board[CHARS_IN_BOARD];

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
    printf("read_game of %s failed: %d\n",argv[curr_arg],retval);
    printf("curr_move = %d\n",curr_game.curr_move);
    return 3;
  }

  bWhiteToMove = !(curr_game.num_moves % 2);

  if (bWhiteToMove)
    info_pt = curr_game.white_pieces;
  else
    info_pt = curr_game.black_pieces;

  print_piece_info2(info_pt); // for now
  putchar(0x0a);
  populate_board_from_piece_info(&curr_game,board);
  print_bd0(board,curr_game.orientation);

  return 0;
}
