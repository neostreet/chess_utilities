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
"  (-min_force_diffvalue) (-match_boardfilename) filename\n"
"  (-qnn) [white | black] filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

static unsigned char match_board[CHARS_IN_BOARD];

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
  bool bHaveMatchBoard;
  int board_bin_arg;
  int quiz_number;
  int min_force_diff;
  int force_diff;
  bool bBlack;
  int initial_move;
  int retval;
  struct game curr_game;

  if ((argc < 2) || (argc > 13)) {
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
        printf("populate_initial_board_from_board_file() failed: %d\n",retval);
        return 2;
      }
    }
    else if (!strncmp(argv[curr_arg],"-init_bin_board",15)) {
      retval = populate_initial_board_from_bin_board_file(&argv[curr_arg][15]);

      if (retval) {
        printf("populate_initial_board_from_bin_board_file() failed: %d\n",retval);
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
      retval = populate_board_from_board_file(match_board,&argv[curr_arg][12]);

      if (retval) {
        printf("populate_board_from_board_file() failed: %d\n",retval);
        return 4;
      }
    }
    else
      break;
  }

  if (quiz_number != -1) {
    if (argc - curr_arg != 2) {
      printf(usage);
      return 5;
    }

    if (!strcmp(argv[curr_arg],"white"))
      bBlack = false;
    else if (!strcmp(argv[curr_arg],"black"))
      bBlack = true;
    else {
      printf(usage);
      return 6;
    }
  }
  else {
    if (argc - curr_arg != 1) {
      printf(usage);
      return 7;
    }
  }

  if (min_force_diff != -1) {
    bForce = true;
    bDebug = true;
  }

  retval = read_game(argv[argc-1],&curr_game,err_msg);

  if (retval) {
    printf("read_game of %s failed: %d\n",argv[argc-1],retval);
    printf("curr_move = %d\n",curr_game.curr_move);
    return 8;
  }

  if (bToggle)
    curr_game.orientation ^= 1;

  if (bDebug) {
    for (n = 0; n < curr_game.num_moves; n++) {
      printf("%2d %2d\n",curr_game.moves[n].from,curr_game.moves[n].to);
    }

    set_initial_board(&curr_game);
    curr_game.curr_move = 0;

    if (min_force_diff == -1) {
      printf("curr_move = %d\n",curr_game.curr_move);
      print_space_and_force(&curr_game,bSpace,bForce);
      putchar(0x0a);
      print_bd(&curr_game);
    }

    for (n = 0; n < curr_game.num_moves; n++) {
      update_board(&curr_game,false);
      curr_game.curr_move++;

      if (min_force_diff != -1) {
        force_diff = refresh_force_count(&curr_game);

        if (force_diff < min_force_diff)
          continue;
      }

      putchar(0x0a);
      printf("curr_move = %d\n",curr_game.curr_move);
      print_space_and_force(&curr_game,bSpace,bForce);
      putchar(0x0a);
      print_bd(&curr_game);
    }
  }
  else {
    if (quiz_number != -1) {
      initial_move = (quiz_number - 1) * 2;

      if (bBlack)
        initial_move++;

      if (initial_move > curr_game.num_moves) {
        printf("initial_move must be <= %d\n",curr_game.num_moves);
        return 9;
      }

      set_initial_board(&curr_game);
      curr_game.curr_move = 0;

      for (n = 0; n < initial_move; n++) {
        update_board(&curr_game,false);
        curr_game.curr_move++;
      }

      if (force_diff >= min_force_diff) {
        printf("curr_move = %d\n",curr_game.curr_move);
        print_space_and_force(&curr_game,bSpace,bForce);
        putchar(0x0a);
        print_bd(&curr_game);
        print_special_moves(&curr_game);
      }
    }
    else {
      printf("curr_move = %d\n",curr_game.curr_move);
      print_space_and_force(&curr_game,bSpace,bForce);
      putchar(0x0a);
      print_bd(&curr_game);
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
