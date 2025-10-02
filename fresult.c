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
"usage: fresult (-decode) (-points) (-my_wins) (-my_draws) (-my_losses) filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bDecode;
  bool bMyWins;
  bool bMyDraws;
  bool bMyLosses;
  bool bPoints;
  int retval;
  FILE *fptr;
  int filename_len;
  struct game curr_game;
  bool bPrintedFilename;

  if ((argc < 2) || (argc > 7)) {
    printf(usage);
    return 1;
  }

  bDecode = false;
  bPoints = false;
  bMyWins = false;
  bMyDraws = false;
  bMyLosses = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-decode"))
      bDecode = true;
    else if (!strcmp(argv[curr_arg],"-points"))
      bPoints = true;
    else if (!strcmp(argv[curr_arg],"-my_wins"))
      bMyWins = true;
    else if (!strcmp(argv[curr_arg],"-my_draws"))
      bMyDraws = true;
    else if (!strcmp(argv[curr_arg],"-my_losses"))
      bMyLosses = true;
    else
      break;
  }

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  if ((bMyWins && bMyDraws) ||
      (bMyWins && bMyLosses) ||
      (bMyDraws && bMyLosses)) {

    printf("can only specify one of the -my_* options\n");
    return 3;
  }

  if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    return 4;
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

    if (bMyWins) {
      if (curr_game.result == RESULT_WIN)
        printf("%s %s\n",filename,curr_game.date);
    }
    else if (bMyDraws) {
      if (curr_game.result == RESULT_DRAW)
        printf("%s %s\n",filename,curr_game.date);
    }
    else if (bMyLosses) {
      if (curr_game.result == RESULT_LOSS)
        printf("%s %s\n",filename,curr_game.date);
    }
    else if (bDecode) {
      switch (curr_game.result) {
        case RESULT_WIN:
          printf("win %s %s\n",filename,curr_game.date);

          break;

        case RESULT_DRAW:
          printf("draw %s %s\n",filename,curr_game.date);

          break;

        case RESULT_LOSS:
          printf("loss %s %s\n",filename,curr_game.date);

          break;
      }
    }
    else if (bPoints) {
      switch (curr_game.result) {
        case RESULT_WIN:
          printf("1 %s %s\n",filename,curr_game.date);

          break;

        case RESULT_DRAW:
          printf(".5 %s %s\n",filename,curr_game.date);

          break;

        case RESULT_LOSS:
          printf("0 %s %s\n",filename,curr_game.date);

          break;
      }
    }
    else
      printf("%d %s %s\n",curr_game.result,filename,curr_game.date);
  }

  fclose(fptr);

  return 0;
}
