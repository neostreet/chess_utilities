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
"usage: print_bd (-debug) (-toggle) (-space_and_force) (-initial_boardfilename)\n"
"  (-board_binfilename) (-qnn [white | black]) filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
int populate_initial_board_from_board_file(char *filename);

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bDebug;
  bool bToggle;
  bool bSpaceAndForce;
  bool bBoardBin;
  int board_bin_arg;
  int quiz_number;
  bool bBlack;
  int initial_move;
  int retval;
  struct game curr_game;

  if ((argc < 2) || (argc > 9)) {
    printf(usage);
    return 1;
  }

  bDebug = false;
  bToggle = false;
  bSpaceAndForce = false;
  quiz_number = -1;
  bBoardBin = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = true;
    else if (!strcmp(argv[curr_arg],"-toggle"))
      bToggle = true;
    else if (!strcmp(argv[curr_arg],"-space_and_force"))
      bSpaceAndForce = true;
    else if (!strncmp(argv[curr_arg],"-initial_board",14)) {
      retval = populate_initial_board_from_board_file(&argv[curr_arg][14]);

      if (retval) {
        printf("populate_initial_board_from_board_file() failed: %d\n",retval);
        return 2;
      }
    }
    else if (!strncmp(argv[curr_arg],"-board_bin",10)) {
      bBoardBin = true;
      board_bin_arg = curr_arg;
    }
    else if (!strncmp(argv[curr_arg],"-qn",3))
      sscanf(&argv[curr_arg][3],"%d",&quiz_number);
    else
      break;
  }

  if (quiz_number != -1) {
    if (argc - curr_arg != 2) {
      printf(usage);
      return 3;
    }

    if (!strcmp(argv[curr_arg],"white"))
      bBlack = false;
    else if (!strcmp(argv[curr_arg],"black"))
      bBlack = true;
    else {
      printf(usage);
      return 4;
    }
  }
  else {
    if (argc - curr_arg != 1) {
      printf(usage);
      return 5;
    }
  }

  retval = read_game(argv[argc-1],&curr_game,err_msg);

  if (retval) {
    printf("read_game of %s failed: %d\n",argv[argc-1],retval);
    printf("curr_move = %d\n",curr_game.curr_move);
    return 6;
  }

  if (bToggle)
    curr_game.orientation ^= 1;

  if (bDebug) {
    for (n = 0; n < curr_game.num_moves; n++) {
      printf("%2d %2d\n",curr_game.moves[n].from,curr_game.moves[n].to);
    }

    set_initial_board(&curr_game);
    curr_game.curr_move = 0;
    putchar(0x0a);
    print_bd(&curr_game);

    for (n = 0; n < curr_game.num_moves; n++) {
      update_board(&curr_game,false);
      curr_game.curr_move++;

      putchar(0x0a);
      print_bd(&curr_game);
    }
  }
  else if (quiz_number != -1) {
    initial_move = (quiz_number - 1) * 2;

    if (bBlack)
      initial_move++;

    if (initial_move > curr_game.num_moves) {
      printf("initial_move must be <= %d\n",curr_game.num_moves);
      return 7;
    }

    set_initial_board(&curr_game);
    curr_game.curr_move = 0;

    for (n = 0; n < initial_move; n++) {
      update_board(&curr_game,false);
      curr_game.curr_move++;
    }

    if (bSpaceAndForce) {
      calculate_seirawan_counts(&curr_game);
      refresh_force_count(&curr_game);

      printf("Space: %d - %d\n",
        curr_game.seirawan_count[0],curr_game.seirawan_count[1]);
      printf("Force: %d - %d\n",
        curr_game.force_count[0],curr_game.force_count[1]);
    }

    putchar(0x0a);
    print_bd(&curr_game);
  }
  else {
    if (bSpaceAndForce) {
      refresh_force_count(&curr_game);

      printf("Space: %d - %d\n",
        curr_game.seirawan_count[0],curr_game.seirawan_count[1]);
      printf("Force: %d - %d\n",
        curr_game.force_count[0],curr_game.force_count[1]);
    }

    putchar(0x0a);
    print_bd(&curr_game);
  }

  if (bBoardBin)
    write_board_to_binfile(curr_game.board,&argv[board_bin_arg][10]);

  return 0;
}
