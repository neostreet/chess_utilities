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
"usage: mycolor [black | white] filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

int main(int argc,char **argv)
{
  int n;
  bool bWhite;
  int retval;
  FILE *fptr;
  int filename_len;
  struct game curr_game;

  if (argc != 3) {
    printf(usage);
    return 1;
  }

  if (!strcmp(argv[1],"white"))
    bWhite = true;
  else if (!strcmp(argv[1],"black"))
    bWhite = false;

  if ((fptr = fopen(argv[2],"r")) == NULL) {
    printf(couldnt_open,argv[2]);
    return 2;
  }

  for ( ; ; ) {
    GetLine(fptr,filename,&filename_len,MAX_FILENAME_LEN);

    if (feof(fptr))
      break;

    retval = read_game(filename,&curr_game,err_msg);

    if (retval) {
      printf("read_game of %s failed: %d\n",filename,retval);
      printf("curr_move = %d\n",curr_game.curr_move);
      continue;
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
