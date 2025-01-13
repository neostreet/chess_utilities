#include <stdio.h>
#include <string.h>

#include "chess.h"
#define MAKE_GLOBALS_HERE
#include "chess.glb"
#include "chess.fun"
#include "chess.mac"

static char usage[] =
"usage: populate_piece_info_from_board (-binary_format) filename\n";

int main(int argc,char **argv)
{
  int curr_arg;
  bool bBinaryFormat;
  int retval;
  struct game curr_game;
  struct piece_info white_pieces[NUM_PIECES_PER_PLAYER];
  struct piece_info black_pieces[NUM_PIECES_PER_PLAYER];
  unsigned char board[CHARS_IN_BOARD];

  if ((argc < 2) || (argc > 3)) {
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

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  if (!bBinaryFormat) {
    retval = read_game(argv[curr_arg],&curr_game,err_msg);

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

  retval = populate_piece_info_from_board(curr_game.board,white_pieces,black_pieces);

  if (retval) {
    printf("populate_piece_info_from_board() failed\n");
    return 5;
  }

  populate_board_from_piece_info(white_pieces,black_pieces,board);

  retval = compare_boards(curr_game.board,board);

  if (retval != 1) {
    printf("compare_boards() failed\n");
    return 6;
  }

  printf("%s\n",argv[curr_arg]);
  putchar(0x0a);
  print_bd0(board,curr_game.orientation);
  putchar(0x0a);

  printf("White:\n");
  print_piece_info2(white_pieces,true,true,true);

  printf("Black:\n");
  print_piece_info2(black_pieces,false,true,true);

  return 0;
}
