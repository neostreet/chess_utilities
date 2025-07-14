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
"usage: promotions (-by_player) (-none) (-maskmask) filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bByPlayer;
  bool bNone;
  int retval;
  FILE *fptr;
  int filename_len;
  struct game curr_game;
  bool bBlack;
  int num_promotions1;
  int num_promotions2;
  int total_num_promotions1;
  int total_num_promotions2;
  int mask;

  if ((argc < 2) || (argc > 5)) {
    printf(usage);
    return 1;
  }

  bByPlayer = false;
  bNone = false;
  mask = 0;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-by_player"))
      bByPlayer = true;
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

  if ((fptr = fopen(argv[argc-1],"r")) == NULL) {
    printf(couldnt_open,argv[argc-1]);
    return 3;
  }

  total_num_promotions1 = 0;
  total_num_promotions2 = 0;

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

    num_promotions1 = 0;
    num_promotions2 = 0;

    for (curr_game.curr_move = 0; curr_game.curr_move < curr_game.num_moves; curr_game.curr_move++) {
      bBlack = curr_game.curr_move & 0x1;

      if (!mask) {
        mask = SPECIAL_MOVE_PROMOTION_QUEEN | SPECIAL_MOVE_PROMOTION_ROOK |
          SPECIAL_MOVE_PROMOTION_KNIGHT | SPECIAL_MOVE_PROMOTION_BISHOP;
      }

      if (curr_game.moves[curr_game.curr_move].special_move_info & mask) {
        if (!bByPlayer) {
          if (bBlack)
            num_promotions2++;
          else
            num_promotions1++;
        }
        else {
          if (!curr_game.orientation) {
            if (bBlack)
              num_promotions2++;
            else
              num_promotions1++;
          }
          else {
            if (bBlack)
              num_promotions1++;
            else
              num_promotions2++;
          }
        }
      }
    }

    if ((!bNone && (num_promotions1 + num_promotions2)) || (bNone && !(num_promotions1 + num_promotions2))) {
      if (!bByPlayer) {
        printf("%3d white, %3d black, %3d total %s\n",
          num_promotions1,num_promotions2,num_promotions1 + num_promotions2,filename);
      }
      else {
        printf("%3d me, %3d opponent, %3d total %s\n",
          num_promotions1,num_promotions2,num_promotions1 + num_promotions2,filename);
      }

      total_num_promotions1 += num_promotions1;
      total_num_promotions2 += num_promotions2;
    }
  }

  fclose(fptr);

  if (!bByPlayer)
    printf("\n%d white, %d black, %d total\n",total_num_promotions1,total_num_promotions2,total_num_promotions1 + total_num_promotions2);
  else
    printf("\n%d me, %d opponent, %d total\n",total_num_promotions1,total_num_promotions2,total_num_promotions1 + total_num_promotions2);

  return 0;
}
