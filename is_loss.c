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
"usage: is_loss (-i_am_white) (-i_am_black) (-terse_modemode) (-unexpected) (-unexpected_gap) filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

int main(int argc,char **argv)
{
  int curr_arg;
  bool bIAmWhite;
  bool bIAmBlack;
  int terse_mode;
  bool bUnexpected;
  int unexpected_gap;
  int retval;
  FILE *fptr;
  int filename_len;
  struct game curr_game;

  if ((argc < 2) || (argc > 7)) {
    printf(usage);
    return 1;
  }

  bIAmWhite = false;
  bIAmBlack = false;
  terse_mode = 0;
  bUnexpected = false;
  unexpected_gap = 30;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-i_am_white"))
      bIAmWhite = true;
    else if (!strcmp(argv[curr_arg],"-i_am_black"))
      bIAmBlack = true;
    else if (!strncmp(argv[curr_arg],"-terse_mode",11))
      sscanf(&argv[curr_arg][11],"%d",&terse_mode);
    else if (!strcmp(argv[curr_arg],"-unexpected"))
      bUnexpected = true;
    else if (!strncmp(argv[curr_arg],"-unexpected_gap",15))
      sscanf(&argv[curr_arg][15],"%d",&unexpected_gap);
    else
      break;
  }

  if (bIAmWhite and bIAmBlack) {
    printf("can't specify both -i_am_white and -i_am_black\n");
    return 2;
  }

  if (argc - curr_arg != 1) {
    printf(usage);
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

    if (bIAmWhite && curr_game.orientation)
      continue;

    if (bIAmBlack && !curr_game.orientation)
      continue;

    if (curr_game.result == RESULT_LOSS) {
      if (bUnexpected) {
        if (curr_game.my_elo_before - curr_game.opponent_elo_before < unexpected_gap)
          continue;
      }

      if (!terse_mode) {
        if (!bUnexpected)
          printf("1 %s %s\n",filename,curr_game.date);
        else {
          printf("%s %d (%d %d)\n",filename,
            curr_game.my_elo_before - curr_game.opponent_elo_before,
            curr_game.my_elo_before,curr_game.opponent_elo_before);
        }
      }
      else if (terse_mode == 1)
        printf("1\n");
      else if (terse_mode == 2)
        printf("%s\n",filename);
    }
    else {
      if (bUnexpected)
        continue;

      if (!terse_mode)
        printf("0 %s %s\n",filename,curr_game.date);
      else if (terse_mode == 1)
        printf("0\n");
    }
  }

  fclose(fptr);

  return 0;
}
