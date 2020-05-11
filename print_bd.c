#include <vector>

using namespace std;

#include <stdio.h>
#include <string.h>

#include "chess.h"
#define MAKE_GLOBALS_HERE
#include "chess.glb"
#include "chess.fun"
#include "chess.mac"

static char usage[] =
"usage: print_bd (-debug) (-toggle) (-space) (-force) (-initial_boardfilename)\n"
"  (-init_bin_boardfilename) (-board_binfilename) (-print_pieces)\n"
"  (-min_force_diffvalue) (-match_boardfilename) (-only_checks) (-only_castle)\n"
"  (-only_promotions) (-only_captures) (-multiple_queens) (-move_number_only) \n"
"  (-qnn) [white | black] filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

static unsigned char match_board1[CHARS_IN_BOARD];

void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
void print_space_and_force(struct game *game_pt,bool bSpace,bool bForce);

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bDebug;
  bool bToggle;
  bool bSpace;
  bool bForce;
  bool bBoardBin;
  bool bPrintPieces;
  bool bOnlyChecks;
  bool bOnlyCastle;
  bool bOnlyPromotions;
  bool bOnlyCaptures;
  bool bMultipleQueens;
  bool bMoveNumberOnly;
  bool bHaveMatchBoard;
  bool bPrintedBoard;
  int board_bin_arg;
  int quiz_number;
  int min_force_diff;
  int force_diff;
  bool bBlack;
  int initial_move;
  int retval;
  struct game curr_game;
  int match;

  if ((argc < 2) || (argc > 19)) {
    printf(usage);
    return 1;
  }

  bDebug = false;
  bToggle = false;
  bSpace = false;
  bForce = false;
  quiz_number = -1;
  bBoardBin = false;
  bPrintPieces = false;
  bOnlyChecks = false;
  bOnlyCastle = false;
  bOnlyPromotions = false;
  bOnlyCaptures = false;
  bMultipleQueens = false;
  bMoveNumberOnly = false;
  min_force_diff = -1;
  force_diff = 0;
  bHaveMatchBoard = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = true;
    else if (!strcmp(argv[curr_arg],"-toggle"))
      bToggle = true;
    else if (!strcmp(argv[curr_arg],"-space"))
      bSpace = true;
    else if (!strcmp(argv[curr_arg],"-force"))
      bForce = true;
    else if (!strncmp(argv[curr_arg],"-initial_board",14)) {
      retval = populate_initial_board_from_board_file(&argv[curr_arg][14]);

      if (retval) {
        printf("populate_initial_board_from_board_file() failed on 5s: %d\n",
          &argv[curr_arg][14],retval);
        return 2;
      }
    }
    else if (!strncmp(argv[curr_arg],"-init_bin_board",15)) {
      retval = populate_initial_board_from_bin_board_file(&argv[curr_arg][15]);

      if (retval) {
        printf("populate_initial_board_from_bin_board_file() failed on %s: %d\n",
          &argv[curr_arg][15],retval);
        return 3;
      }
    }
    else if (!strcmp(argv[curr_arg],"-print_pieces"))
      bPrintPieces = true;
    else if (!strncmp(argv[curr_arg],"-board_bin",10)) {
      bBoardBin = true;
      board_bin_arg = curr_arg;
    }
    else if (!strncmp(argv[curr_arg],"-qn",3))
      sscanf(&argv[curr_arg][3],"%d",&quiz_number);
    else if (!strncmp(argv[curr_arg],"-min_force_diff",15))
      sscanf(&argv[curr_arg][15],"%d",&min_force_diff);
    else if (!strncmp(argv[curr_arg],"-match_board",12)) {
      bHaveMatchBoard = true;
      retval = populate_board_from_bin_board_file(match_board1,&argv[curr_arg][12]);

      if (retval) {
        printf("populate_board_from_board_file() failed on %s: %d\n",
          &argv[curr_arg][12],retval);
        return 4;
      }

      //if (bDebug)
        //print_bd0(match_board1);
    }
    else if (!strcmp(argv[curr_arg],"-only_checks"))
      bOnlyChecks = true;
    else if (!strcmp(argv[curr_arg],"-only_castle"))
      bOnlyCastle = true;
    else if (!strcmp(argv[curr_arg],"-only_promotions"))
      bOnlyPromotions = true;
    else if (!strcmp(argv[curr_arg],"-only_captures"))
      bOnlyCaptures = true;
    else if (!strcmp(argv[curr_arg],"-multiple_queens"))
      bMultipleQueens = true;
    else if (!strcmp(argv[curr_arg],"-move_number_only"))
      bMoveNumberOnly = true;
    else
      break;
  }

  if (bOnlyChecks && bOnlyCastle) {
    printf("can't specify both -only_checks and -only_castle\n");
    return 5;
  }

  if (quiz_number != -1) {
    if (argc - curr_arg != 2) {
      printf(usage);
      return 6;
    }

    if (!strcmp(argv[curr_arg],"white"))
      bBlack = false;
    else if (!strcmp(argv[curr_arg],"black"))
      bBlack = true;
    else {
      printf(usage);
      return 7;
    }
  }
  else {
    if (argc - curr_arg != 1) {
      printf(usage);
      return 8;
    }
  }

  if (min_force_diff != -1) {
    bForce = true;
    bDebug = true;
  }

  if (bHaveMatchBoard || bOnlyChecks || bOnlyCastle || bOnlyPromotions || bOnlyCaptures || bMultipleQueens) {
    bDebug = true;
  }

  retval = read_game(argv[argc-1],&curr_game,err_msg);

  if (retval) {
    printf("read_game of %s failed: %d\n",argv[argc-1],retval);
    printf("curr_move = %d\n",curr_game.curr_move);
    return 9;
  }

  if (bToggle)
    curr_game.orientation ^= 1;

  if (bDebug) {
    //for (n = 0; n < curr_game.num_moves; n++) {
      //printf("%2d %2d\n",curr_game.moves[n].from,curr_game.moves[n].to);
    //}

    set_initial_board(&curr_game);
    curr_game.curr_move = 0;
    bPrintedBoard = false;

    for (curr_game.curr_move = 0; curr_game.curr_move < curr_game.num_moves; curr_game.curr_move++) {
      update_board(&curr_game,false);

      if (min_force_diff != -1) {
        force_diff = refresh_force_count(&curr_game);

        if (force_diff < min_force_diff)
          continue;
      }

      if (bHaveMatchBoard) {
        match = match_board(curr_game.board,match_board1);

        if (!match)
          continue;
      }

      if (bOnlyChecks) {
        if (!(curr_game.moves[curr_game.curr_move].special_move_info & SPECIAL_MOVE_CHECK))
          continue;
      }

      if (bOnlyCastle) {
        if (!(curr_game.moves[curr_game.curr_move].special_move_info & SPECIAL_MOVE_CASTLE))
          continue;
      }

      if (bOnlyPromotions) {
        if (!(curr_game.moves[curr_game.curr_move].special_move_info & (SPECIAL_MOVE_PROMOTION_QUEEN | SPECIAL_MOVE_PROMOTION_ROOK | SPECIAL_MOVE_PROMOTION_KNIGHT | SPECIAL_MOVE_PROMOTION_BISHOP)))
          continue;
      }

      if (bOnlyCaptures) {
        if (!(curr_game.moves[curr_game.curr_move].special_move_info & SPECIAL_MOVE_CAPTURE))
          continue;
      }

      if (bMultipleQueens) {
        if (!multiple_queens((unsigned char *)&curr_game.board))
          continue;
      }

      if (bPrintedBoard)
        putchar(0x0a);

      printf("curr_move = %d\n",curr_game.curr_move);

      if (!bMoveNumberOnly) {
        print_space_and_force(&curr_game,bSpace,bForce);
        putchar(0x0a);
        print_bd(&curr_game);
        print_special_moves(&curr_game);
        bPrintedBoard = true;
      }
    }
  }
  else {
    if (quiz_number != -1) {
      initial_move = (quiz_number - 1) * 2;

      if (bBlack)
        initial_move++;

      if (initial_move >= curr_game.num_moves) {
        printf("initial_move must be < %d\n",curr_game.num_moves);
        return 10;
      }

      set_initial_board(&curr_game);
      curr_game.curr_move = 0;

      for (n = 0; n <= initial_move; n++) {
        update_board(&curr_game,false);
        curr_game.curr_move++;
      }
    }

    if (force_diff >= min_force_diff) {
      curr_game.curr_move--;
      printf("curr_move = %d\n",curr_game.curr_move);
      print_space_and_force(&curr_game,bSpace,bForce);
      putchar(0x0a);
      print_bd(&curr_game);
      print_special_moves(&curr_game);
    }
  }

  if (bBoardBin)
    write_board_to_binfile(curr_game.board,&argv[board_bin_arg][10]);

  if (bPrintPieces)
    print_pieces(&curr_game);

  return 0;
}

void print_space_and_force(struct game *game_pt,bool bSpace,bool bForce)
{
  if (bSpace) {
    calculate_seirawan_counts(game_pt);

    printf("Space: %d - %d\n",
      game_pt->seirawan_count[0],game_pt->seirawan_count[1]);
  }

  if (bForce) {
    refresh_force_count(game_pt);

    printf("Force: %d - %d\n",
      game_pt->force_count[0],game_pt->force_count[1]);
  }
}
