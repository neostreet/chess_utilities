#include <vector>

using namespace std;

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
"usage: fenemy_king_square (-verbose) filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bVerbose;
  bool bBlack;
  int retval;
  struct game curr_game;
  FILE *fptr;
  int filename_len;
  int row;
  int column;

  if ((argc < 2) || (argc > 3)) {
    printf(usage);
    return 1;
  }

  bVerbose = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else
      break;
  }

  if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    return 2;
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

  if (!get_enemy_king_row_and_column(&curr_game,&row,&column)) {
    printf("get_enemy_king_row_and_column() of %s failed\n",filename);
    return 3;
  }

  if (!bVerbose) {
    printf("%c%d\n",'a' + row,column + 1);
  }
  else {
    printf("%c%d %s\n",'a' + row,column + 1,filename);
  }

  }

  fclose(fptr);

  return 0;
}
