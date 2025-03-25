#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FILENAME_LEN 1024

static char bin_filename[MAX_FILENAME_LEN];

#include "chess.h"
#define MAKE_GLOBALS_HERE
#include "chess.glb"
#include "chess.fun"
#include "chess.mac"

static char usage[] =
"usage: ch2bin (-debug) (-skip_checks) (-ignore_read_errors) filename\n";

static struct game curr_game;

static int build_bin_filename(
  char *ch_filename,
  int ch_filename_len,
  char *bin_filename,
  int max_filename_len);

int main(int argc,char **argv)
{
  int curr_arg;
  bool bDebug;
  bool bIgnoreReadErrors;
  int ch_filename_len;
  int retval;

  if ((argc < 2) || (argc > 5)) {
    printf(usage);
    return 1;
  }

  bDebug = false;
  bSkipChecks = false;
  bIgnoreReadErrors = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = true;
    else if (!strcmp(argv[curr_arg],"-skip_checks"))
      bSkipChecks = true;
    else if (!strcmp(argv[curr_arg],"-ignore_read_errors"))
      bIgnoreReadErrors = true;
    else
      break;
  }

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  ch_filename_len = strlen(argv[curr_arg]);

  retval = build_bin_filename(argv[curr_arg],ch_filename_len,bin_filename,MAX_FILENAME_LEN);

  if (retval) {
    printf("build_bin_filename failed on %s: %d\n",argv[curr_arg],retval);
    return 3;
  }

  bzero(&curr_game,sizeof (struct game));

  retval = read_game(argv[curr_arg],&curr_game);

  if (!bIgnoreReadErrors) {
    if (retval) {
      printf("read_game of %s failed: %d\n",argv[curr_arg],retval);
      printf("curr_move = %d\n",curr_game.curr_move);
      return 4;
    }
  }

  retval = write_binary_game(bin_filename,&curr_game);

  if (retval) {
    printf("write_binary_game of %s failed: %d\n",bin_filename,retval);
    return 5;
  }

  return 0;
}

static int build_bin_filename(
  char *ch_filename,
  int ch_filename_len,
  char *bin_filename,
  int max_filename_len)
{
  int n;

  for (n = 0; n < ch_filename_len; n++) {
    if (ch_filename[n] == '.')
      break;
  }

  if (n == ch_filename_len)
    return 1;

  if (n + 4 > max_filename_len - 1)
    return 2;

  strncpy(bin_filename,ch_filename,n);
  strcpy(&bin_filename[n],".bin");

  return 0;
}
