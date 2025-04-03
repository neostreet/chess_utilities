#include <stdio.h>
#include <string.h>

#include "chess.h"
#define MAKE_GLOBALS_HERE
#include "chess.glb"
#include "chess.fun"
#include "chess.mac"

#define MAX_FILENAME_LEN 256
static char filename[MAX_FILENAME_LEN];

#define MAX_GAMES 100
struct game games[MAX_GAMES];

static char usage[] =
"usage: find_doppelgames (-binary_format) filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bBinaryFormat;
  int retval;
  FILE *fptr;
  int filename_len;
  int game_ix;

  if ((argc < 2) || (argc > 3)) {
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

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    return 3;
  }

  for (game_ix = 0; game_ix < MAX_GAMES; game_ix++) {
    GetLine(fptr,filename,&filename_len,MAX_FILENAME_LEN);

    if (feof(fptr))
      break;

    bzero(&games[game_ix],sizeof (struct game));

    if (!bBinaryFormat) {
      retval = read_game(filename,&games[game_ix]);

      if (retval) {
        printf("read_game of %s failed: %d\n",filename,retval);

        continue;
      }
    }
    else {
      retval = read_binary_game(filename,&games[game_ix]);

      if (retval) {
        printf("read_binary_game of %s failed: %d\n",filename,retval);

        continue;
      }
    }
  }

  fclose(fptr);

  for (n = 0; n < game_ix; n++)
    printf("%d %s\n",games[n].result,games[n].title);

  return 0;
}
