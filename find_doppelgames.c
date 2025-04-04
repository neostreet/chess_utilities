#include <stdio.h>
#include <string.h>

#include "chess.h"
#define MAKE_GLOBALS_HERE
#include "chess.glb"
#include "chess.fun"
#include "chess.mac"

#define MAX_GAMES 100
struct game games[MAX_GAMES];

#define MAX_FILENAME_LEN 256
static char filenames[MAX_GAMES][MAX_FILENAME_LEN];

static char usage[] =
"usage: find_doppelgames (-binary_format) (-verbose) (-debug) filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

static int doppelgames_calls;
static int doppelgames_found;

bool doppelgames(struct game *gamept1,struct game *gamept2);

int main(int argc,char **argv)
{
  int m;
  int n;
  int curr_arg;
  bool bBinaryFormat;
  bool bVerbose;
  bool bDebug;
  int retval;
  FILE *fptr;
  int filename_len;
  int game_ix;

  if ((argc < 2) || (argc > 5)) {
    printf(usage);
    return 1;
  }

  bBinaryFormat = false;
  bVerbose = false;
  bDebug = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-binary_format"))
      bBinaryFormat = true;
    else if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = true;
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

  for (game_ix = 0; game_ix < MAX_GAMES; game_ix++) {
    GetLine(fptr,filenames[game_ix],&filename_len,MAX_FILENAME_LEN);

    if (feof(fptr))
      break;

    bzero(&games[game_ix],sizeof (struct game));

    if (!bBinaryFormat) {
      retval = read_game(filenames[game_ix],&games[game_ix]);

      if (retval) {
        printf("read_game of %s failed: %d\n",filenames[game_ix],retval);

        continue;
      }
    }
    else {
      retval = read_binary_game(filenames[game_ix],&games[game_ix]);

      if (retval) {
        printf("read_binary_game of %s failed: %d\n",filenames[game_ix],retval);

        continue;
      }
    }
  }

  fclose(fptr);

  for (n = 0; n < game_ix - 1; n++) {
    if (doppelgames_found)
        break;

    for (m = n + 1; m < game_ix; m++) {
      if (doppelgames(&games[n],&games[m])) {
        if (!bVerbose)
          printf("%s %s\n",filenames[n],filenames[m]);
        else
          printf("%s and %s are doppelgames\n",filenames[n],filenames[m]);
      }
      else if (bVerbose)
        printf("%s and %s are not doppelgames\n",filenames[n],filenames[m]);
    }
  }

  if (bDebug)
    printf("doppelgames() called %d times; found %d doppelgames\n",doppelgames_calls,doppelgames_found);

  return 0;
}

bool doppelgames(struct game *gamept1,struct game *gamept2)
{
  int n;

  doppelgames_calls++;

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

  doppelgames_found++;

  return true;
}
