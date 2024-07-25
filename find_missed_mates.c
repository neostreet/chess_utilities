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
"usage: find_missed_mates (-binary_format) (-all) (in_a_loss) filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

static struct move work_legal_moves[MAX_LEGAL_MOVES];

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bBinaryFormat;
  bool bAll;
  bool bInALoss;
  bool bLoss;
  int retval;
  FILE *fptr;
  int filename_len;
  struct game curr_game;
  bool bBlack;
  struct game work_game;
  int work_legal_moves_count;
  int dbg;

  if ((argc < 2) || (argc > 5)) {
    printf(usage);
    return 1;
  }

  bBinaryFormat = false;
  bAll = false;
  bInALoss = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-binary_format"))
      bBinaryFormat = true;
    else if (!strcmp(argv[curr_arg],"-all"))
      bAll = true;
    else if (!strcmp(argv[curr_arg],"-in_a_loss"))
      bInALoss = true;
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

    set_initial_board(&curr_game);

    for (curr_game.curr_move = 0; curr_game.curr_move < curr_game.num_moves; curr_game.curr_move++) {
      if (curr_game.curr_move == dbg_move)
        dbg = 1;

      legal_moves_count = 0;
      get_legal_moves(&curr_game,legal_moves,&legal_moves_count);

      for (n = 0; n < legal_moves_count; n++) {
        // only search for alternative moves to what was actually played
        if ((legal_moves[n].from == curr_game.moves[curr_game.curr_move].from) &&
            (legal_moves[n].to == curr_game.moves[curr_game.curr_move].to)) {
          continue;
        }

        copy_game(&work_game,&curr_game);
        work_game.moves[work_game.curr_move].from = legal_moves[n].from;
        work_game.moves[work_game.curr_move].to = legal_moves[n].to;
        work_game.moves[work_game.curr_move].special_move_info = 0;
        update_board(&work_game,NULL,NULL,true);
        work_game.curr_move++;

        bBlack = work_game.curr_move & 0x1;

        if (player_is_in_check(bBlack,work_game.board,work_game.curr_move)) {
          // don't report alternative mates if there was a mate in the game at the same move number
          if (curr_game.moves[curr_game.curr_move].special_move_info & SPECIAL_MOVE_MATE)
            ;
          else {
            work_legal_moves_count = 0;
            get_legal_moves(&work_game,work_legal_moves,&work_legal_moves_count);

            if (!work_legal_moves_count) {
              if (!bInALoss) {
                printf("%s: a mate was missed on move %d:\n",filename,curr_game.curr_move);
                print_bd(&work_game);
                break;
              }
              else {
                bLoss = false;

                if (!curr_game.orientation) {
                  if (curr_game.result == BLACK_WIN)
                    bLoss = true;
                }
                else {
                  if (curr_game.result == WHITE_WIN)
                    bLoss = true;
                }

                if (bLoss) {
                  printf("%s: a mate was missed on move %d, in a loss:\n",filename,curr_game.curr_move);
                  print_bd(&work_game);
                  break;
                }
              }
            }
          }
        }
      }

      if (!bAll && (n < legal_moves_count))
        break;

      update_board(&curr_game,NULL,NULL,false);
      update_piece_info(&curr_game);
    }
  }

  fclose(fptr);

  return 0;
}
