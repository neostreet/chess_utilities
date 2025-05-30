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
"usage: count_queen_moves (-binary_format) (-mine) (-opponent) filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bBinaryFormat;
  bool bMine;
  bool bOpponent;
  int retval;
  FILE *fptr;
  int filename_len;
  struct game curr_game;

  if ((argc < 2) || (argc > 5)) {
    printf(usage);
    return 1;
  }

  bBinaryFormat = false;
  bMine = false;
  bOpponent = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-binary_format"))
      bBinaryFormat = true;
    else if (!strcmp(argv[curr_arg],"-mine"))
      bMine = true;
    else if (!strcmp(argv[curr_arg],"-opponent"))
      bOpponent = true;
    else
      break;
  }

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  if (bMine && bOpponent) {
    printf("can't specify both -mine and -opponent\n");
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

    bzero(&curr_game,sizeof (struct game));

    if (!bBinaryFormat) {
      retval = read_game(filename,&curr_game);

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

    if (bMine) {
      if (!curr_game.orientation) {
        printf("%d %s\n",curr_game.white_pieces[3].move_count,filename);
      }
      else {
        printf("%d %s\n",curr_game.black_pieces[11].move_count,filename);
      }
    }
    else if (bOpponent) {
      if (!curr_game.orientation) {
        printf("%d %s\n",curr_game.black_pieces[11].move_count,filename);
      }
      else {
        printf("%d %s\n",curr_game.white_pieces[3].move_count,filename);
      }
    }
    else {
      printf("white queen: %d, black queen: %d %s\n",
        curr_game.white_pieces[3].move_count,
        curr_game.black_pieces[11].move_count,
        filename);
    }
  }

  fclose(fptr);

  return 0;
}
