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
"usage: find_mates (-binary_format) (-mine) (-not_mine) (-white) (-black) (-mating_squaresquare) filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bBinaryFormat;
  bool bMine;
  bool bNotMine;
  bool bWhite;
  bool bBlack;
  int file;
  int rank;
  int mating_square;
  int retval;
  FILE *fptr;
  int filename_len;
  struct game curr_game;
  unsigned char board[CHARS_IN_BOARD];

  if ((argc < 2) || (argc > 8)) {
    printf(usage);
    return 1;
  }

  bBinaryFormat = false;
  bMine = false;
  bNotMine = false;
  bWhite = false;
  bBlack = false;
  mating_square = -1;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-binary_format"))
      bBinaryFormat = true;
    else if (!strcmp(argv[curr_arg],"-mine"))
      bMine = true;
    else if (!strcmp(argv[curr_arg],"-not_mine"))
      bNotMine = true;
    else if (!strcmp(argv[curr_arg],"-white"))
      bWhite = true;
    else if (!strcmp(argv[curr_arg],"-black"))
      bBlack = true;
    else if ((strlen(argv[curr_arg]) == 16) &&
      !strncmp(argv[curr_arg],"-mating_square",14) &&
      (argv[curr_arg][14] >= 'a') && (argv[curr_arg][14] <= 'h') &&
      (argv[curr_arg][15] >= '1') && (argv[curr_arg][15] <= '8')) {

      file = argv[curr_arg][14] - 'a';
      rank = argv[curr_arg][15] - '1';
      mating_square = POS_OF(rank,file);
    }
    else
      break;
  }

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  if (bMine && bNotMine) {
    printf("can't specify both -mine and -not_mine\n");
    return 3;
  }

  if (bWhite && bBlack) {
    printf("can't specify both -white and -black\n");
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

    if (!bBinaryFormat) {
      retval = read_game(filename,&curr_game,err_msg);

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

    if (curr_game.moves[curr_game.num_moves-1].special_move_info & SPECIAL_MOVE_MATE) {
      if (mating_square != -1 ) {
        if (curr_game.moves[curr_game.num_moves-1].to != mating_square)
          continue;
      }

      if (bWhite) {
        if (!(curr_game.num_moves % 2))
          continue;
      }
      else if (bBlack) {
        if (curr_game.num_moves % 2)
          continue;
      }

      if (!bMine && !bNotMine)
        printf("%s\n",filename);
      else if (bMine) {
        if (curr_game.num_moves % 2) {
          if (!curr_game.orientation)
            printf("%s\n",filename);
        }
        else {
          if (curr_game.orientation)
            printf("%s\n",filename);
        }
      }
      else {
        if (curr_game.num_moves % 2) {
          if (curr_game.orientation)
            printf("%s\n",filename);
        }
        else {
          if (!curr_game.orientation)
            printf("%s\n",filename);
        }
      }
    }
  }

  return 0;
}
