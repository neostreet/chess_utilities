#include <stdio.h>
#include <string.h>

#include "chess.h"
#define MAKE_GLOBALS_HERE
#include "chess.glb"
#include "chess.fun"
#include "chess.mac"

static char usage[] =
"usage: print_piece_info2 (-debug) (-binary_format) (-abbrev) (-only_remaining) filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int curr_arg;
  bool bDebug;
  bool bBinaryFormat;
  bool bAbbrev;
  bool bOnlyRemaining;
  int retval;
  struct game curr_game;

  if ((argc < 2) || (argc > 6)) {
    printf(usage);
    return 1;
  }

  bDebug = false;
  bBinaryFormat = false;
  bAbbrev = false;
  bOnlyRemaining = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = true;
    else if (!strcmp(argv[curr_arg],"-binary_format"))
      bBinaryFormat = true;
    else if (!strcmp(argv[curr_arg],"-abbrev"))
      bAbbrev = true;
    else if (!strcmp(argv[curr_arg],"-only_remaining"))
      bOnlyRemaining = true;
    else
      break;
  }

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  if (!bBinaryFormat) {
    retval = read_game(argv[curr_arg],&curr_game);

    if (retval) {
      printf("read_game of %s failed: %d\n",argv[curr_arg],retval);
      printf("curr_move = %d\n",curr_game.curr_move);

      return 3;
    }
  }
  else {
    retval = read_binary_game(argv[curr_arg],&curr_game);

    if (retval) {
      printf("read_binary_game of %s failed: %d\n",argv[curr_arg],retval);
      printf("curr_move = %d\n",curr_game.curr_move);

      return 4;
    }
  }

  printf("%s\n",argv[curr_arg]);
  putchar(0x0a);
  print_bd(&curr_game);
  putchar(0x0a);

  printf("White:\n");
  print_piece_info2(curr_game.white_pieces,true,bAbbrev,bOnlyRemaining);

  printf("Black:\n");
  print_piece_info2(curr_game.black_pieces,false,bAbbrev,bOnlyRemaining);

  return 0;
}
