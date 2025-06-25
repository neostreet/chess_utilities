#include <stdio.h>
#include <string.h>

#include "chess.h"
#define MAKE_GLOBALS_HERE
#include "chess.glb"
#include "chess.fun"
#include "chess.mac"

static char usage[] =
"usage: legal_moves (-debug) (-hex) (-move_numbers) (-go_to_movemove) filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

int main(int argc,char **argv)
{
  int curr_arg;
  bool bDebug;
  bool bHex;
  bool bMoveNumbers;
  bool bGoToMove;
  int go_to_move;
  int retval;
  struct game curr_game;

  if ((argc < 2) || (argc > 6)) {
    printf(usage);
    return 1;
  }

  bDebug = false;
  bHex = false;
  bMoveNumbers = false;
  bGoToMove = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = true;
    else if (!strcmp(argv[curr_arg],"-hex"))
      bHex = true;
    else if (!strcmp(argv[curr_arg],"-move_numbers"))
      bMoveNumbers = true;
    else if (!strncmp(argv[curr_arg],"-go_to_move",11)) {
      bGoToMove = true;
      sscanf(&argv[curr_arg][11],"%d",&go_to_move);
    }
    else
      break;
  }

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  retval = read_game(argv[curr_arg],&curr_game);

  if (retval) {
    printf("read_game of %s failed: %d\n",argv[curr_arg],retval);
    printf("curr_move = %d\n",curr_game.curr_move);

    return 3;
  }

  if (bGoToMove) {
    if (go_to_move < 0)
      go_to_move = 0;
    else if (go_to_move > curr_game.num_moves)
      go_to_move = curr_game.num_moves;

    position_game(&curr_game,go_to_move);
  }

  printf("%s, num_moves = %d, curr_move = %d\n",argv[curr_arg],curr_game.num_moves,curr_game.curr_move);
  putchar(0x0a);
  print_bd(&curr_game);
  putchar(0x0a);

  if (!(curr_game.curr_move % 2)) {
    printf("White to move\n");
    print_piece_info2(curr_game.white_pieces,true,true,true);
  }
  else {
    printf("Black to move\n");
    print_piece_info2(curr_game.black_pieces,false,true,true);
  }

  putchar(0x0a);

  legal_moves_count =  0;

  get_legal_moves(&curr_game,legal_moves,&legal_moves_count);

  printf("%d legal_moves:\n\n",legal_moves_count);

  print_moves(legal_moves,legal_moves_count,bHex,bMoveNumbers);

  return 0;
}
