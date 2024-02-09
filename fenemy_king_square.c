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
"usage: fenemy_king_square (-verbose) (-board_offset) filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bVerbose;
  bool bBoardOffset;
  bool bBlack;
  int retval;
  struct game curr_game;
  FILE *fptr;
  int filename_len;
  int file;
  int rank;
  int board_offset;

  if ((argc < 2) || (argc > 4)) {
    printf(usage);
    return 1;
  }

  bVerbose = false;
  bBoardOffset = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else if (!strcmp(argv[curr_arg],"-board_offset"))
      bBoardOffset = true;
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

  for ( ; ; ) {

  GetLine(fptr,filename,&filename_len,MAX_FILENAME_LEN);

  if (feof(fptr))
    break;

  retval = read_game(filename,&curr_game,err_msg);

  if (retval) {
    printf("read_game of %s failed: %d\n",filename,retval);
    printf("curr_move = %d\n",curr_game.curr_move);

    continue;
  }

  if (!get_enemy_king_file_and_rank(&curr_game,&file,&rank)) {
    printf("get_enemy_king_file_and_rank() of %s failed\n",filename);
    return 4;
  }

  if (bBoardOffset)
    board_offset = rank * 8 + file;

  if (!bVerbose) {
    if (!bBoardOffset)
      printf("%c%d\n",'a' + file,rank + 1);
    else
      printf("%d\n",board_offset);
  }
  else {
    if (!bBoardOffset)
      printf("%c%d %s\n",'a' + file,rank + 1,filename);
    else
      printf("%d %s\n",board_offset,filename);
  }

  }

  fclose(fptr);

  return 0;
}
