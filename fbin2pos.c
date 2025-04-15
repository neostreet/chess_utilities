#include <stdio.h>
#include <string.h>

#define MAX_FILENAME_LEN 1024

static char filename[MAX_FILENAME_LEN];
static char pos_filename[MAX_FILENAME_LEN];

#define MAX_LINE_LEN 8192
static char line[MAX_LINE_LEN];

#include "chess.h"
#define MAKE_GLOBALS_HERE
#include "chess.glb"
#include "chess.fun"
#include "chess.mac"

static char usage[] =
"usage: fbin2pos (-debug) filename\n";

static struct game curr_game;
static struct game_position curr_position;

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

static int build_pos_filename(
  char *bin_filename,
  int bin_filename_len,
  char *pos_filename,
  int max_filename_len);

int main(int argc,char **argv)
{
  int curr_arg;
  bool bDebug;
  FILE *fptr0;
  int file_len;
  int bin_filename_len;
  int retval;

  if ((argc < 2) || (argc > 3)) {
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

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  if ((fptr0 = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    return 3;
  }

  for ( ; ; ) {
    GetLine(fptr0,filename,&file_len,MAX_FILENAME_LEN);

    if (feof(fptr0))
      break;

    bin_filename_len = strlen(filename);

    retval = build_pos_filename(filename,bin_filename_len,pos_filename,MAX_FILENAME_LEN);

    if (retval) {
      printf("build_pos_filename failed on %s: %d\n",filename,retval);
      continue;
    }

    bzero(&curr_game,sizeof (struct game));

    retval = read_binary_game(filename,&curr_game);

    if (retval) {
      printf("read_binary_game of %s failed: %d\n",filename,retval);
      printf("curr_move = %d\n",curr_game.curr_move);
      continue;
    }

    curr_position.orientation = curr_game.orientation;
    copy_board(curr_game.board,curr_position.board);

    retval = write_game_position(pos_filename,&curr_position);

    if (retval) {
      printf("write_game_position of %s failed: %d\n",filename,retval);
      continue;
    }
  }

  return 0;
}

static int build_pos_filename(
  char *bin_filename,
  int bin_filename_len,
  char *pos_filename,
  int max_filename_len)
{
  if (bin_filename_len + 4 >= max_filename_len)
    return 1;

  sprintf(pos_filename,"%s.pos",bin_filename);

  return 0;
}
