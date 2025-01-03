#include <stdio.h>
#include <string.h>

#define MAX_FILENAME_LEN 1024

static char filename[MAX_FILENAME_LEN];
static char trunc_filename[MAX_FILENAME_LEN];

#define MAX_LINE_LEN 8192
static char line[MAX_LINE_LEN];

#include "chess.h"
#define MAKE_GLOBALS_HERE
#include "chess.glb"
#include "chess.fun"
#include "chess.mac"

static char usage[] =
"usage: fbin2trunc2 (-debug) moves_to_truncate filename extension\n";

static struct game curr_game;

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

static int build_trunc_filename(
  char *bin_filename,
  int bin_filename_len,
  char *trunc_filename,
  int max_filename_len,
  char *extension);

int main(int argc,char **argv)
{
  int curr_arg;
  bool bDebug;
  int moves_to_truncate;
  FILE *fptr0;
  int file_len;
  int bin_filename_len;
  int retval;

  if ((argc < 4) || (argc > 5)) {
    printf(usage);
    return 1;
  }

  bDebug = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = true;
    else
      break;
  }

  if (argc - curr_arg != 3) {
    printf(usage);
    return 2;
  }

  sscanf(argv[curr_arg],"%d",&moves_to_truncate);

  if ((fptr0 = fopen(argv[curr_arg+1],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg+1]);
    return 3;
  }

  for ( ; ; ) {
    GetLine(fptr0,filename,&file_len,MAX_FILENAME_LEN);

    if (feof(fptr0))
      break;

    bin_filename_len = strlen(filename);

    retval = build_trunc_filename(filename,bin_filename_len,trunc_filename,MAX_FILENAME_LEN,argv[curr_arg+2]);

    if (retval) {
      printf("build_trunc_filename failed on %s: %d\n",filename,retval);
      continue;
    }

    bzero(&curr_game,sizeof (struct game));

    retval = read_binary_game(filename,&curr_game);

    if (retval) {
      printf("read_binary_game of %s failed: %d\n",filename,retval);
      printf("curr_move = %d\n",curr_game.curr_move);
      continue;
    }

    if (curr_game.num_moves < moves_to_truncate) {
      printf("can't truncate %s by %d moves, as it only contains %d\n",filename,moves_to_truncate,curr_game.num_moves);
      continue;
    }

    curr_game.num_moves -= moves_to_truncate;
    curr_game.curr_move = curr_game.num_moves;
    curr_game.orientation = curr_game.num_moves % 2;

    retval = write_binary_game(trunc_filename,&curr_game);

    if (retval) {
      printf("write_binary_game of %s failed: %d\n",trunc_filename,retval);
      continue;
    }
  }

  return 0;
}

static int build_trunc_filename(
  char *bin_filename,
  int bin_filename_len,
  char *trunc_filename,
  int max_filename_len,
  char *extension)
{
  int n;
  int ext_len;

  ext_len = strlen(extension) + 1;

  for (n = 0; n < bin_filename_len; n++) {
    if (bin_filename[n] == '.')
      break;
  }

  if (n == bin_filename_len)
    return 1;

  if (n + ext_len > max_filename_len - 1)
    return 2;

  sprintf(trunc_filename,"%s.%s",bin_filename,extension);

  return 0;
}
