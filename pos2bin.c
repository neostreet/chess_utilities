#include <stdio.h>
#include <string.h>

#define MAX_FILENAME_LEN 1024

static char bin_filename[MAX_FILENAME_LEN];

#include "chess.h"
#define MAKE_GLOBALS_HERE
#include "chess.glb"
#include "chess.fun"
#include "chess.mac"

static char usage[] =
"usage: pos2bin (-debug) filename\n";

static struct game_position curr_position;
static struct game curr_game;

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

static int build_bin_filename(
  char *pos_filename,
  int pos_filename_len,
  char *bin_filename,
  int max_filename_len);

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bDebug;
  int pos_filename_len;
  int retval;

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

  pos_filename_len = strlen(argv[curr_arg]);

  retval = build_bin_filename(argv[curr_arg],pos_filename_len,bin_filename,MAX_FILENAME_LEN);

  if (retval) {
    printf("build_bin_filename failed on %s: %d\n",argv[curr_arg],retval);
    return 3;
  }

  retval = read_game_position(argv[curr_arg],&curr_position);

  if (retval) {
    printf("read_game_position of %s failed: %d\n",argv[curr_arg],retval);
    return 4;
  }

  for (n = 0; n < CHARS_IN_BOARD; n++)
    custom_initial_board[n] = curr_position.board[n];

  curr_game.orientation = curr_position.orientation;
  curr_game.has_custom_initial_board = 1;

  retval = write_binary_game(bin_filename,&curr_game);

  if (retval) {
    printf("write_binary_game of %s failed: %d\n",bin_filename,retval);
    return 5;
  }

  return 0;
}

static int build_bin_filename(
  char *pos_filename,
  int pos_filename_len,
  char *bin_filename,
  int max_filename_len)
{
  int n;

  for (n = 0; n < pos_filename_len; n++) {
    if (pos_filename[n] == '.')
      break;
  }

  if (n == pos_filename_len)
    return 1;

  if (n + 4 > max_filename_len - 1)
    return 2;

  strncpy(bin_filename,pos_filename,n);
  strcpy(&bin_filename[n],".bin");

  return 0;
}
