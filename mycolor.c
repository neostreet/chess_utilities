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
"usage: mycolor -binary_format [black | white] filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bWhite;
  bool bBinaryFormat;
  int retval;
  FILE *fptr;
  int filename_len;
  struct game curr_game;

  if ((argc < 3) || (argc > 4)) {
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

  if (argc - curr_arg != 2) {
    printf(usage);
    return 2;
  }

  if (!strcmp(argv[curr_arg],"white"))
    bWhite = true;
  else if (!strcmp(argv[curr_arg],"black"))
    bWhite = false;

  if ((fptr = fopen(argv[curr_arg+1],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg+1]);
    return 2;
  }

  for ( ; ; ) {
    GetLine(fptr,filename,&filename_len,MAX_FILENAME_LEN);

    if (feof(fptr))
      break;

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

    if (bWhite) {
      if (!curr_game.orientation)
        printf("%s\n",filename);
    }
    else {
      if (curr_game.orientation)
        printf("%s\n",filename);
    }
  }

  fclose(fptr);

  return 0;
}
