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
"usage: count_king_moves (-i_am_white) (-i_am_black) [white | black] filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bIAmWhite;
  bool bIAmBlack;
  int by_white;
  int retval;
  FILE *fptr;
  int filename_len;
  struct game curr_game;

  if ((argc < 3) || (argc > 5)) {
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

  if (argc - curr_arg != 2) {
    printf(usage);
    return 2;
  }

  if (bIAmWhite && bIAmBlack) {
    printf("can't specify both -i_am_white and -i_am_black\n");
    return 3;
  }

  by_white = -1;

  if (!strcmp(argv[curr_arg],"white"))
    by_white = 1;
  else if (!strcmp(argv[curr_arg],"black"))
    by_white = 0;

  if (by_white == -1) {
    printf(usage);
    return 4;
  }

  if ((fptr = fopen(argv[curr_arg+1],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg+1]);
    return 5;
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

    if (by_white)
      printf("%d %s\n",curr_game.white_pieces[4].move_count,filename);
    else
      printf("%d %s\n",curr_game.black_pieces[12].move_count,filename);
  }

  fclose(fptr);

  return 0;
}
