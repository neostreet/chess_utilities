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
"usage: promotions (-mine) (-not_mine) (-none) (-maskmask) filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bMine;
  bool bNotMine;
  bool bNone;
  int retval;
  FILE *fptr;
  int filename_len;
  struct game curr_game;
  int num_promotions;
  int total_num_promotions;
  int mask;

  if ((argc < 2) || (argc > 6)) {
    printf(usage);
    return 1;
  }

  bMine = false;
  bNotMine = false;
  bNone = false;
  mask = 0;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-mine"))
      bMine = true;
    else if (!strcmp(argv[curr_arg],"-not_mine"))
      bNotMine = true;
    else if (!strcmp(argv[curr_arg],"-none"))
      bNone = true;
    else if (!strncmp(argv[curr_arg],"-mask",5))
      sscanf(&argv[curr_arg][5],"%x",&mask);
    else
      break;
  }

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  if ((bMine && bNotMine) || (bMine && bNone) || (bNotMine && bNone)) {
    printf("can only specify one of -mine, -not_mine, and -none\n");
    return 3;
  }

  if ((fptr = fopen(argv[argc-1],"r")) == NULL) {
    printf(couldnt_open,argv[argc-1]);
    return 4;
  }

  if (!mask) {
    mask = SPECIAL_MOVE_PROMOTION_QUEEN | SPECIAL_MOVE_PROMOTION_ROOK |
      SPECIAL_MOVE_PROMOTION_KNIGHT | SPECIAL_MOVE_PROMOTION_BISHOP;
  }

  total_num_promotions = 0;

  for ( ; ; ) {
    GetLine(fptr,filename,&filename_len,MAX_FILENAME_LEN);

    if (feof(fptr))
      break;

    retval = read_game(filename,&curr_game);

    if (retval) {
      printf("read_game of %s failed: %d\n",filename,retval);
      printf("curr_move = %d\n",curr_game.curr_move);
      continue;
    }

    num_promotions = 0;

    for (curr_game.curr_move = 0; curr_game.curr_move < curr_game.num_moves; curr_game.curr_move++) {
      if (curr_game.moves[curr_game.curr_move].special_move_info & mask) {
        if (bMine) {
          if (curr_game.num_moves % 2) {
            if (!curr_game.orientation)
              ;
            else
              continue;
          }
          else {
            if (curr_game.orientation)
              ;
            else
              continue;
          }
        }
        else if (bNotMine) {
          if (curr_game.num_moves % 2) {
            if (curr_game.orientation)
              ;
            else
              continue;
          }
          else {
            if (!curr_game.orientation)
              ;
            else
              continue;
          }
        }

        num_promotions++;
      }
    }

    if (!bNone && num_promotions) {
      printf("%d %s\n",num_promotions,filename);
      total_num_promotions += num_promotions;
    }

    if (bNone && !num_promotions)
      printf("%s\n",filename);
  }

  fclose(fptr);

  if (!bNone && total_num_promotions)
    printf("\n%d total\n",total_num_promotions);

  return 0;
}
