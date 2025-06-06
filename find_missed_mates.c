#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "chess.h"
#define MAKE_GLOBALS_HERE
#include "chess.glb"
#include "chess.fun"
#include "chess.mac"

#define MAX_FILENAME_LEN 256
static char filename[MAX_FILENAME_LEN];
static char trunc_filename[MAX_FILENAME_LEN];

static char usage[] =
"usage: find_missed_mates (-terse) (-all) (in_a_loss) (-mine) (-opponent)\n"
"  (-count) (-both_players) (-white) (-black) (-truncate) (-binary_format) (-ignore_read_errors) (-extext) filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

static struct move work_legal_moves[MAX_LEGAL_MOVES];

static int build_trunc_filename(
  char *bin_filename,
  int bin_filename_len,
  char *trunc_filename,
  int max_filename_len,
  char *ext);

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bTerse;
  bool bAll;
  bool bInALoss;
  bool bMine;
  bool bOpponent;
  bool bCount;
  bool bBothPlayers;
  bool bWhite;
  bool bBlack;
  bool bTruncate;
  bool bBinaryFormat;
  bool bIgnoreReadErrors;
  char *ext;
  int retval;
  FILE *fptr;
  int filename_len;
  int bin_filename_len;
  struct game curr_game;
  bool bBlacksMove;
  struct game work_game;
  int work_legal_moves_count;
  int dbg;
  int count;
  int white_count;
  int black_count;
  char *cpt;

  if ((argc < 2) || (argc > 15)) {
    printf(usage);
    return 1;
  }

  bTerse = false;
  bAll = false;
  bInALoss = false;
  bMine = false;
  bOpponent = false;
  bCount = false;
  bBothPlayers = false;
  bWhite = false;
  bBlack = false;
  bTruncate = false;
  bBinaryFormat = false;
  bIgnoreReadErrors = false;
  ext = NULL;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-terse"))
      bTerse = true;
    else if (!strcmp(argv[curr_arg],"-all"))
      bAll = true;
    else if (!strcmp(argv[curr_arg],"-in_a_loss"))
      bInALoss = true;
    else if (!strcmp(argv[curr_arg],"-mine"))
      bMine = true;
    else if (!strcmp(argv[curr_arg],"-opponent"))
      bOpponent = true;
    else if (!strcmp(argv[curr_arg],"-count"))
      bCount = true;
    else if (!strcmp(argv[curr_arg],"-both_players"))
      bBothPlayers = true;
    else if (!strcmp(argv[curr_arg],"-white"))
      bWhite = true;
    else if (!strcmp(argv[curr_arg],"-black"))
      bBlack = true;
    else if (!strcmp(argv[curr_arg],"-truncate"))
      bTruncate = true;
    else if (!strcmp(argv[curr_arg],"-binary_format"))
      bBinaryFormat = true;
    else if (!strcmp(argv[curr_arg],"-ignore_read_errors"))
      bIgnoreReadErrors = true;
    else if (!strncmp(argv[curr_arg],"-ext",4))
      ext = &argv[curr_arg][4];
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

  if (bCount && bBothPlayers) {
    printf("can't specify both -count and -both_players\n");
    return 4;
  }

  if (bWhite && bBlack) {
    printf("can't specify both -white and -black\n");
    return 5;
  }

  if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    return 6;
  }

  cpt = getenv("DEBUG_FIND_MISSED_MATES");

  if (cpt != NULL) {
    debug_level = atoi(cpt);
    debug_fptr = fopen("find_missed_mates.dbg","w");
  }
  else {
    debug_level = 0;
    debug_fptr = NULL;
  }

  for ( ; ; ) {
    GetLine(fptr,filename,&filename_len,MAX_FILENAME_LEN);

    if (feof(fptr))
      break;

    bzero(&curr_game,sizeof (struct game));

    if (!bBinaryFormat) {
      retval = read_game(filename,&curr_game);

      if (retval) {
        if (!bIgnoreReadErrors) {
          printf("read_game of %s failed: %d\n",filename,retval);
          printf("curr_move = %d\n",curr_game.curr_move);

          continue;
        }
        else
          printf("ignoring read error of %s at move %d\n",filename,curr_game.curr_move);
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

    if (bCount)
      count = 0;
    else if (bBothPlayers) {
      white_count = 0;
      black_count = 0;
    }

    for (curr_game.curr_move = 0;
         curr_game.curr_move < curr_game.num_moves;
         update_board(&curr_game,NULL,NULL,false),update_piece_info(&curr_game),curr_game.curr_move++) {

      if (curr_game.curr_move == dbg_move)
        dbg = 1;

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

      legal_moves_count = 0;
      get_legal_moves(&curr_game,legal_moves,&legal_moves_count);

      for (n = 0; n < legal_moves_count; n++) {
        // only search for alternative moves to what was actually played
        if ((legal_moves[n].from == curr_game.moves[curr_game.curr_move].from) &&
            (legal_moves[n].to == curr_game.moves[curr_game.curr_move].to)) {
          continue;
        }

        if (n == dbg_move)
          dbg = 1;

        copy_game(&work_game,&curr_game);
        work_game.moves[work_game.curr_move].from = legal_moves[n].from;
        work_game.moves[work_game.curr_move].to = legal_moves[n].to;
        work_game.moves[work_game.curr_move].special_move_info = legal_moves[n].special_move_info;
        update_board(&work_game,NULL,NULL,true);
        work_game.curr_move++;

        bBlacksMove = work_game.curr_move & 0x1;

        if (player_is_in_check(bBlacksMove,work_game.board,work_game.curr_move)) {
          // don't report alternative mates if there was a mate in the game at the same move number
          if (curr_game.moves[curr_game.curr_move].special_move_info & SPECIAL_MOVE_MATE)
            ;
          else {
            work_legal_moves_count = 0;
            get_legal_moves(&work_game,work_legal_moves,&work_legal_moves_count);

            if (!work_legal_moves_count) {
              if (!bInALoss) {
                if (bCount) {
                  count++;
                }
                else if (bBothPlayers) {
                  if (!(curr_game.curr_move % 2))
                    white_count++;
                  else
                    black_count++;
                }
                else {
                  if ((bWhite && !bBlacksMove) || (bBlack && bBlacksMove)) {
                      ;
                  }
                  else {
                    if (bTerse) {
                      printf("%s\n",filename);

                      if (bTruncate) {
                        curr_game.num_moves = curr_game.curr_move;

                        bin_filename_len = strlen(filename);

                        retval = build_trunc_filename(filename,bin_filename_len,trunc_filename,MAX_FILENAME_LEN,ext);

                        if (retval) {
                          printf("build_trunc_filename failed on %s: %d\n",filename,retval);
                        }
                        else {
                          curr_game.orientation = curr_game.num_moves % 2;

                          retval = write_binary_game(trunc_filename,&curr_game);

                          if (retval)
                            printf("write_binary_game of %s failed: %d\n",trunc_filename,retval);
                        }
                      }
                    }
                    else {
                      printf("%s: a mate was missed on move %d, from = %c%c, to = %c%c:\n",
                        filename,curr_game.curr_move,
                        'a' + FILE_OF(legal_moves[n].from),'1' + RANK_OF(legal_moves[n].from),
                        'a' + FILE_OF(legal_moves[n].to),'1' + RANK_OF(legal_moves[n].to));
                      print_bd(&work_game);
                    }
                  }
                }

                break;
              }
              else {
                if (curr_game.result == RESULT_LOSS) {
                  if (bCount) {
                    count++;
                  }
                  else if (bBothPlayers) {
                    if (!(curr_game.curr_move % 2))
                      white_count++;
                    else
                      black_count++;
                  }
                  else {
                    if (bTerse) {
                      printf("%s\n",filename);
                    }
                    else {
                      printf("%s: a mate was missed on move %d, from = %c%c, to = %c%c, in a loss:\n",
                        filename,curr_game.curr_move,
                        'a' + FILE_OF(legal_moves[n].from),'1' + RANK_OF(legal_moves[n].from),
                        'a' + FILE_OF(legal_moves[n].to),'1' + RANK_OF(legal_moves[n].to));
                      print_bd(&work_game);
                    }
                  }

                  break;
                }
              }
            }
          }
        }
      }

      if (!bAll && !bCount && !bBothPlayers && (n < legal_moves_count))
        break;
    }

    if (bCount) {
      if (count) {
        printf("%d missed mates found in %s\n",count,filename);
      }
    }
    else if (bBothPlayers) {
      if ((white_count >= 1) && (black_count >= 1)) {
        printf("%s\n",filename);
      }
    }
  }

  fclose(fptr);

  if (debug_fptr)
    fclose(debug_fptr);

  return 0;
}

static int build_trunc_filename(
  char *bin_filename,
  int bin_filename_len,
  char *trunc_filename,
  int max_filename_len,
  char *ext)
{
  int n;

  for (n = 0; n < bin_filename_len; n++) {
    if (bin_filename[n] == '.')
      break;
  }

  if (n == bin_filename_len)
    return 1;

  if (!ext) {
    if (n + 6 > max_filename_len - 1)
      return 2;
  }
  else {
    if (n + 1 + strlen(ext) > max_filename_len - 1)
      return 3;
  }

  strncpy(trunc_filename,bin_filename,n);

  if (!ext)
    strcpy(&trunc_filename[n],".trunc");
  else {
    trunc_filename[n] = '.';
    strcpy(&trunc_filename[n+1],ext);
  }

  return 0;
}
