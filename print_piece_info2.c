#include <stdio.h>
#include <string.h>

#include "chess.h"
#define MAKE_GLOBALS_HERE
#include "chess.glb"
#include "chess.fun"
#include "chess.mac"

static char usage[] =
"usage: print_piece_info2 (-debug) (-abbrev) (-only_remaining) (-raw) filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bDebug;
  bool bAbbrev;
  bool bOnlyRemaining;
  bool bRaw;
  int retval;
  struct game curr_game;

  if ((argc < 2) || (argc > 6)) {
    printf(usage);
    return 1;
  }

  bDebug = false;
  bAbbrev = false;
  bOnlyRemaining = false;
  bRaw = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = true;
    else if (!strcmp(argv[curr_arg],"-abbrev"))
      bAbbrev = true;
    else if (!strcmp(argv[curr_arg],"-only_remaining"))
      bOnlyRemaining = true;
    else if (!strcmp(argv[curr_arg],"-raw"))
      bRaw = true;
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

  printf("%s\n",argv[curr_arg]);
  putchar(0x0a);
  print_bd(&curr_game);
  putchar(0x0a);

  printf("White:\n");

  if (!bRaw)
    print_piece_info2(curr_game.white_pieces,true,bAbbrev,bOnlyRemaining);
  else {
    for (n = 0; n < NUM_PIECES_PER_PLAYER; n++) {
      printf("  %d %d %d %d %d\n",
        curr_game.white_pieces[n].piece_id,
        curr_game.white_pieces[n].current_board_position,
        curr_game.white_pieces[n].move_count,
        curr_game.white_pieces[n].original_piece_id,
        curr_game.white_pieces[n].original_board_position);
    }
  }

  printf("Black:\n");

  if (!bRaw)
    print_piece_info2(curr_game.black_pieces,false,bAbbrev,bOnlyRemaining);
  else {
    for (n = 0; n < NUM_PIECES_PER_PLAYER; n++) {
      printf("  %d %d %d %d %d\n",
        curr_game.black_pieces[n].piece_id,
        curr_game.black_pieces[n].current_board_position,
        curr_game.black_pieces[n].move_count,
        curr_game.black_pieces[n].original_piece_id,
        curr_game.black_pieces[n].original_board_position);
    }
  }

  return 0;
}
