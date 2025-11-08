#include <stdio.h>
#include <string.h>

#include "chess.h"
#define MAKE_GLOBALS_HERE
#include "chess.glb"
#include "chess.fun"
#include "chess.mac"

#define MAX_LINE_LEN 256
static char line[MAX_LINE_LEN];

static char usage[] =
"usage: fset_curr_move filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

int main(int argc,char **argv)
{
  int retval;
  FILE *fptr;
  int line_len;
  int line_no;
  int curr_move;
  int filename_offset;
  char *filename;
  struct game curr_game;

  if (argc != 2) {
    printf(usage);
    return 1;
  }

  if ((fptr = fopen(argv[1],"r")) == NULL) {
    printf(couldnt_open,argv[1]);
    return 2;
  }

  line_no = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

     line_no++;

    sscanf("%d",line,&curr_move);

    for (filename_offset = 0; filename_offset < line_len; filename_offset++) {
      if (line[filename_offset] == ' ') {
        filename_offset++;
        break;
      }
    }

    if (filename_offset == line_len) {
      printf("couldn't parse filename on line %d\n",line_no);
      continue;
    }

    filename = &line[filename_offset];

    bzero(&curr_game,sizeof (struct game));

    retval = read_game(filename,&curr_game);

    if (retval) {
      printf("read_game of %s failed: %d\n",filename,retval);
      printf("curr_move = %d\n",curr_game.curr_move);

      continue;
    }

    if ((curr_move < 0) || (curr_move > curr_game.num_moves)) {
      printf("curr_move is out of bounds on line %d; skipping\n",line_no);

      continue;
    }

    curr_game.curr_move = curr_move;

    retval = write_binary_game(filename,&curr_game);

    if (retval) {
      printf("write_binary_game of %s failed: %d\n",filename,retval);
      continue;
    }
  }

  fclose(fptr);

  return 0;
}
