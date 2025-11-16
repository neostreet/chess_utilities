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
"usage: tabulate_games_by_color (-binary_format) [white | black] filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

int main(int argc,char **argv)
{
  int curr_arg;
  bool bBinaryFormat;
  int by_white;
  int retval;
  FILE *fptr;
  int file_no;
  int dbg_file_no;
  int dbg;
  int filename_len;
  struct game curr_game;
  int wins;
  int draws;
  int losses;
  int total_games;

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

  by_white = -1;

  if (!strcmp(argv[curr_arg],"white"))
    by_white = 1;
  else if (!strcmp(argv[curr_arg],"black"))
    by_white = 0;

  if (by_white == -1) {
    printf(usage);
    return 3;
  }

  if ((fptr = fopen(argv[curr_arg+1],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg+1]);
    return 4;
  }

  file_no = 0;
  wins = 0;
  draws = 0;
  losses = 0;

  for ( ; ; ) {
    GetLine(fptr,filename,&filename_len,MAX_FILENAME_LEN);

    if (feof(fptr))
      break;

    file_no++;

    if (file_no == dbg_file_no)
      dbg = 1;

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

    if (curr_game.result == RESULT_DRAW)
      draws++;
    else if (!curr_game.orientation) {
      if (by_white) {
        if (curr_game.result == RESULT_WIN)
          wins++;
        else
          losses++;
      }
      else {
        if (curr_game.result == RESULT_WIN)
          losses++;
        else
          wins++;
      }
    }
    else {
      if (by_white) {
        if (curr_game.result == RESULT_WIN)
          losses++;
        else
          wins++;
      }
      else {
        if (curr_game.result == RESULT_WIN)
          wins++;
        else
          losses++;
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
