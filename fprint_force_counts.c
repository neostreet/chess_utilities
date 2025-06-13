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
"usage: fprint_force_counts (-binary_format) (-i_am_white) (-i_am_black) (-my_count_first) (-diff_first)\n"
"  (-max_diff) (-go_to_movemove) filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bBinaryFormat;
  bool bIAmWhite;
  bool bIAmBlack;
  bool bMyCountFirst;
  bool bDiffFirst;
  bool bMaxDiff;
  bool bGoToMove;
  int go_to_move;
  int retval;
  FILE *fptr;
  int filename_len;
  struct game curr_game;
  int first_count;
  int second_count;
  int max_first_count;
  int max_second_count;
  int max_diff;
  int curr_diff;
  int move_number;

  if ((argc < 2) || (argc > 9)) {
    printf(usage);
    return 1;
  }

  bBinaryFormat = false;
  bIAmWhite = false;
  bIAmBlack = false;
  bMyCountFirst = false;
  bDiffFirst = false;
  bMaxDiff = false;
  bGoToMove = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-binary_format"))
      bBinaryFormat = true;
    else if (!strcmp(argv[curr_arg],"-i_am_white"))
      bIAmWhite = true;
    else if (!strcmp(argv[curr_arg],"-i_am_black"))
      bIAmBlack = true;
    else if (!strcmp(argv[curr_arg],"-my_count_first"))
      bMyCountFirst = true;
    else if (!strcmp(argv[curr_arg],"-diff_first"))
      bDiffFirst = true;
    else if (!strcmp(argv[curr_arg],"-max_diff"))
      bMaxDiff = true;
    else if (!strncmp(argv[curr_arg],"-go_to_move",11)) {
      bGoToMove = true;
      sscanf(&argv[curr_arg][11],"%d",&go_to_move);
    }
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

  if (bMaxDiff && bGoToMove) {
    printf("can't specify both -max_diff and -go_to_move\n");
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

    if (bIAmWhite && curr_game.orientation)
      continue;

    if (bIAmBlack && !curr_game.orientation)
      continue;

    if (bGoToMove) {
      if (go_to_move < 0)
        go_to_move = 0;
      else if (go_to_move > curr_game.num_moves)
        go_to_move = curr_game.num_moves;

      position_game(&curr_game,go_to_move);
    }

    if (!bMaxDiff) {
      if (!bMyCountFirst) {
        first_count = force_count[WHITE];
        second_count = force_count[BLACK];
      }
      else {
        if (!curr_game.orientation) {
          first_count = force_count[WHITE];
          second_count = force_count[BLACK];
        }
        else {
          first_count = force_count[BLACK];
          second_count = force_count[WHITE];
        }
      }
    }
    else {
      max_diff = -1;

      position_game(&curr_game,0);

      for (curr_game.curr_move = 0; curr_game.curr_move < curr_game.num_moves; curr_game.curr_move++) {
        update_board(&curr_game,NULL,NULL,false);
        calculate_force_counts(&curr_game);

        if (!bMyCountFirst) {
          first_count = force_count[WHITE];
          second_count = force_count[BLACK];
        }
        else {
          if (!curr_game.orientation) {
            first_count = force_count[WHITE];
            second_count = force_count[BLACK];
          }
          else {
            first_count = force_count[BLACK];
            second_count = force_count[WHITE];
          }
        }

        curr_diff = first_count - second_count;

        if (curr_diff > max_diff) {
          max_diff = curr_diff;
          max_first_count = first_count;
          max_second_count = second_count;
          move_number = curr_game.curr_move + 1;
        }
      }

      if (max_diff == -1) {
        max_diff = 0;
        max_first_count = 39;
        max_second_count = 39;
        move_number = 0;
      }
    }

    if (!bMaxDiff) {
      if (!bDiffFirst) {
        if (!bGoToMove) {
          printf("%d %d (%d) %s %s\n",
            first_count,
            second_count,
            first_count - second_count,
            filename,
            curr_game.date);
        }
        else {
          printf("%d %d (%d) %s move %d %s\n",
            first_count,
            second_count,
            first_count - second_count,
            filename,
            go_to_move,
            curr_game.date);
        }
      }
      else {
        if (!bGoToMove) {
          printf("%d %d %d %s %s\n",
            first_count - second_count,
            first_count,
            second_count,
            filename,
            curr_game.date);
        }
        else {
          printf("%d %d %d %s move %d %s\n",
            first_count - second_count,
            first_count,
            second_count,
            filename,
            go_to_move,
            curr_game.date);
        }
      }
    }
    else {
      if (!bDiffFirst) {
        printf("%d %d (%d) %s move %d %s\n",
          max_first_count,
          max_second_count,
          max_diff,
          filename,
          move_number,
          curr_game.date);
      }
      else {
        printf("%d %d %d %s move %d %s\n",
          max_diff,
          max_first_count,
          max_second_count,
          filename,
          move_number,
          curr_game.date);
      }
    }
  }

  fclose(fptr);

  return 0;
}
