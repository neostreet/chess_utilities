#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "chess.h"
#define MAKE_GLOBALS_HERE
#include "chess.glb"
#include "chess.fun"
#include "chess.mac"

#define MAX_FILENAME_LEN 256
static char filename[MAX_FILENAME_LEN];

int mated_square_counts[NUM_BOARD_SQUARES];

static char usage[] =
"usage: mated_square_histogram (-verbose) (-binary_format) (-sort) filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

int elem_compare(const void *elem1,const void *elem2);

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bVerbose;
  bool bBinaryFormat;
  bool bSort;
  int retval;
  FILE *fptr;
  int filename_len;
  struct game curr_game;
  int mated_square;
  int total_counts;
  char algebraic[3];
  int ixs[NUM_BOARD_SQUARES];

  if ((argc < 2) || (argc > 5)) {
    printf(usage);
    return 1;
  }

  bVerbose = false;
  bBinaryFormat = false;
  bSort = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else if (!strcmp(argv[curr_arg],"-binary_format"))
      bBinaryFormat = true;
    else if (!strcmp(argv[curr_arg],"-sort"))
      bSort = true;
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

  for (n = 0; n < NUM_BOARD_SQUARES; n++)
    mated_square_counts[n] = 0;

  algebraic[2] = 0;

  for ( ; ; ) {
    GetLine(fptr,filename,&filename_len,MAX_FILENAME_LEN);

    if (feof(fptr))
      break;

    bzero(&curr_game,sizeof (struct game));

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

    if (curr_game.moves[curr_game.num_moves-1].special_move_info & SPECIAL_MOVE_MATE) {
      if (curr_game.num_moves % 2)
        mated_square = curr_game.black_pieces[12].current_board_position;
      else
        mated_square = curr_game.white_pieces[4].current_board_position;

      mated_square_counts[mated_square]++;
    }
  }

  fclose(fptr);

  total_counts = 0;

  for (n = 0; n < NUM_BOARD_SQUARES; n++)
    ixs[n] = n;

  if (bSort)
    qsort(ixs,NUM_BOARD_SQUARES,sizeof (int),elem_compare);

  for (n = 0; n < NUM_BOARD_SQUARES; n++) {
    if (bVerbose || mated_square_counts[ixs[n]]) {
      algebraic[0] = 'a' + FILE_OF(ixs[n]);
      algebraic[1] = '1' + RANK_OF(ixs[n]);
      printf("%5d %s\n",mated_square_counts[ixs[n]],algebraic);
      total_counts += mated_square_counts[ixs[n]];
    }
  }

  printf("\n%5d total\n",total_counts);

  return 0;
}

int elem_compare(const void *elem1,const void *elem2)
{
  int ix1;
  int ix2;

  ix1 = *(int *)elem1;
  ix2 = *(int *)elem2;

  return mated_square_counts[ix2] - mated_square_counts[ix1];
}
