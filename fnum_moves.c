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
"  (-lt_num_movesnum_moves) (-terse) (-even) (-odd) filename\n";

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
  int retval;
  FILE *fptr;
  int filename_len;
  struct game curr_game;

  if ((argc < 2) || (argc > 9)) {
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
    else
      break;
  }

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  if ((gt_num_moves != -1) && (eq_num_moves != -1)) {
    printf("can't specify both -gt_num_moves and -eq_num_moves\n");
    return 3;
  }

  if ((gt_num_moves != -1) && (lt_num_moves != -1)) {
    printf("can't specify both -gt_num_moves and -lt_num_moves\n");
    return 4;
  }

  if ((eq_num_moves != -1) && (lt_num_moves != -1)) {
    printf("can't specify both -eq_num_moves and -lt_num_moves\n");
    return 5;
  }

  if (bEven && bOdd) {
    printf("can't specify both -even and -odd\n");
    return 6;
  }

  if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    return 7;
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
