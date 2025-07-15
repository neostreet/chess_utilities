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
"usage: resignation_ratio (-i_am_white) (-i_am_black) filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bIAmWhite;
  bool bIAmBlack;
  int retval;
  FILE *fptr;
  int filename_len;
  struct game curr_game;
  int num_losses[2];
  int num_resignations[2];
  double resignation_ratio[2];

  if ((argc < 2) || (argc > 4)) {
    printf(usage);
    return 1;
  }

  bIAmWhite = false;
  bIAmBlack = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-i_am_white"))
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

  for (n = 0; n < 2; n++) {
    num_losses[n] = 0;
    num_resignations[n] = 0;
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

    if (bIAmWhite && curr_game.orientation)
      continue;

    if (bIAmBlack && !curr_game.orientation)
      continue;

    if (curr_game.result == RESULT_DRAW)
      continue;

    if (curr_game.result == RESULT_LOSS) {
      num_losses[0]++;

      if (curr_game.moves[curr_game.num_moves-1].special_move_info & SPECIAL_MOVE_MATE)
        continue;

      if (curr_game.time_forfeit)
        continue;

      num_resignations[0]++;
    }
    else if (curr_game.result == RESULT_WIN) {
      num_losses[1]++;

      if (curr_game.moves[curr_game.num_moves-1].special_move_info & SPECIAL_MOVE_MATE)
        continue;

      if (curr_game.time_forfeit)
        continue;

      num_resignations[1]++;
    }
  }

  fclose(fptr);

  for (n = 0; n < 2; n++) {
    if (num_losses[n]) {
      resignation_ratio[n] = (double)num_resignations[n] / (double)num_losses[n];

      printf("%s: %lf (%d %d)\n",(n ? "opponent" : "me"),
        resignation_ratio[n],num_resignations[n],num_losses[n]);
    }
  }

  return 0;
}
