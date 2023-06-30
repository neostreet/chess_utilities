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
static char out_filename[MAX_FILENAME_LEN];

static char usage[] = "usage: fprint_force_diff (-debug) filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bDebug;
  FILE *fptr;
  FILE *out_fptr;
  int filename_len;
  int retval;
  struct game curr_game;

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

  if ((fptr = fopen(argv[argc-1],"r")) == NULL) {
    printf(couldnt_open,argv[argc-1]);
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

  sprintf(out_filename,"%s.force_diff",filename);

  if ((out_fptr = fopen(out_filename,"w")) == NULL) {
    printf(couldnt_open,out_filename);
    return 3;
  }

  set_initial_board(&curr_game);

  fprintf(out_fptr,"%d %d 0\n",curr_game.force_count[WHITE],curr_game.force_count[WHITE] - curr_game.force_count[BLACK]);

  for (curr_game.curr_move = 0; curr_game.curr_move < curr_game.num_moves; curr_game.curr_move++) {
    update_board(&curr_game,false);
    refresh_force_count(&curr_game);
    fprintf(out_fptr,"%d %d %d\n",curr_game.force_count[WHITE],curr_game.force_count[WHITE] - curr_game.force_count[BLACK],
      curr_game.curr_move + 1);
  }

  fclose(out_fptr);

  }

  return 0;
}
