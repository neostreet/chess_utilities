#include <stdio.h>
#include <string.h>

#include "chess.h"
#define MAKE_GLOBALS_HERE
#include "chess.glb"
#include "chess.fun"
#include "chess.mac"

static char usage[] =
"usage: legal_moves (-debug) (-binary_format) filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bDebug;
  bool bBinaryFormat;
  int retval;
  struct game curr_game;

  if ((argc < 2) || (argc > 4)) {
    printf(usage);
    return 1;
  }

  bDebug = false;
  bBinaryFormat = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = true;
    else if (!strcmp(argv[curr_arg],"-binary_format"))
      bBinaryFormat = true;
    else
      break;
  }

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  if (!bBinaryFormat) {
    retval = read_game(argv[curr_arg],&curr_game,err_msg);

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

  legal_moves_count =  0;

  get_legal_moves(&curr_game,legal_moves,&legal_moves_count);

  printf("%d legal_moves:\n\n",legal_moves_count);

  for (n = 0; n < legal_moves_count; n++) {
    printf("from: %c%c to: %c%c\n",
    'a' + FILE_OF(legal_moves[n].from),'1' + RANK_OF(legal_moves[n].from),
    'a' + FILE_OF(legal_moves[n].to),'1' + RANK_OF(legal_moves[n].to));
  }

  return 0;
}
