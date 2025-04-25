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
"usage: find_missed_stalemates (-verbose) (-binary_format) (-multiple) (in_a_loss) (-mine) (-opponent)\n"
"  (-both) filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bVerbose;
  bool bBinaryFormat;
  bool bMultiple;
  bool bInALoss;
  bool bMine;
  bool bOpponent;
  bool bBoth;
  int retval;
  FILE *fptr;
  int filename_len;
  struct game curr_game;
  int num_missed_stalemates;
  int my_missed_stalemates;
  int opponent_missed_stalemates;
  int total_missed_stalemates;
  int total_my_missed_stalemates;
  int total_opponent_missed_stalemates;

  if ((argc < 2) || (argc > 9)) {
    printf(usage);
    return 1;
  }

  bVerbose = false;
  bBinaryFormat = false;
  bMultiple = false;
  bInALoss = false;
  bMine = false;
  bOpponent = false;
  bBoth = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else if (!strcmp(argv[curr_arg],"-binary_format"))
      bBinaryFormat = true;
    else if (!strcmp(argv[curr_arg],"-multiple"))
      bMultiple = true;
    else if (!strcmp(argv[curr_arg],"-in_a_loss"))
      bInALoss = true;
    else if (!strcmp(argv[curr_arg],"-mine"))
      bMine = true;
    else if (!strcmp(argv[curr_arg],"-opponent"))
      bOpponent = true;
    else if (!strcmp(argv[curr_arg],"-both"))
      bBoth = true;
    else
      break;
  }

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  if (bMine && bOpponent) {
    printf("can't specify both -mine and -opponent\n");
    return 3;
  }

  if (bMine && bBoth) {
    printf("can't specify both -mine and -both\n");
    return 4;
  }

  if (bOpponent && bBoth) {
    printf("can't specify both -opponent and -both\n");
    return 5;
  }

  if (bMultiple && bBoth) {
    printf("can't specify both -multiple and -both\n");
    return 6;
  }

  if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    return 7;
  }

  if (!bBoth)
    total_missed_stalemates = 0;
  else {
    total_my_missed_stalemates = 0;
    total_opponent_missed_stalemates = 0;
  }

  for ( ; ; ) {
    GetLine(fptr,filename,&filename_len,MAX_FILENAME_LEN);

    if (feof(fptr))
      break;

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

    if (bInALoss) {
      if (curr_game.result != RESULT_LOSS)
        continue;
    }

    if (!bBoth)
      num_missed_stalemates = 0;
    else {
      my_missed_stalemates = 0;
      opponent_missed_stalemates = 0;
    }

    for (curr_game.curr_move = 1; curr_game.curr_move < curr_game.num_moves; curr_game.curr_move++) {
      if (bMine) {
        if (!curr_game.orientation) {
          if (curr_game.curr_move % 2)
            continue;
        }
        else {
          if (!(curr_game.curr_move % 2))
            continue;
        }
      }
      else if (bOpponent) {
        if (!curr_game.orientation) {
          if (!(curr_game.curr_move % 2))
            continue;
        }
        else {
          if (curr_game.curr_move % 2)
            continue;
        }
      }

      if ((curr_game.moves[curr_game.curr_move-1].special_move_info & SPECIAL_MOVE_STALEMATE_IN_ONE) &&
        !(curr_game.moves[curr_game.curr_move].special_move_info & SPECIAL_MOVE_STALEMATE)) {

        if (!bBoth)
          num_missed_stalemates++;
        else {
          if (!curr_game.orientation)
            my_missed_stalemates++;
          else
            opponent_missed_stalemates++;
        }
      }
    }

    if ((!bBoth && num_missed_stalemates) || (bBoth && (my_missed_stalemates > 0) && (opponent_missed_stalemates > 0))) {
      if (bMultiple && (num_missed_stalemates == 1))
        continue;

      if (!bBoth)
        total_missed_stalemates += num_missed_stalemates;
      else {
        total_my_missed_stalemates += my_missed_stalemates;
        total_opponent_missed_stalemates += opponent_missed_stalemates;
      }

      if (!bVerbose)
        printf("%s\n",filename);
      else {
        if (!bBoth)
          printf("%d %s\n",num_missed_stalemates,filename);
        else
          printf("%d %d %s\n",my_missed_stalemates,opponent_missed_stalemates,filename);
      }
    }
  }

  fclose(fptr);

  if (bVerbose) {
    if (!bBoth)
      printf("\n%d\n",total_missed_stalemates);
    else
      printf("\n%d %d\n",total_my_missed_stalemates,total_opponent_missed_stalemates);
  }

  return 0;
}
