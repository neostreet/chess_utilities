#include <stdio.h>
#include <string.h>

#include "chess.h"
#define MAKE_GLOBALS_HERE
#include "chess.glb"
#include "chess.fun"
#include "chess.mac"

#define MAX_FILENAME_LEN 1024

static char pos_filename[MAX_FILENAME_LEN];

static char usage[] =
"usage: game_position_to_pos (-binary_format) filename\n";

static int build_pos_filename(
  char *game_filename,
  int game_filename_len,
  char *pos_filename,
  int max_filename_len);

int main(int argc,char **argv)
{
  int curr_arg;
  bool bBinaryFormat;
  int retval;
  struct game curr_game;
  struct game_position curr_position;
  int game_filename_len;

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

  curr_position.orientation = curr_game.orientation;
  copy_board(curr_game.board,curr_position.board);

  game_filename_len = strlen(argv[curr_arg]);

  retval = build_pos_filename(argv[curr_arg],game_filename_len,pos_filename,MAX_FILENAME_LEN);

  if (retval) {
    printf("build_pos_filename failed on %s: %d\n",argv[curr_arg],retval);
    return 5;
  }

  retval = write_game_position(pos_filename,&curr_position);

  if (retval) {
    printf("write_game_position of %s failed: %d\n",argv[curr_arg],retval);
    return 6;
  }

  return 0;
}

static int build_pos_filename(
  char *game_filename,
  int game_filename_len,
  char *pos_filename,
  int max_filename_len)
{
  int n;

  for (n = 0; n < game_filename_len; n++) {
    if (game_filename[n] == '.')
      break;
  }

  if (n == game_filename_len)
    return 1;

  if (n + 4 > max_filename_len - 1)
    return 2;

  strncpy(pos_filename,game_filename,n);
  strcpy(&pos_filename[n],".pos");

  return 0;
}
