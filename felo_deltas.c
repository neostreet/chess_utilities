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
"usage: felo_deltas (-terse_modemode) (-opponent) filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  int terse_mode;
  bool bOpponent;
  int retval;
  FILE *fptr;
  int filename_len;
  struct game curr_game;
  int elo_delta;

  if ((argc < 2) || (argc > 4)) {
    printf(usage);
    return 1;
  }

  terse_mode = 0;
  bOpponent = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strncmp(argv[curr_arg],"-terse_mode",11))
      sscanf(&argv[curr_arg][11],"%d",&terse_mode);
    else if (!strcmp(argv[curr_arg],"-opponent"))
      bOpponent = true;
    else
      break;
  }

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
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

    retval = read_binary_game(filename,&curr_game);

    if (retval) {
      printf("read_binary_game of %s failed: %d\n",filename,retval);
      printf("curr_move = %d\n",curr_game.curr_move);

      continue;
    }

    if (!bOpponent)
      elo_delta = curr_game.my_elo_delta;
    else
      elo_delta = curr_game.opponent_elo_delta;

    if (!terse_mode)
      printf("%d %s %s\n",elo_delta,filename,curr_game.date);
    else if (terse_mode == 1)
      printf("%d\n",elo_delta);
    else if (terse_mode == 2)
      printf("%s\n",filename);
  }

  fclose(fptr);

  return 0;
}
