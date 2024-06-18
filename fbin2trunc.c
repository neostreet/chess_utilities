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
"usage: fbin2trunc (-debug) (-alternate_colors) num_moves filename\n";

static struct game curr_game;

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

static int build_trunc_filename(
  char *bin_filename,
  int bin_filename_len,
  char *trunc_filename,
  int max_filename_len);

int main(int argc,char **argv)
{
  int curr_arg;
  bool bDebug;
  bool bAlternateColors;
  int num_moves;
  FILE *fptr0;
  int file_len;
  int bin_filename_len;
  int retval;
  int file_no;

  if ((argc < 3) || (argc > 5)) {
    printf(usage);
    return 1;
  }

  bDebug = false;
  bAlternateColors = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = true;
    else if (!strcmp(argv[curr_arg],"-alternate_colors"))
      bAlternateColors = true;
    else
      break;
  }

  if (argc - curr_arg != 2) {
    printf(usage);
    return 2;
  }

  sscanf(argv[curr_arg],"%d",&num_moves);

  if ((fptr0 = fopen(argv[curr_arg+1],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg+1]);
    return 3;
  }

  file_no = 0;

  for ( ; ; ) {
    GetLine(fptr0,filename,&file_len,MAX_FILENAME_LEN);

    if (feof(fptr0))
      break;

    file_no++;

    bin_filename_len = strlen(filename);

    retval = build_trunc_filename(filename,bin_filename_len,trunc_filename,MAX_FILENAME_LEN);

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

    if (curr_game.num_moves < num_moves) {
      printf("can't truncate %s to %d moves, as it only contains %d\n",filename,num_moves,curr_game.num_moves);
      continue;
    }

    if (!bAlternateColors) {
      curr_game.num_moves = num_moves;
      curr_game.curr_move = num_moves;
      curr_game.orientation = (num_moves % 2);
    }
    else {
      if (!((file_no - 1) % 2)) {
        curr_game.num_moves = num_moves;
        curr_game.curr_move = num_moves;
        curr_game.orientation = (num_moves % 2);
      }
      else {
        curr_game.num_moves = num_moves - 1;
        curr_game.curr_move = num_moves - 1;
        curr_game.orientation = ((num_moves - 1) % 2);
      }
    }

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
  int max_filename_len)
{
  int n;

  for (n = 0; n < bin_filename_len; n++) {
    if (bin_filename[n] == '.')
      break;
  }

  if (n == bin_filename_len)
    return 1;

  if (n + 6 > max_filename_len - 1)
    return 2;

  strncpy(trunc_filename,bin_filename,n);
  strcpy(&trunc_filename[n],".trunc");

  return 0;
}
