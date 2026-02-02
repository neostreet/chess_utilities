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
"usage: fresult (-my_wins) (-my_draws) (-my_losses) (-decode) (-points) (-plus_minus)\n"
"  (-geval) (-ltval) filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bMyWins;
  bool bMyDraws;
  bool bMyLosses;
  bool bDecode;
  bool bPoints;
  bool bPlusMinus;
  bool bGeVal;
  bool bLtVal;
  int geval;
  int ltval;
  bool bPrint;
  int plus_minus;
  int retval;
  FILE *fptr;
  int filename_len;
  struct game curr_game;
  bool bPrintedFilename;

  if ((argc < 2) || (argc > 10)) {
    printf(usage);
    return 1;
  }

  bMyWins = false;
  bMyDraws = false;
  bMyLosses = false;
  bDecode = false;
  bPoints = false;
  bPlusMinus = false;
  bGeVal = false;
  bLtVal = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-my_wins"))
      bMyWins = true;
    else if (!strcmp(argv[curr_arg],"-my_draws"))
      bMyDraws = true;
    else if (!strcmp(argv[curr_arg],"-my_losses"))
      bMyLosses = true;
    else if (!strcmp(argv[curr_arg],"-decode"))
      bDecode = true;
    else if (!strcmp(argv[curr_arg],"-points"))
      bPoints = true;
    else if (!strcmp(argv[curr_arg],"-plus_minus"))
      bPlusMinus = true;
    else if (!strncmp(argv[curr_arg],"-ge",3)) {
      sscanf(&argv[curr_arg][3],"%d",&geval);
      bGeVal = true;
    }
    else if (!strncmp(argv[curr_arg],"-lt",3)) {
      sscanf(&argv[curr_arg][3],"%d",&ltval);
      bLtVal = true;
    }
    else
      break;
  }

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  if (bMyWins + bMyDraws + bMyLosses > 1) {
    printf("can only specify one of the -my_* options\n");
    return 3;
  }

  if (bDecode +  bPoints + bPlusMinus > 1) {
    printf("can only specify one of -decode, -points, and -plus_minus\n");
    return 4;
  }

  if (bGeVal && bLtVal) {
    printf("can't specify both -ge and -lt\n");
    return 5;
  }

  if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    return 6;
  }

  if (bPlusMinus)
    plus_minus = 0;

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
    else if (bPlusMinus) {
      switch (curr_game.result) {
        case RESULT_WIN:
          plus_minus += 1;

          break;

        case RESULT_LOSS:
          plus_minus -= 1;

          break;
      }

      bPrint = true;

      if (bGeVal) {
        if (plus_minus < geval)
          bPrint = false;
      }
      else if (bLtVal) {
        if (plus_minus >= ltval)
          bPrint = false;
      }

      if (bPrint)
        printf("%d %s %s\n",plus_minus,filename,curr_game.date);
    }
    else
      printf("%d %s %s\n",curr_game.result,filename,curr_game.date);
  }

  fclose(fptr);

  return 0;
}
