#include <stdio.h>
#include <string.h>

#include "chess.h"
#define MAKE_GLOBALS_HERE
#include "chess.glb"
#include "chess.fun"
#include "chess.mac"

static char usage[] =
"usage: board_match (-debug) (-bin) (-exact_match) board1_file board2_file\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

static char populate_err1[] =
"populate_board_from_board_file() failed for %s: %d\n";
static char populate_err2[] =
"populate_board_from_bin_board_file() failed for %s: %d\n";

static unsigned char board1[CHARS_IN_BOARD];
static unsigned char board2[CHARS_IN_BOARD];

void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bDebug;
  bool bBin;
  bool bExactMatch;
  int retval;
  int match;

  if ((argc < 3) || (argc > 6)) {
    printf(usage);
    return 1;
  }

  bDebug = false;
  bBin = false;
  bExactMatch = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = true;
    else if (!strcmp(argv[curr_arg],"-bin"))
      bBin = true;
    else if (!strcmp(argv[curr_arg],"-exact_match"))
      bExactMatch = true;
    else
      break;
  }

  if (argc - curr_arg != 2) {
    printf(usage);
    return 2;
  }

  if (!bBin) {
    retval = populate_board_from_board_file(board1,argv[curr_arg]);

    if (retval) {
      printf(populate_err1,argv[curr_arg],retval);
      return 3;
    }

    retval = populate_board_from_board_file(board2,argv[curr_arg+1]);

    if (retval) {
      printf(populate_err1,argv[curr_arg+1],retval);
      return 4;
    }
  }
  else {
    retval = populate_board_from_bin_board_file(board1,argv[curr_arg]);

    if (retval) {
      printf(populate_err2,argv[curr_arg],retval);
      return 5;
    }

    retval = populate_board_from_bin_board_file(board2,argv[curr_arg+1]);

    if (retval) {
      printf(populate_err2,argv[curr_arg+1],retval);
      return 6;
    }
  }

  match = match_board(board1,board2,bExactMatch);

  if (!match)
    printf("no match\n");
  else
    printf("match\n");

  return 0;
}
