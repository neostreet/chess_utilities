#include <stdio.h>
#include <string.h>

#include "chess.h"
#define MAKE_GLOBALS_HERE
#include "chess.glb"
#include "chess.fun"
#include "chess.mac"

static char usage[] =
"usage: doppelgames (-binary_format) game1 game2\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

bool doppelgames(struct game *gamept1,struct game *gamept2);

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bBinaryFormat;
  struct game game1;
  struct game game2;
  int retval;

  if ((argc < 2) || (argc > 4)) {
    printf(usage);
    return 1;
  }

  bBinaryFormat = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-binary_format"))
      bBinaryFormat = true;
    else
      break;
  }

  if (argc - curr_arg != 2) {
    printf(usage);
    return 2;
  }

  bzero(&game1,sizeof (struct game));
  bzero(&game2,sizeof (struct game));

  if (!bBinaryFormat) {
    retval = read_game(argv[curr_arg],&game1);

    if (retval) {
      printf("read_game of %s failed: %d\n",argv[curr_arg],retval);

      return 3;
    }

    retval = read_game(argv[curr_arg+1],&game2);

    if (retval) {
      printf("read_game of %s failed: %d\n",argv[curr_arg+1],retval);

      return 4;
    }
  }
  else {
    retval = read_binary_game(argv[curr_arg],&game1);

    if (retval) {
      printf("read_binary_game of %s failed: %d\n",argv[curr_arg],retval);

      return 5;
    }

    retval = read_binary_game(argv[curr_arg+1],&game2);

    if (retval) {
      printf("read_binary_game of %s failed: %d\n",argv[curr_arg+1],retval);

      return 6;
    }
  }

  if (doppelgames(&game1,&game2))
    printf("%s and %s are doppelgames\n",argv[curr_arg],argv[curr_arg+1]);
  else
    printf("%s and %s are not doppelgames\n",argv[curr_arg],argv[curr_arg+1]);

  return 0;
}

bool doppelgames(struct game *gamept1,struct game *gamept2)
{
  int n;

  if (gamept1->orientation != gamept2->orientation)
    return false;

  if (gamept1->result != gamept2->result)
    return false;

  if (gamept1->num_moves != gamept2->num_moves)
    return false;

  for (n = 0; n < gamept1->num_moves; n++) {
    if (gamept1->moves[n].from != gamept2->moves[n].from)
      return false;

    if (gamept1->moves[n].to != gamept2->moves[n].to)
      return false;

    if (gamept1->moves[n].special_move_info != gamept2->moves[n].special_move_info)
      return false;
  }

  return true;
}
