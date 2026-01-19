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
"usage: tabulate_losses (-i_am_white) (-i_am_black) filename\n";

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
  int losses;
  int time_forfeits;
  int resignations;
  int mates;

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

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  if (bIAmWhite and bIAmBlack) {
    printf("can't specify both -i_am_white and -i_am_black\n");
    return 3;
  }

  if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    return 4;
  }

  losses = 0;
  time_forfeits = 0;
  resignations = 0;
  mates = 0;

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

    if (curr_game.result != RESULT_LOSS)
        continue;

    if (bIAmWhite && curr_game.orientation)
      continue;

    if (bIAmBlack && !curr_game.orientation)
      continue;

    losses++;

    if (curr_game.time_forfeit)
      time_forfeits++;
    else if (curr_game.moves[curr_game.num_moves-1].special_move_info & SPECIAL_MOVE_MATE)
      mates++;
    else
      resignations++;
  }

  fclose(fptr);

  printf("%6d time forfeits\n",time_forfeits);
  printf("%6d mates\n",mates);
  printf("%6d resignations\n",resignations);
  printf("\n%6d losses\n",losses);

  return 0;
}
