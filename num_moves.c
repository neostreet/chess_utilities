#include <stdio.h>
#include <string.h>

#include "chess.h"
#define MAKE_GLOBALS_HERE
#include "chess.glb"
#include "chess.fun"
#include "chess.mac"

#define MAX_FILENAME_LEN 256
static char filename[MAX_FILENAME_LEN];

static char usage[] =
"usage: num_moves (-binary_format) (-ge_num_movesnum_moves) (-eq_num_movesnum_moves) (-terse) filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bBinaryFormat;
  int num_moves;
  int ge_num_moves;
  int eq_num_moves;
  bool bTerse;
  int retval;
  FILE *fptr;
  int filename_len;
  struct game curr_game;

  if ((argc < 2) || (argc > 6)) {
    printf(usage);
    return 1;
  }

  bBinaryFormat = false;
  ge_num_moves = -1;
  eq_num_moves = -1;
  bTerse = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-binary_format"))
      bBinaryFormat = true;
    else if (!strncmp(argv[curr_arg],"-ge_num_moves",13))
      sscanf(&argv[curr_arg][13],"%d",&ge_num_moves);
    else if (!strncmp(argv[curr_arg],"-eq_num_moves",13))
      sscanf(&argv[curr_arg][13],"%d",&eq_num_moves);
    else if (!strcmp(argv[curr_arg],"-terse"))
      bTerse = true;
    else
      break;
  }

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  if ((ge_num_moves != -1) && (eq_num_moves != -1)) {
    printf("can't specify both -ge_num_moves and -eq_num_moves\n");
    return 3;
  }

  if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    return 4;
  }

  for ( ; ; ) {
    GetLine(fptr,filename,&filename_len,MAX_FILENAME_LEN);

    if (feof(fptr))
      break;

    if (!bBinaryFormat) {
      retval = read_game(filename,&curr_game,err_msg);

      if (retval) {
        printf("read_game of %s failed: %d\n",filename,retval);
        printf("curr_move = %d\n",curr_game.curr_move);

        continue;
      }
    }
    else {
      retval = read_binary_game(filename,&curr_game);

      if (retval) {
        printf("read_binary_game of %s failed: %d\n",filename,retval);
        printf("curr_move = %d\n",curr_game.curr_move);

        continue;
      }
    }

    num_moves = (curr_game.num_moves + 1) / 2;

    if (eq_num_moves != -1) {
      if (num_moves == eq_num_moves) {
        if (!bTerse)
          printf("%d %s\n",num_moves,filename);
        else
          printf("%s\n",filename);
      }
    }
    else if (num_moves >= ge_num_moves) {
      if (!bTerse)
        printf("%d %s\n",num_moves,filename);
      else
        printf("%s\n",filename);
    }
  }

  fclose(fptr);

  return 0;
}
