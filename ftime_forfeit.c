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
"usage: ftime_forfeit (-win) (-draw) (-loss) (-result) filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bWin;
  bool bDraw;
  bool bLoss;
  bool bResult;
  int retval;
  FILE *fptr;
  int filename_len;
  struct game curr_game;

  if ((argc < 2) || (argc > 6)) {
    printf(usage);
    return 1;
  }

  bWin = false;
  bDraw = false;
  bLoss = false;
  bResult = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-win"))
      bWin = true;
    else if (!strcmp(argv[curr_arg],"-draw"))
      bDraw = true;
    else if (!strcmp(argv[curr_arg],"-loss"))
      bLoss = true;
    else if (!strcmp(argv[curr_arg],"-result"))
      bResult = true;
    else
      break;
  }

  if ((bWin && bDraw) || (bWin && bLoss) || (bDraw && bLoss)) {
    printf("can only specify one of -win, -draw, and -loss\n");
    return 2;
  }

  if (argc - curr_arg != 1) {
    printf(usage);
    return 3;
  }

  if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    return 3;
  }

  for ( ; ; ) {
    GetLine(fptr,filename,&filename_len,MAX_FILENAME_LEN);

    if (feof(fptr))
      break;

    bzero(&curr_game,sizeof (struct game));

    retval = read_game(filename,&curr_game);

    if (retval) {
      printf("read_game of %s failed: %d\n",filename,retval);
      printf("curr_move = %d\n",curr_game.curr_move);

      continue;
    }

    if (bWin) {
      if (curr_game.result != RESULT_WIN)
        continue;
    }
    else if (bDraw) {
      if (curr_game.result != RESULT_DRAW)
        continue;
    }
    else if (bLoss) {
      if (curr_game.result != RESULT_LOSS)
        continue;
    }

    if (curr_game.time_forfeit) {
      if (!bResult)
        printf("%s\n",filename);
      else
        printf("%d %s\n",curr_game.result,filename);
    }
  }

  fclose(fptr);

  return 0;
}
