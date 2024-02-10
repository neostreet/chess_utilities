#include <stdio.h>
#include <string.h>

#include "chess.h"
#define MAKE_GLOBALS_HERE
#include "chess.glb"
#include "chess.fun"
#include "chess.mac"

static char usage[] = "usage: print_bd0 orientation filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

static unsigned char board1[CHARS_IN_BOARD];

int main(int argc,char **argv)
{
  int retval;
  int orientation;

  if (argc != 3) {
    printf(usage);
    return 1;
  }

  sscanf(argv[1],"%d",&orientation);

  retval = populate_board_from_bin_board_file(board1,argv[2]);

  if (retval) {
    printf("populate_board_from_bin_board_file() failed on %s: %d\n",argv[2],retval);
    return 2;
  }

  print_bd0(board1,orientation);

  return 0;
}
