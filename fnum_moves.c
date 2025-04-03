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
"usage: fnum_moves (-binary_format) (-gt_num_movesnum_moves) (-eq_num_movesnum_moves)\n"
"  (-lt_num_movesnum_moves) (-terse) (-even) (-odd)\n"
"  (-only_wins) (-only_draws) (-only_losses) filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bBinaryFormat;
  int num_moves;
  int gt_num_moves;
  int eq_num_moves;
  int lt_num_moves;
  bool bTerse;
  bool bEven;
  bool bOdd;
  bool bOnlyWins;
  bool bOnlyDraws;
  bool bOnlyLosses;
  int retval;
  FILE *fptr;
  int filename_len;
  struct game curr_game;

  if ((argc < 2) || (argc > 12)) {
    printf(usage);
    return 1;
  }

  bBinaryFormat = false;
  gt_num_moves = -1;
  eq_num_moves = -1;
  lt_num_moves = -1;
  bTerse = false;
  bEven = false;
  bOdd = false;
  bOnlyWins = false;
  bOnlyDraws = false;
  bOnlyLosses = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-binary_format"))
      bBinaryFormat = true;
    else if (!strncmp(argv[curr_arg],"-gt_num_moves",13))
      sscanf(&argv[curr_arg][13],"%d",&gt_num_moves);
    else if (!strncmp(argv[curr_arg],"-eq_num_moves",13))
      sscanf(&argv[curr_arg][13],"%d",&eq_num_moves);
    else if (!strncmp(argv[curr_arg],"-lt_num_moves",13))
      sscanf(&argv[curr_arg][13],"%d",&lt_num_moves);
    else if (!strcmp(argv[curr_arg],"-terse"))
      bTerse = true;
    else if (!strcmp(argv[curr_arg],"-even"))
      bEven = true;
    else if (!strcmp(argv[curr_arg],"-odd"))
      bOdd = true;
    else if (!strcmp(argv[curr_arg],"-only_wins"))
      bOnlyWins = true;
    else if (!strcmp(argv[curr_arg],"-only_draws"))
      bOnlyDraws = true;
    else if (!strcmp(argv[curr_arg],"-only_losses"))
      bOnlyLosses = true;
    else
      break;
  }

  if (bOnlyWins and bOnlyDraws) {
    printf("can't specify both -only_wins and -only_draws\n");
    return 2;
  }

  if (bOnlyWins and bOnlyLosses) {
    printf("can't specify both -only_wins and -only_losses\n");
    return 3;
  }

  if (bOnlyDraws and bOnlyLosses) {
    printf("can't specify both -only_draws and -only_losses\n");
    return 4;
  }

  if (argc - curr_arg != 1) {
    printf(usage);
    return 5;
  }

  if ((gt_num_moves != -1) && (eq_num_moves != -1)) {
    printf("can't specify both -gt_num_moves and -eq_num_moves\n");
    return 6;
  }

  if ((gt_num_moves != -1) && (lt_num_moves != -1)) {
    printf("can't specify both -gt_num_moves and -lt_num_moves\n");
    return 7;
  }

  if ((eq_num_moves != -1) && (lt_num_moves != -1)) {
    printf("can't specify both -eq_num_moves and -lt_num_moves\n");
    return 8;
  }

  if (bEven && bOdd) {
    printf("can't specify both -even and -odd\n");
    return 9;
  }

  if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    return 10;
  }

  for ( ; ; ) {
    GetLine(fptr,filename,&filename_len,MAX_FILENAME_LEN);

    if (feof(fptr))
      break;

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

    if (bOnlyWins || bOnlyDraws || bOnlyLosses) {
      if (bOnlyWins) {
        if (curr_game.result != RESULT_WIN)
          continue;
      }
      else if (bOnlyDraws) {
        if (curr_game.result != RESULT_DRAW)
          continue;
      }
      else if (bOnlyLosses) {
        if (curr_game.result != RESULT_LOSS)
          continue;
      }
    }

    num_moves = curr_game.num_moves;

    if (gt_num_moves != -1) {
      if (num_moves > gt_num_moves) {
        if (!bTerse)
          printf("%d %s\n",num_moves,filename);
        else
          printf("%d\n",num_moves);
      }
    }
    else if (eq_num_moves != -1) {
      if (num_moves == eq_num_moves) {
        if (!bTerse)
          printf("%d %s\n",num_moves,filename);
        else
          printf("%s\n",filename);
      }
    }
    else if (lt_num_moves != -1) {
      if (num_moves < lt_num_moves) {
        if (!bTerse)
          printf("%d %s\n",num_moves,filename);
        else
          printf("%d\n",num_moves);
      }
    }
    else {
      if (!bTerse) {
        if (bEven) {
          if (!(num_moves % 2))
            printf("%d %s\n",num_moves,filename);
        }
        else if (bOdd) {
          if (num_moves % 2)
            printf("%d %s\n",num_moves,filename);
        }
        else
          printf("%d %s\n",num_moves,filename);
      }
      else {
        if (bEven) {
          if (!(num_moves % 2))
            printf("%d\n",num_moves);
        }
        else if (bOdd) {
          if (num_moves % 2)
            printf("%d\n",num_moves);
        }
        else
          printf("%d\n",num_moves);
      }
    }
  }

  fclose(fptr);

  return 0;
}
