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
"usage: find_consecutive_promotions (-verbose) filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

static bool has_consecutive_promotions(struct game *gamept,int *promotion_move);
static bool is_promotion(int special_move_info);

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bVerbose;
  int retval;
  FILE *fptr;
  int filename_len;
  struct game curr_game;
  int promotion_move;

  if ((argc < 2) || (argc > 3)) {
    printf(usage);
    return 1;
  }

  bVerbose = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
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

    if (has_consecutive_promotions(&curr_game,&promotion_move)) {
      if (!bVerbose)
        printf("%s\n",filename);
      else
        printf("%s %d\n",filename,promotion_move);
    }
  }

  return 0;
}

static bool has_consecutive_promotions(struct game *gamept,int *promotion_move)
{
  int n;

  for (n = 0; n < gamept->num_moves - 1; n++) {
    if (is_promotion(gamept->moves[n].special_move_info) && is_promotion(gamept->moves[n + 1].special_move_info)) {
      *promotion_move = n;

      return true;
    }
  }

  return false;
}

static bool is_promotion(int special_move_info)
{
  if (special_move_info & (SPECIAL_MOVE_PROMOTION_QUEEN | SPECIAL_MOVE_PROMOTION_ROOK | SPECIAL_MOVE_PROMOTION_KNIGHT | SPECIAL_MOVE_PROMOTION_BISHOP))
    return true;

  return false;
}
