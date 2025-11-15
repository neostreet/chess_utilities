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
"usage: tabulate_games (-verbose) (-i_am_white) (-i_am_black) filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

int main(int argc,char **argv)
{
  int curr_arg;
  bool bVerbose;
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
  int wins_sum_elo_delta;
  int draws_sum_elo_delta;
  int losses_sum_elo_delta;
  double dwork;

  if ((argc < 2) || (argc > 5)) {
    printf(usage);
    return 1;
  }

  bVerbose = false;
  bIAmWhite = false;
  bIAmBlack = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
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

  if (argc - curr_arg != 1) {
    printf(usage);
    return 3;
  }

  if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    return 4;
  }

  wins = 0;
  draws = 0;
  losses = 0;
  wins_sum_elo_delta = 0;
  draws_sum_elo_delta = 0;
  losses_sum_elo_delta = 0;

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

    switch(curr_game.result) {
      case RESULT_WIN:
        wins++;
        wins_sum_elo_delta += curr_game.my_elo_delta;

        break;
      case RESULT_DRAW:
        draws++;
        draws_sum_elo_delta += curr_game.my_elo_delta;

        break;
      case RESULT_LOSS:
        losses++;
        losses_sum_elo_delta += curr_game.my_elo_delta;

        break;
    }
  }

  fclose(fptr);

  total_games = wins + draws + losses;

  if (!bVerbose) {
    printf("%5d wins\n",wins);
    printf("%5d draws\n",draws);
    printf("%5d losses\n",losses);
  }
  else {
    if (wins)
      dwork = (double)wins_sum_elo_delta / (double)wins;
    else
      dwork = (double)0;

    printf("%5d wins %d %lf\n",wins,wins_sum_elo_delta,dwork);

    if (draws)
      dwork = (double)draws_sum_elo_delta / (double)draws;
    else
      dwork = (double)0;

    printf("%5d draws %d %lf\n",draws,draws_sum_elo_delta,dwork);

    if (losses)
      dwork = (double)losses_sum_elo_delta / (double)losses;
    else
      dwork = (double)0;

    printf("%5d losses %d %lf\n",losses,losses_sum_elo_delta,dwork);
  }

  printf("\n%5d games\n",total_games);
  printf("\n%6.2lf%%\n",chess_win_pct(wins,draws,losses));

  return 0;
}
