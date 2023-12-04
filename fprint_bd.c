#include <vector>

using namespace std;

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
"usage: fprint_bd (-debug) (-terse) (-toggle) (-initial_boardfilename)\n"
"  (-init_bin_boardfilename) (-board_binfilename) (-num_white_piecesnum) (-num_black_piecesnum)\n"
"  (-match_boardfilename) (-match_forcefilename) (-only_checks) (-only_mates) (-only_castle)\n"
"  (-only_promotions) (-only_captures) (-only_en_passants) (-multiple_queens) (-move_number_only)\n"
"  (-mine) (-not_mine) (-search_all_moves) (-exact_match)\n"
"  (-white_force_countnum) (-black_force_countnum) (-qnn) [white | black] filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

static int afl_dbg;

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  int orientation;
  bool bDebug;
  bool bTerse;
  bool bSearchAllMoves;
  bool bExactMatch;
  bool bToggle;
  bool bBoardBin;
  int num_white_pieces;
  int num_black_pieces;
  int white_force_count;
  int black_force_count;
  bool bOnlyChecks;
  bool bOnlyMates;
  bool bOnlyCastle;
  bool bOnlyPromotions;
  bool bOnlyCaptures;
  bool bOnlyEnPassants;
  bool bMultipleQueens;
  bool bMoveNumberOnly;
  bool bMine;
  bool bNotMine;
  bool bHaveMatchBoard;
  bool bHaveMatchForce;
  bool bPrintedFilename;
  bool bPrintedBoard;
  bool bSkip;
  unsigned char match_board1[CHARS_IN_BOARD];
  unsigned char force_board1[CHARS_IN_BOARD];
  int match_piece_counts[NUM_PIECE_TYPES_0 * 2];
  int curr_piece_counts[NUM_PIECE_TYPES_0 * 2];
  int board_bin_arg;
  int quiz_number;
  bool bBlack;
  int initial_move;
  int retval;
  struct game curr_game;
  int match;
  FILE *fptr;
  int filename_len;
  int num_pieces;

  if ((argc < 2) || (argc > 27)) {
    printf(usage);
    return 1;
  }

  dbg_move = -1;
  bDebug = false;
  bTerse = false;
  bSearchAllMoves = false;
  bExactMatch = false;
  bToggle = false;
  quiz_number = -1;
  bBoardBin = false;
  num_white_pieces = -1;
  num_black_pieces = -1;
  white_force_count = -1;
  black_force_count = -1;
  bOnlyChecks = false;
  bOnlyMates = false;
  bOnlyCastle = false;
  bOnlyPromotions = false;
  bOnlyCaptures = false;
  bOnlyEnPassants = false;
  bMultipleQueens = false;
  bMoveNumberOnly = false;
  bMine = false;
  bNotMine = false;
  bHaveMatchBoard = false;
  bHaveMatchForce = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = true;
    else if (!strcmp(argv[curr_arg],"-terse"))
      bTerse = true;
    else if (!strcmp(argv[curr_arg],"-search_all_moves"))
      bSearchAllMoves = true;
    else if (!strcmp(argv[curr_arg],"-toggle"))
      bToggle = true;
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
    else if (!strncmp(argv[curr_arg],"-white_force_count",18))
      sscanf(&argv[curr_arg][18],"%d",&white_force_count);
    else if (!strncmp(argv[curr_arg],"-black_force_count",18))
      sscanf(&argv[curr_arg][18],"%d",&black_force_count);
    else if (!strncmp(argv[curr_arg],"-board_bin",10)) {
      bBoardBin = true;
      board_bin_arg = curr_arg;
    }
    else if (!strncmp(argv[curr_arg],"-qn",3))
      sscanf(&argv[curr_arg][3],"%d",&quiz_number);
    else if (!strncmp(argv[curr_arg],"-match_board",12)) {
      bHaveMatchBoard = true;
      retval = populate_board_from_bin_board_file(match_board1,&argv[curr_arg][12]);

      if (retval) {
        printf("populate_board_from_board_file() failed on %s: %d\n",
          &argv[curr_arg][12],retval);
        return 4;
      }
    }
    else if (!strncmp(argv[curr_arg],"-match_force",12)) {
      bHaveMatchForce = true;
      retval = populate_board_from_bin_board_file(force_board1,&argv[curr_arg][12]);

      if (retval) {
        printf("populate_board_from_board_file() failed on %s: %d\n",
          &argv[curr_arg][12],retval);
        return 5;
      }

      get_piece_counts(force_board1,match_piece_counts);
    }
    else if (!strcmp(argv[curr_arg],"-only_checks"))
      bOnlyChecks = true;
    else if (!strcmp(argv[curr_arg],"-only_mates"))
      bOnlyMates = true;
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
    return 6;
  }

  if (bMine && bNotMine) {
    printf("can't specify both -mine and -not_mine\n");
    return 7;
  }

  if (quiz_number != -1) {
    if (argc - curr_arg != 2) {
      printf(usage);
      return 8;
    }

    if (!strcmp(argv[curr_arg],"white"))
      bBlack = false;
    else if (!strcmp(argv[curr_arg],"black"))
      bBlack = true;
    else {
      printf(usage);
      return 9;
    }
  }
  else {
    if (argc - curr_arg != 1) {
      printf(usage);
      return 10;
    }
  }

  if ((fptr = fopen(argv[argc-1],"r")) == NULL) {
    printf(couldnt_open,argv[argc-1]);
    return 11;
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

  if (!bOnlyChecks && !bOnlyMates && !bOnlyCastle && !bOnlyPromotions && !bOnlyCaptures && !bOnlyEnPassants && !bMultipleQueens &&
    !bMine && !bNotMine && !bHaveMatchBoard && !bHaveMatchForce && (num_white_pieces == -1) &&
    (num_black_pieces == -1) && (white_force_count == -1) && (black_force_count == -1))
    printf("%s\n",filename);

  curr_game.curr_move--;
  orientation = curr_game.orientation;

  if (bToggle)
    curr_game.orientation ^= 1;

  if (bSearchAllMoves) {
    set_initial_board(&curr_game);
    curr_game.curr_move = 0;

    bPrintedFilename = false;
    bPrintedBoard = false;

    for (curr_game.curr_move = 0; curr_game.curr_move < curr_game.num_moves; curr_game.curr_move++) {
      update_board(curr_game.board,&curr_game.moves[curr_game.curr_move],curr_game.curr_move & 0x1);

      if (curr_game.curr_move == dbg_move)
        afl_dbg = 1;

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

      if (white_force_count != -1) {
        refresh_force_count(&curr_game);

        if (curr_game.force_count[WHITE] != white_force_count)
          continue;
      }

      if (black_force_count != -1) {
        refresh_force_count(&curr_game);

        if (curr_game.force_count[BLACK] != black_force_count)
          continue;
      }

      if (bHaveMatchBoard) {
        match = match_board(curr_game.board,match_board1,bExactMatch);

        if (!match)
          continue;
      }

      if (bHaveMatchForce) {
        get_piece_counts(curr_game.board,curr_piece_counts);

        if (!piece_counts_match(curr_piece_counts,match_piece_counts))
          continue;
      }

      if (bOnlyChecks) {
        if (!(curr_game.moves[curr_game.curr_move].special_move_info & SPECIAL_MOVE_CHECK))
          continue;
      }

      if (bOnlyMates) {
        if (!(curr_game.moves[curr_game.curr_move].special_move_info & SPECIAL_MOVE_MATE))
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

      if (bOnlyChecks || bOnlyMates || bOnlyCastle || bOnlyPromotions || bOnlyCaptures || bOnlyEnPassants || bMultipleQueens || bHaveMatchBoard || bHaveMatchForce || bMine || bNotMine) {
        if (!bPrintedFilename) {
          printf("%s\n",filename);

          if (bTerse)
            break;

          bPrintedFilename = true;
        }
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
        continue;
      }

      set_initial_board(&curr_game);
      curr_game.curr_move = 0;

      for (n = 0; n <= initial_move; n++) {
        update_board(curr_game.board,&curr_game.moves[curr_game.curr_move],curr_game.curr_move & 0x1);

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
      if (white_force_count != -1) {
        refresh_force_count(&curr_game);

        if (curr_game.force_count[WHITE] != white_force_count)
          bSkip = true;
      }
    }

    if (!bSkip) {
      if (black_force_count != -1) {
        refresh_force_count(&curr_game);

        if (curr_game.force_count[BLACK] != black_force_count)
          bSkip = true;
      }
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

    if (!bSkip && bOnlyMates) {
      if (!(curr_game.moves[curr_game.curr_move].special_move_info & SPECIAL_MOVE_MATE))
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
      if (bOnlyChecks || bOnlyMates || bOnlyCastle || bOnlyPromotions || bOnlyCaptures || bMultipleQueens || bHaveMatchBoard ||
        bMine || bNotMine || (num_white_pieces != -1) || (num_black_pieces != -1) ||
        (white_force_count != -1) || (black_force_count != -1)) {
        printf("%s\n",filename);
      }

      if (!bTerse) {
        printf("curr_move = %d\n",curr_game.curr_move);
        putchar(0x0a);
        print_bd(&curr_game);
        print_special_moves(&curr_game);
      }
    }
  }

  if (bBoardBin)
    write_board_to_binfile(curr_game.board,&argv[board_bin_arg][10]);

  if (bPrintedBoard)
    putchar(0x0a);

  }

  fclose(fptr);

  return 0;
}
