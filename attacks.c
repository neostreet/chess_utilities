#include <vector>

using namespace std;

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
"usage: attacks (-debug) filename square1 square2\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bDebug;
  int square1;
  int square2;
  int retval;
  FILE *fptr;
  int filename_len;
  struct game curr_game;

  if ((argc < 4) || (argc > 5)) {
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

  if (argc - curr_arg != 3) {
    printf(usage);
    return 2;
  }

  if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    return 3;
  }

  if ((square1 = calc_square(argv[curr_arg+1])) == -1) {
    printf("square1 is invalid\n");
    return 4;
  }

  if ((square2 = calc_square(argv[curr_arg+2])) == -1) {
    printf("square2 is invalid\n");
    return 5;
  }

  for ( ; ; ) {
    GetLine(fptr,filename,&filename_len,MAX_FILENAME_LEN);

    if (feof(fptr))
      break;

    retval = read_game(filename,&curr_game,err_msg);

    if (retval) {
      printf("read_game of %s failed: %d\n",filename,retval);
      printf("curr_move = %d\n",curr_game.curr_move);

      if (bDebug)
        printf("%d do_castle() failures\n",do_castle_failures);

      continue;
    }

    if (square_attacks_square(&curr_game,square1,square2)) {
      printf("%s\n",filename);
      print_bd(&curr_game);
    }
  }

  if (bDebug)
    printf("%d do_castle() successes\n",do_castle_successes);

  return 0;
}
