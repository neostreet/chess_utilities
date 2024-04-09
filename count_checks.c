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
"usage: count_checks (-verbose) (-consecutive) (-game_ending) (-by_player) (-mate) (-none) filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bVerbose;
  bool bConsecutive;
  bool bGameEnding;
  bool bByPlayer;
  bool bMate;
  bool bNone;
  int retval;
  FILE *fptr;
  int filename_len;
  struct game curr_game;
  bool bBlack;
  int num_checks;
  int total_num_checks;
  int num_checks1;
  int num_checks2;
  int total_num_checks1;
  int total_num_checks2;
  int max_consecutive_checks1;
  int max_consecutive_checks2;
  int check;
  bool bHaveCheck;

  if ((argc < 2) || (argc > 8)) {
    printf(usage);
    return 1;
  }

  bVerbose = false;
  bConsecutive = false;
  bGameEnding = false;
  bByPlayer = false;
  bMate = false;
  bNone = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else if (!strcmp(argv[curr_arg],"-consecutive"))
      bConsecutive = true;
    else if (!strcmp(argv[curr_arg],"-game_ending"))
      bGameEnding = true;
    else if (!strcmp(argv[curr_arg],"-by_player"))
      bByPlayer = true;
    else if (!strcmp(argv[curr_arg],"-mate"))
      bMate = true;
    else if (!strcmp(argv[curr_arg],"-none"))
      bNone = true;
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

  if ((fptr = fopen(argv[argc-1],"r")) == NULL) {
    printf(couldnt_open,argv[argc-1]);
    return 4;
  }

  if (!bVerbose)
    total_num_checks = 0;
  else {
    total_num_checks1 = 0;
    total_num_checks2 = 0;
  }

  for ( ; ; ) {
    GetLine(fptr,filename,&filename_len,MAX_FILENAME_LEN);

    if (feof(fptr))
      break;

    retval = read_game(filename,&curr_game,err_msg);

    if (retval) {
      printf("read_game of %s failed: %d\n",filename,retval);
      printf("curr_move = %d\n",curr_game.curr_move);
      continue;
    }

    if (bGameEnding) {
      num_checks = 0;

      for (curr_game.curr_move = curr_game.num_moves - 1; curr_game.curr_move >= 0; curr_game.curr_move -= 2) {
        check = curr_game.moves[curr_game.curr_move].special_move_info & SPECIAL_MOVE_CHECK;

        if (!check)
          break;
        else
          num_checks++;
      }

      if (num_checks)
        printf("%d %s\n",num_checks,filename);
    }
    else {
      num_checks1 = 0;
      num_checks2 = 0;

      if (!bConsecutive) {
        for (curr_game.curr_move = 0; curr_game.curr_move < curr_game.num_moves; curr_game.curr_move++) {
          bBlack = curr_game.curr_move & 0x1;

          if (!bMate)
            bHaveCheck = curr_game.moves[curr_game.curr_move].special_move_info & SPECIAL_MOVE_CHECK;
          else
            bHaveCheck = curr_game.moves[curr_game.curr_move].special_move_info & SPECIAL_MOVE_MATE;

          if (bHaveCheck) {
            if (!bByPlayer) {
              if (bBlack)
                num_checks2++;
              else
                num_checks1++;
            }
            else {
              if (!curr_game.orientation) {
                if (bBlack)
                  num_checks2++;
                else
                  num_checks1++;
              }
              else {
                if (bBlack)
                  num_checks1++;
                else
                  num_checks2++;
              }
            }
          }
        }
      }
      else {
        // calculate the number of checks for the first group

        num_checks = 0;
        max_consecutive_checks1 = 0;

        for (curr_game.curr_move = 0; curr_game.curr_move < curr_game.num_moves; curr_game.curr_move += 2) {
          if (curr_game.moves[curr_game.curr_move].special_move_info & SPECIAL_MOVE_CHECK)
            num_checks++;
          else {
            if (num_checks > max_consecutive_checks1)
              max_consecutive_checks1 = num_checks;

            num_checks = 0;
          }
        }

        if (num_checks > max_consecutive_checks1)
          max_consecutive_checks1 = num_checks;

        // calculate the number of checks for the second group

        num_checks = 0;
        max_consecutive_checks2 = 0;

        for (curr_game.curr_move = 1; curr_game.curr_move < curr_game.num_moves; curr_game.curr_move += 2) {
          if (curr_game.moves[curr_game.curr_move].special_move_info & SPECIAL_MOVE_CHECK)
            num_checks++;
          else {
            if (num_checks > max_consecutive_checks2)
              max_consecutive_checks2 = num_checks;

            num_checks = 0;
          }
        }

        if (num_checks > max_consecutive_checks2)
          max_consecutive_checks2 = num_checks;
      }

      if (!bConsecutive) {
        if ((!bNone && (num_checks1 + num_checks2)) || (bNone && !(num_checks1 + num_checks2))) {
          if (!bVerbose) {
            printf("%d %s\n",num_checks1 + num_checks2,filename);
            total_num_checks += num_checks1 + num_checks2;
          }
          else {
            if (!bByPlayer) {
              printf("%3d white, %3d black, %3d total %s\n",
                num_checks1,num_checks2,num_checks1 + num_checks2,filename);
            }
            else {
              printf("%3d me, %3d opponent, %3d total %s\n",
                num_checks1,num_checks2,num_checks1 + num_checks2,filename);
            }

            total_num_checks1 += num_checks1;
            total_num_checks2 += num_checks2;
          }
        }
      }
      else {
        if (!bByPlayer)
          printf("%d white, %d black %s\n",max_consecutive_checks1,max_consecutive_checks2,filename);
        else {
          if (!curr_game.orientation)
            printf("%d me, %d opponent %s\n",max_consecutive_checks1,max_consecutive_checks2,filename);
          else
            printf("%d me, %d opponent %s\n",max_consecutive_checks2,max_consecutive_checks1,filename);
        }
      }
    }
  }

  fclose(fptr);

  if (!bConsecutive && !bGameEnding) {
    if (!bVerbose)
      printf("\n%d\n",total_num_checks);
    else {
      if (!bByPlayer)
        printf("\n%d white, %d black, %d total\n",total_num_checks1,total_num_checks2,total_num_checks1 + total_num_checks2);
      else
        printf("\n%d me, %d opponent, %d total\n",total_num_checks1,total_num_checks2,total_num_checks1 + total_num_checks2);
    }
  }

  return 0;
}
