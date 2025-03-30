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
"usage: tabulate_eco_games (-binary_format) (-i_am_white) (-i_am_black) eco filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

double chess_win_pct(int wins,int draws,int losses);

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bBinaryFormat;
  bool bIAmWhite;
  bool bIAmBlack;
  int retval;
  FILE *fptr;
  int filename_len;
  struct game curr_game;
  int wins;
  int draws;
  int losses;
  int total_games;

  if ((argc < 3) || (argc > 6)) {
    printf(usage);
    return 1;
  }

  bBinaryFormat = false;
  bIAmWhite = false;
  bIAmBlack = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-binary_format"))
      bBinaryFormat = true;
    else if (!strcmp(argv[curr_arg],"-i_am_white"))
      bIAmWhite = true;
    else if (!strcmp(argv[curr_arg],"-i_am_black"))
      bIAmBlack = true;
    else
      break;
  }

  if (bIAmWhite and bIAmBlack) {
    printf("can't specify both -i_am_white and -i_am_black\n");
    return 2;
  }

  if (argc - curr_arg != 2) {
    printf(usage);
    return 3;
  }

  if ((fptr = fopen(argv[curr_arg+1],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg+1]);
    return 4;
  }

  wins = 0;
  draws = 0;
  losses = 0;

  for ( ; ; ) {
    GetLine(fptr,filename,&filename_len,MAX_FILENAME_LEN);

    if (feof(fptr))
      break;

    bzero(&curr_game,sizeof (struct game));

    if (!bBinaryFormat) {
      retval = read_game(filename,&curr_game);

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

    if (bIAmWhite && curr_game.orientation)
      continue;

    if (bIAmBlack && !curr_game.orientation)
      continue;

    if (!strcmp(curr_game.eco,argv[curr_arg])) {
      switch(curr_game.result) {
        case RESULT_WIN:
          wins++;

          break;
        case RESULT_DRAW:
          draws++;

          break;
        case RESULT_LOSS:
          losses++;

          break;
      }
    }
  }

  fclose(fptr);

  total_games = wins + draws + losses;

  printf("%5d wins\n",wins);
  printf("%5d draws\n",draws);
  printf("%5d losses\n",losses);
  printf("\n%5d games\n",total_games);
  printf("\n%6.2lf%%\n",chess_win_pct(wins,draws,losses));

  return 0;
}

double chess_win_pct(int wins,int draws,int losses)
{
  int games;
  double win_pct;

  games = wins + draws + losses;

  win_pct = ((double)wins + ((double)draws / (double) 2)) / (double)games * (double)100;

  return win_pct;
}
