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
"usage: fmost_moved_piece (-binary_format) (-i_am_white) (-i_am_black) [white | black] filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bBinaryFormat;
  bool bIAmWhite;
  bool bIAmBlack;
  int by_white;
  int retval;
  FILE *fptr;
  int filename_len;
  struct game curr_game;
  struct piece_info *info_pt;
  char original_piece_id;
  char original_piece_char;
  int max_move_count;
  int max_ix;

  if ((argc < 3) || (argc > 6)) {
    printf(usage);
    return 1;
  }

  bBinaryFormat = false;
  bIAmWhite = false;
  bIAmBlack = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-binary_format"))
      bBinaryFormat = true;
    else if (!strcmp(argv[curr_arg],"-i_am_white"))
      bIAmWhite = true;
    else if (!strcmp(argv[curr_arg],"-i_am_black"))
      bIAmBlack = true;
    else
      break;
  }

  if (argc - curr_arg != 2) {
    printf(usage);
    return 2;
  }

  if (bIAmWhite and bIAmBlack) {
    printf("can't specify both -i_am_white and -i_am_black\n");
    return 3;
  }

  by_white = -1;

  if (!strcmp(argv[curr_arg],"white"))
    by_white = 1;
  else if (!strcmp(argv[curr_arg],"black"))
    by_white = 0;

  if (by_white == -1) {
    printf(usage);
    return 4;
  }

  if ((fptr = fopen(argv[curr_arg+1],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg+1]);
    return 5;
  }

  for ( ; ; ) {
    GetLine(fptr,filename,&filename_len,MAX_FILENAME_LEN);

    if (feof(fptr))
      break;

    if (!bBinaryFormat) {
      retval = read_game(filename,&curr_game);

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

    if (bIAmWhite && curr_game.orientation)
      continue;

    if (bIAmBlack && !curr_game.orientation)
      continue;

    if (by_white)
      info_pt = curr_game.white_pieces;
    else
      info_pt = curr_game.black_pieces;

    max_move_count = 0;

    for (n = 0; n < NUM_PIECES_PER_PLAYER; n++) {
      if (info_pt[n].move_count > max_move_count) {
        max_move_count = info_pt[n].move_count;
        max_ix = n;
      }
    }

    if (max_move_count) {
      original_piece_id = info_pt[max_ix].original_piece_id;

      if (original_piece_id < 0)
        original_piece_id *= -1;

      if (by_white)
        original_piece_char = piece_ids2[original_piece_id - 1] + ('a' - 'A');
      else
        original_piece_char = piece_ids2[original_piece_id - 1];

      printf("%d %c %c%c %s\n",
        info_pt[max_ix].move_count,
        original_piece_char,
        'a' + FILE_OF(info_pt[max_ix].original_board_position),
        '1' + RANK_OF(info_pt[max_ix].original_board_position),
        filename);
    }
  }

  fclose(fptr);

  return 0;
}
