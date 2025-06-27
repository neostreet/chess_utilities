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
"usage: count_checks (-debug) (-verbose) (-consecutive) (-mine) (-opponent) (-game_ending)\n"
"  (-game_ending_countcount) (-mate) (-none) (-binary_format) (-ge_valval) (-terse_modemode)\n"
"  (-game_ending_in_mate) filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bDebug;
  bool bVerbose;
  bool bConsecutive;
  bool bMine;
  bool bOpponent;
  bool bGameEnding;
  int game_ending_count;
  bool bMate;
  bool bNone;
  bool bBinaryFormat;
  int ge_val;
  int terse_mode;
  bool bGameEndingInMate;
  int retval;
  FILE *fptr;
  int filename_len;
  struct game curr_game;
  int num_checks;
  int max_consecutive_checks;
  int check;
  bool bHaveCheck;
  int starting_move;
  int increment;

  if ((argc < 2) || (argc > 15)) {
    printf(usage);
    return 1;
  }

  bDebug = false;
  bVerbose = false;
  bConsecutive = false;
  bMine = false;
  bOpponent = false;
  bGameEnding = false;
  game_ending_count = 0;
  bMate = false;
  bNone = false;
  bBinaryFormat = false;
  ge_val = -1;
  terse_mode = 0;
  bGameEndingInMate = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = true;
    else if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else if (!strcmp(argv[curr_arg],"-consecutive"))
      bConsecutive = true;
    else if (!strcmp(argv[curr_arg],"-mine"))
      bMine = true;
    else if (!strcmp(argv[curr_arg],"-opponent"))
      bOpponent = true;
    else if (!strcmp(argv[curr_arg],"-game_ending"))
      bGameEnding = true;
    else if (!strncmp(argv[curr_arg],"-game_ending_count",18))
      sscanf(&argv[curr_arg][18],"%d",&game_ending_count);
    else if (!strcmp(argv[curr_arg],"-mate"))
      bMate = true;
    else if (!strcmp(argv[curr_arg],"-none"))
      bNone = true;
    else if (!strcmp(argv[curr_arg],"-binary_format"))
      bBinaryFormat = true;
    else if (!strncmp(argv[curr_arg],"-ge_val",7))
      sscanf(&argv[curr_arg][7],"%d",&ge_val);
    else if (!strncmp(argv[curr_arg],"-terse_mode",11))
      sscanf(&argv[curr_arg][11],"%d",&terse_mode);
    else if (!strcmp(argv[curr_arg],"-game_ending_in_mate")) {
      bGameEnding = true;
      bGameEndingInMate = true;
    }
    else
      break;
  }

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  if (bMate && bNone) {
    printf("can't specify both -mate and -none\n");
    return 3;
  }

  if (bMine && bOpponent) {
    printf("can't specify both -mine and -opponent\n");
    return 4;
  }

  if (bConsecutive) {
    if (!bMine && !bOpponent) {
      printf("if -consecutive is specified, either -mine or -opponent must be specified\n");
      return 5;
    }
  }

  if ((fptr = fopen(argv[argc-1],"r")) == NULL) {
    printf(couldnt_open,argv[argc-1]);
    return 6;
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

    if (bGameEnding) {
      if (bGameEndingInMate) {
        if (curr_game.moves[curr_game.num_moves - 1].special_move_info & SPECIAL_MOVE_MATE)
          ;
        else
          continue;
      }

      num_checks = 0;

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

      for (curr_game.curr_move = curr_game.num_moves - 1; curr_game.curr_move >= 0; curr_game.curr_move -= 2) {
        check = curr_game.moves[curr_game.curr_move].special_move_info & SPECIAL_MOVE_CHECK;

        if (!check)
          break;
        else
          num_checks++;
      }

      if (num_checks) {
        if (!game_ending_count)
          printf("%d %s\n",num_checks,filename);
        else if (num_checks == game_ending_count)
          printf("%s\n",filename);
      }

      continue;
    }
    else {
      num_checks = 0;

      if (bMine) {
        if (!curr_game.orientation)
          starting_move = 0;
        else
          starting_move = 1;

        increment = 2;
      }
      else if (bOpponent) {
        if (!curr_game.orientation)
          starting_move = 1;
        else
          starting_move = 0;

        increment = 2;
      }
      else {
        starting_move = 0;
        increment = 1;
      }

      if (!bConsecutive) {
        for (curr_game.curr_move = starting_move; curr_game.curr_move < curr_game.num_moves; curr_game.curr_move += increment) {
          if (!bMate)
            bHaveCheck = curr_game.moves[curr_game.curr_move].special_move_info & SPECIAL_MOVE_CHECK;
          else
            bHaveCheck = curr_game.moves[curr_game.curr_move].special_move_info & SPECIAL_MOVE_MATE;

          if (bHaveCheck) {
            num_checks++;

            if (bVerbose) {
              if (num_checks == 1)
                printf("%s\n",filename);

              printf("  %d\n",curr_game.curr_move);
            }
          }
        }
      }
      else {
        num_checks = 0;
        max_consecutive_checks = 0;

        for (curr_game.curr_move = starting_move; curr_game.curr_move < curr_game.num_moves; curr_game.curr_move += increment) {
          if (curr_game.moves[curr_game.curr_move].special_move_info & SPECIAL_MOVE_CHECK)
            num_checks++;
          else {
            if (num_checks > max_consecutive_checks)
              max_consecutive_checks = num_checks;

            num_checks = 0;
          }
        }

        if (num_checks > max_consecutive_checks)
          max_consecutive_checks = num_checks;

        if (num_checks != max_consecutive_checks)
          num_checks = max_consecutive_checks;
      }

      if (!bVerbose) {
        if ((!bNone && (num_checks)) || (bNone && !num_checks)) {
          if (!bMate) {
            if (ge_val == -1) {
              if (terse_mode == 1)
                printf("%d\n",num_checks);
              else if (terse_mode == 2)
                printf("%s\n",filename);
              else
                printf("%d %s\n",num_checks,filename);
            }
            else if (num_checks >= ge_val) {
              if (terse_mode == 1)
                printf("%d\n",num_checks);
              else if (terse_mode == 2)
                printf("%s\n",filename);
              else
                printf("%d %s\n",num_checks,filename);
            }
          }
          else {
            if (num_checks >= ge_val)
              printf("%s\n",filename);
          }
        }
      }
    }
  }

  fclose(fptr);

  return 0;
}
