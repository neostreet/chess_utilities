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
"usage: find_rook_and_pawn_endgames (-debug) (-binary_format) filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

bool rook_and_pawn_endgame(struct game *gamept);

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bDebug;
  bool bBinaryFormat;
  int retval;
  FILE *fptr;
  int filename_len;
  struct game curr_game;

  if ((argc < 2) || (argc > 4)) {
    printf(usage);
    return 1;
  }

  bDebug = false;
  bBinaryFormat = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = true;
    else if (!strcmp(argv[curr_arg],"-binary_format"))
      bBinaryFormat = true;
    else
      break;
  }

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    return 3;
  }

  for ( ; ; ) {
    GetLine(fptr,filename,&filename_len,MAX_FILENAME_LEN);

    if (feof(fptr))
      break;

    bzero(&curr_game,sizeof (struct game));

    if (!bBinaryFormat) {
      retval = read_game(filename,&curr_game,err_msg);

      if (retval) {
        printf("read_game of %s failed: %d\n",filename,retval);
        printf("curr_move = %d\n",curr_game.curr_move);

        continue;
      }
    }
    else {
      retval = read_binary_game(filename,&curr_game);

      if (retval) {
        printf("read_binary_game of %s failed: %d\n",filename,retval);
        printf("curr_move = %d\n",curr_game.curr_move);

        continue;
      }
    }

    set_initial_board(&curr_game);

    for (curr_game.curr_move = 0; curr_game.curr_move < curr_game.num_moves; curr_game.curr_move++) {
      update_board(&curr_game,NULL,NULL,false);
      update_piece_info(&curr_game);

      if (rook_and_pawn_endgame(&curr_game)) {
        printf("%s\n",filename);
        break;
      }
    }
  }

  fclose(fptr);

  return 0;
}

bool rook_and_pawn_endgame(struct game *gamept)
{
  int n;

  // must have at least one White rook
  if ((gamept->white_pieces[0].current_board_position == -1) && (gamept->white_pieces[7].current_board_position == -1))
    return false;

  // can't have either White knight
  if ((gamept->white_pieces[1].current_board_position != -1) || (gamept->white_pieces[6].current_board_position != -1))
    return false;

  // can't have either White bishop
  if ((gamept->white_pieces[2].current_board_position != -1) || (gamept->white_pieces[5].current_board_position != -1))
    return false;

  // can't have the White queen
  if (gamept->white_pieces[3].current_board_position != -1)
    return false;

  // must have at least one White pawn
  for (n = 8; n <= 15; n++) {
    if (gamept->white_pieces[n].current_board_position != -1)
      break;
  }

  if (n == 16)
    return false;

  // must have at least one Black rook
  if ((gamept->black_pieces[8].current_board_position == -1) && (gamept->black_pieces[15].current_board_position == -1))
    return false;

  // can't have either Black knight
  if ((gamept->black_pieces[9].current_board_position != -1) || (gamept->black_pieces[14].current_board_position != -1))
    return false;

  // can't have either Black bishop
  if ((gamept->black_pieces[10].current_board_position != -1) || (gamept->black_pieces[13].current_board_position != -1))
    return false;

  // can't have the Black queen
  if (gamept->black_pieces[11].current_board_position != -1)
    return false;

  // must have at least one Black pawn
  for (n = 0; n <= 7; n++) {
    if (gamept->black_pieces[n].current_board_position != -1)
      break;
  }

  if (n == 8)
    return false;

  return true;
}
