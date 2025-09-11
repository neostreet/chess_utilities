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
"usage: count_mirrored_boards (-debug) (-verbose)\n"
"  (-ge_valval) (-terse_modemode)\n"
"  (-only_wins) (-only_draws) (-only_losses) (-i_am_white) (-i_am_black)\n"
"  (-exact_countcount) (-max_move_first) filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bDebug;
  bool bVerbose;
  int ge_val;
  int terse_mode;
  bool bOnlyWins;
  bool bOnlyDraws;
  bool bOnlyLosses;
  bool bIAmWhite;
  bool bIAmBlack;
  int exact_count;
  bool bMaxMoveFirst;
  int retval;
  FILE *fptr;
  int filename_len;
  struct game curr_game;
  int num_mirrored_boards;
  int max_move;

  if ((argc < 2) || (argc > 13)) {
    printf(usage);
    return 1;
  }

  bDebug = false;
  bVerbose = false;
  ge_val = -1;
  terse_mode = 0;
  bOnlyWins = false;
  bOnlyDraws = false;
  bOnlyLosses = false;
  bIAmWhite = false;
  bIAmBlack = false;
  exact_count = -1;
  bMaxMoveFirst = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = true;
    else if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else if (!strncmp(argv[curr_arg],"-ge_val",7))
      sscanf(&argv[curr_arg][7],"%d",&ge_val);
    else if (!strncmp(argv[curr_arg],"-terse_mode",11))
      sscanf(&argv[curr_arg][11],"%d",&terse_mode);
    else if (!strcmp(argv[curr_arg],"-only_wins"))
      bOnlyWins = true;
    else if (!strcmp(argv[curr_arg],"-only_draws"))
      bOnlyDraws = true;
    else if (!strcmp(argv[curr_arg],"-only_losses"))
      bOnlyLosses = true;
    else if (!strcmp(argv[curr_arg],"-i_am_white"))
      bIAmWhite = true;
    else if (!strcmp(argv[curr_arg],"-i_am_black"))
      bIAmBlack = true;
    else if (!strncmp(argv[curr_arg],"-exact_count",12))
      sscanf(&argv[curr_arg][12],"%d",&exact_count);
    else if (!strcmp(argv[curr_arg],"-max_move_first"))
      bMaxMoveFirst = true;
    else
      break;
  }

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  if (bOnlyWins && bOnlyDraws) {
    printf("can't specify both -only_wins and -only_draws\n");
    return 3;
  }

  if (bOnlyWins && bOnlyLosses) {
    printf("can't specify both -only_wins and -only_losses\n");
    return 4;
  }

  if (bOnlyDraws && bOnlyLosses) {
    printf("can't specify both -only_draws and -only_losses\n");
    return 5;
  }

  if (bIAmWhite and bIAmBlack) {
    printf("can't specify both -i_am_white and -i_am_black\n");
    return 6;
  }

  if ((fptr = fopen(argv[argc-1],"r")) == NULL) {
    printf(couldnt_open,argv[argc-1]);
    return 7;
  }

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

    if (bIAmWhite && curr_game.orientation)
      continue;

    if (bIAmBlack && !curr_game.orientation)
      continue;

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

    set_initial_board(&curr_game);
    num_mirrored_boards = 0;

    for (curr_game.curr_move = 0; curr_game.curr_move < curr_game.num_moves; curr_game.curr_move++) {
      update_board(&curr_game,NULL,NULL,false);

      if (curr_game.curr_move >= 3) {
        if (board_is_mirrored(&curr_game)) {
          num_mirrored_boards++;
          max_move = curr_game.curr_move;

          if (bVerbose) {
            printf("curr_move = %d\n",curr_game.curr_move);
            putchar(0x0a);
            print_bd(&curr_game);
          }
        }
      }
    }

    if (!num_mirrored_boards)
        continue;

    if ((exact_count != -1) && (num_mirrored_boards != exact_count))
      ;
    else {
      if (num_mirrored_boards >= ge_val) {
        if (terse_mode == 1)
          printf("%d\n",num_mirrored_boards);
        else if (terse_mode == 2)
          printf("%s\n",filename);
        else {
          if (!bMaxMoveFirst)
            printf("%d %d %s\n",num_mirrored_boards,max_move,filename);
          else
            printf("%d %d %s\n",max_move,num_mirrored_boards,filename);
        }
      }
    }
  }

  fclose(fptr);

  return 0;
}
