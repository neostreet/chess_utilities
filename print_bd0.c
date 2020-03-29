#include <vector>

using namespace std;

#include <stdio.h>
#include <string.h>

#include "chess.h"
#define MAKE_GLOBALS_HERE
#include "chess.glb"
#include "chess.fun"
#include "chess.mac"

static char usage[] = "usage: print_bd0 filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

static unsigned char board1[CHARS_IN_BOARD];

int main(int argc,char **argv)
{
  int retval;

  if (argc != 2 ) {
    printf(usage);
    return 1;
  }

  retval = populate_board_from_bin_board_file(board1,argv[1]);
  print_bd0(board1);

  return 0;
}
