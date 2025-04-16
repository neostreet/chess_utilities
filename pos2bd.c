#include <stdio.h>
#include <string.h>

#define MAX_FILENAME_LEN 1024

static char bd_filename[MAX_FILENAME_LEN];

#define MAX_LINE_LEN 8192
static char line[MAX_LINE_LEN];

#include "chess.h"
#define MAKE_GLOBALS_HERE
#include "chess.glb"
#include "chess.fun"
#include "chess.mac"

static char usage[] =
"usage: fbin2bd (-debug) filename\n";

static struct game_position position;

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

static int build_bd_filename(
  char *bin_filename,
  int bin_filename_len,
  char *bd_filename,
  int max_filename_len);

int main(int argc,char **argv)
{
  int curr_arg;
  bool bDebug;
  int bin_filename_len;
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

  bin_filename_len = strlen(argv[curr_arg]);

  retval = build_bd_filename(argv[curr_arg],bin_filename_len,bd_filename,MAX_FILENAME_LEN);

  if (retval) {
    printf("build_bd_filename failed on %s: %d\n",argv[curr_arg],retval);
    return 3;
  }

  retval = read_game_position(argv[curr_arg],&position);

  if (retval) {
    printf("read_game_position of %s failed: %d\n",argv[curr_arg],retval);
    return 4;
  }

  retval = write_board_to_binfile(position.board,bd_filename);

  if (retval) {
    printf("write_board_to_binfile of %s failed: %d\n",bd_filename,retval);
    return 5;
  }

  return 0;
}

static int build_bd_filename(
  char *bin_filename,
  int bin_filename_len,
  char *bd_filename,
  int max_filename_len)
{
  if (bin_filename_len + 3 >= max_filename_len)
    return 1;

  sprintf(bd_filename,"%s.bd",bin_filename);

  return 0;
}
