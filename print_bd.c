#include <stdio.h>
#include <string.h>

#include "chess.h"
#define MAKE_GLOBALS_HERE
#include "chess.glb"
#include "chess.fun"
#include "chess.mac"

static char usage[] =
"usage: print_bd (-debug) (-toggle) (-space) (-force) (-initial_boardfilename)\n"
"  (-init_bin_boardfilename) (-board_binfilename) (-num_white_piecesnum) (-num_black_piecesnum) (-print_pieces)\n"
"  (-min_force_diffvalue) (-match_boardfilename) (-only_checks) (-only_castle)\n"
"  (-only_promotions) (-only_captures) (-only_en_passants) (-multiple_queens) (-move_number_only)\n"
"  (-mine) (-not_mine) (-search_all_moves) (-exact_match) (-qnn) [white | black] filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

static unsigned char match_board1[CHARS_IN_BOARD];

void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  int orientation;
  bool bDebug;
  bool bSearchAllMoves;
  bool bExactMatch;
  bool bToggle;
  bool bSpace;
  bool bForce;
  bool bBoardBin;
  int num_white_pieces;
  int num_black_pieces;
  bool bPrintPieces;
  bool bOnlyChecks;
  bool bOnlyCastle;
  bool bOnlyPromotions;
  bool bOnlyCaptures;
  bool bOnlyEnPassants;
  bool bMultipleQueens;
  bool bMoveNumberOnly;
  bool bMine;
  bool bNotMine;
  bool bHaveMatchBoard;
  bool bPrintedBoard;
  bool bSkip;
  int board_bin_arg;
  int quiz_number;
  int min_force_diff;
  int force_diff;
  bool bBlack;
  int initial_move;
  int retval;
  struct game curr_game;
  int match;
  int num_pieces;

  if ((argc < 2) || (argc > 26)) {
    printf(usage);
    return 1;
  }

  bDebug = false;
  bSearchAllMoves = false;
  bExactMatch = false;
  bToggle = false;
  bSpace = false;
  bForce = false;
  quiz_number = -1;
  bBoardBin = false;
  num_white_pieces = -1;
  num_black_pieces = -1;
  bPrintPieces = false;
  bOnlyChecks = false;
  bOnlyCastle = false;
  bOnlyPromotions = false;
  bOnlyCaptures = false;
  bOnlyEnPassants = false;
  bMultipleQueens = false;
  bMoveNumberOnly = false;
  bMine = false;
  bNotMine = false;
  min_force_diff = -1;
  force_diff = 0;
  bHaveMatchBoard = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = true;
    else if (!strcmp(argv[curr_arg],"-search_all_moves"))
      bSearchAllMoves = true;
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
    else if (!strncmp(argv[curr_arg],"-num_white_pieces",17))
      sscanf(&argv[curr_arg][17],"%d",&num_white_pieces);
    else if (!strncmp(argv[curr_arg],"-num_black_pieces",17))
      sscanf(&argv[curr_arg][17],"%d",&num_black_pieces);
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
    }
    else if (!strcmp(argv[curr_arg],"-only_checks"))
      bOnlyChecks = true;
    else if (!strcmp(argv[curr_arg],"-only_castle"))
      bOnlyCastle = true;
    else if (!strcmp(argv[curr_arg],"-only_promotions"))
      bOnlyPromotions = true;
    else if (!strcmp(argv[curr_arg],"-only_captures"))
      bOnlyCaptures = true;
    else if (!strcmp(argv[curr_arg],"-only_en_passants"))
      bOnlyEnPassants = true;
    else if (!strcmp(argv[curr_arg],"-multiple_queens"))
      bMultipleQueens = true;
    else if (!strcmp(argv[curr_arg],"-move_number_only"))
      bMoveNumberOnly = true;
    else if (!strcmp(argv[curr_arg],"-mine"))
      bMine = true;
    else if (!strcmp(argv[curr_arg],"-not_mine"))
      bNotMine = true;
    else if (!strcmp(argv[curr_arg],"-exact_match"))
      bExactMatch = true;
    else
      break;
  }

  if (bOnlyChecks && bOnlyCastle) {
    printf("can't specify both -only_checks and -only_castle\n");
    return 5;
  }

  if (bMine && bNotMine) {
    printf("can't specify both -mine and -not_mine\n");
    return 6;
  }

  if (quiz_number != -1) {
    if (argc - curr_arg != 2) {
      printf(usage);
      return 7;
    }

    if (!strcmp(argv[curr_arg],"white"))
      bBlack = false;
    else if (!strcmp(argv[curr_arg],"black"))
      bBlack = true;
    else {
      printf(usage);
      return 8;
    }
  }
  else {
    if (argc - curr_arg != 1) {
      printf(usage);
      return 9;
    }
  }

  if (min_force_diff != -1) {
    bForce = true;
  }

  retval = read_game(argv[argc-1],&curr_game,err_msg);

  if (retval) {
    printf("read_game of %s failed: %d\n",argv[argc-1],retval);
    printf("curr_move = %d\n",curr_game.curr_move);
    return 10;
  }

  curr_game.curr_move--;
  orientation = curr_game.orientation;

  if (bToggle)
    curr_game.orientation ^= 1;

  if (bSearchAllMoves) {
    set_initial_board(&curr_game);
    curr_game.curr_move = 0;
    bPrintedBoard = false;

    for (curr_game.curr_move = 0; curr_game.curr_move < curr_game.num_moves; curr_game.curr_move++) {
      update_board(&curr_game,false);

      if (bMine) {
        if ((curr_game.curr_move % 2) != orientation)
          continue;
      }
      else if (bNotMine) {
        if ((curr_game.curr_move % 2) == orientation)
          continue;
      }

      if (num_white_pieces != -1) {
        num_pieces = count_num_pieces(WHITE,&curr_game);

        if (num_pieces != num_white_pieces)
          continue;
      }

      if (num_black_pieces != -1) {
        num_pieces = count_num_pieces(BLACK,&curr_game);

        if (num_pieces != num_black_pieces)
          continue;
      }

      if (min_force_diff != -1) {
        force_diff = refresh_force_count(&curr_game);

        if (force_diff < min_force_diff)
          continue;
      }

      if (bHaveMatchBoard) {
        match = match_board(curr_game.board,match_board1,bExactMatch);

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

      if (bOnlyEnPassants) {
        if (!(curr_game.moves[curr_game.curr_move].special_move_info & SPECIAL_MOVE_EN_PASSANT))
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
        putchar(0x0a);
        print_bd(&curr_game);
        print_special_moves(&curr_game);
        bPrintedBoard = true;
      }
    }
  }
  else {
    bPrintedBoard = false;

    if (quiz_number != -1) {
      initial_move = (quiz_number - 1) * 2;

      if (bBlack)
        initial_move++;

      if (initial_move >= curr_game.num_moves) {
        printf("initial_move must be < %d\n",curr_game.num_moves);
        return 11;
      }

      set_initial_board(&curr_game);
      curr_game.curr_move = 0;

      for (n = 0; n <= initial_move; n++) {
        update_board(&curr_game,false);

        if (n < initial_move)
          curr_game.curr_move++;
      }
    }

    bSkip = false;

    if (bMine) {
      if ((curr_game.curr_move % 2) != orientation)
        bSkip = true;
    }
    else if (bNotMine) {
      if ((curr_game.curr_move % 2) == orientation)
        bSkip = true;
    }

    if (!bSkip) {
      if (num_white_pieces != -1) {
        num_pieces = count_num_pieces(WHITE,&curr_game);

        if (num_pieces != num_white_pieces)
          bSkip = true;
      }
    }

    if (!bSkip) {
      if (num_black_pieces != -1) {
        num_pieces = count_num_pieces(BLACK,&curr_game);

        if (num_pieces != num_black_pieces)
          bSkip = true;
      }
    }

    if (!bSkip) {
      if (force_diff < min_force_diff)
        bSkip = true;
    }

    if (!bSkip && bHaveMatchBoard) {
      match = match_board(curr_game.board,match_board1,bExactMatch);

      if (!match)
        bSkip = true;
    }

    if (!bSkip && bOnlyChecks) {
      if (!(curr_game.moves[curr_game.curr_move].special_move_info & SPECIAL_MOVE_CHECK))
        bSkip = true;
    }

    if (!bSkip && bOnlyCastle) {
      if (!(curr_game.moves[curr_game.curr_move].special_move_info & SPECIAL_MOVE_CASTLE))
        bSkip = true;
    }

    if (!bSkip && bOnlyPromotions) {
      if (!(curr_game.moves[curr_game.curr_move].special_move_info & (SPECIAL_MOVE_PROMOTION_QUEEN | SPECIAL_MOVE_PROMOTION_ROOK | SPECIAL_MOVE_PROMOTION_KNIGHT | SPECIAL_MOVE_PROMOTION_BISHOP)))
        bSkip = true;
    }

    if (!bSkip && bOnlyCaptures) {
      if (!(curr_game.moves[curr_game.curr_move].special_move_info & SPECIAL_MOVE_CAPTURE))
        bSkip = true;
    }

    if (!bSkip && bOnlyEnPassants) {
      if (!(curr_game.moves[curr_game.curr_move].special_move_info & SPECIAL_MOVE_EN_PASSANT))
        bSkip = true;
    }

    if (!bSkip && bMultipleQueens) {
      if (!multiple_queens((unsigned char *)&curr_game.board))
        bSkip = true;
    }

    if (!bSkip) {
      printf("curr_move = %d\n",curr_game.curr_move);
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
