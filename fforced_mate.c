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
"usage: fforced_mate (-i_am_white) (-i_am_black) (-mine) (-opponent) filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

int main(int argc,char **argv)
{
  int curr_arg;
  bool bIAmWhite;
  bool bIAmBlack;
  bool bMine;
  bool bOpponent;
  int retval;
  FILE *fptr;
  int filename_len;
  struct game curr_game;

  if ((argc < 2) || (argc > 6)) {
    printf(usage);
    return 1;
  }

  bIAmWhite = false;
  bIAmBlack = false;
  bMine = false;
  bOpponent = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-i_am_white"))
      bIAmWhite = true;
    else if (!strcmp(argv[curr_arg],"-i_am_black"))
      bIAmBlack = true;
    else if (!strcmp(argv[curr_arg],"-mine"))
      bMine = true;
    else if (!strcmp(argv[curr_arg],"-opponent"))
      bOpponent = true;
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

  if (bMine && bOpponent) {
    printf("can't specify both -mine and -opponent\n");
    return 4;
  }

  if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
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

    if (bMine) {
      if (!curr_game.orientation) {
        if (curr_game.num_moves % 2)
          ;
        else
          continue;
      }
      else {
        if (curr_game.num_moves % 2)
          continue;
        else
          ;
      }
    }
    else if (bOpponent) {
      if (!curr_game.orientation) {
        if (curr_game.num_moves % 2)
          continue;
        else
          ;
      }
      else {
        if (curr_game.num_moves % 2)
          ;
        else
          continue;
      }
    }

    if (!(curr_game.moves[curr_game.num_moves-1].special_move_info & SPECIAL_MOVE_MATE))
      continue;

    position_game(&curr_game,curr_game.num_moves - 2);

    legal_moves_count =  0;

    get_legal_moves(&curr_game,legal_moves,&legal_moves_count);

    if (legal_moves_count != 1)
      continue;

    printf("%s\n",filename);
  }

  fclose(fptr);

  return 0;
}
