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

#define MAX_MATE_DISTANCE 14

int mate_distance_counts[MAX_MATE_DISTANCE];

static char usage[] =
"usage: mate_distance_histogram (-debug) (-binary_format) (-sort) filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

int get_mate_distance(int mating_square,int mated_square);
int elem_compare(const void *elem1,const void *elem2);

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bDebug;
  bool bBinaryFormat;
  bool bSort;
  int retval;
  FILE *fptr;
  int filename_len;
  struct game curr_game;
  int mating_square;
  int mated_square;
  int mate_distance;
  int total_counts;
  int ixs[MAX_MATE_DISTANCE];

  if ((argc < 2) || (argc > 5)) {
    printf(usage);
    return 1;
  }

  bDebug = false;
  bBinaryFormat = false;
  bSort = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = true;
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

  for (n = 0; n < MAX_MATE_DISTANCE; n++)
    mate_distance_counts[n] = 0;

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
      mating_square = curr_game.moves[curr_game.num_moves-1].to;

      if (curr_game.num_moves % 2)
        mated_square = curr_game.black_pieces[12].current_board_position;
      else
        mated_square = curr_game.white_pieces[4].current_board_position;

      mate_distance = get_mate_distance(mating_square,mated_square);

      if (mate_distance < 0) {
        printf("%s: mate_distance %d < 0\n",filename,mate_distance);
        continue;
      }

      if (mate_distance >= MAX_MATE_DISTANCE) {
        printf("%s: mate_distance %d >= %d\n",filename,mate_distance,MAX_MATE_DISTANCE);
        continue;
      }

      if (bDebug)
        printf("%s: mate_distance = %d\n",filename,mate_distance);

      mate_distance_counts[mate_distance]++;
    }
  }

  fclose(fptr);

  total_counts = 0;

  for (n = 0; n < MAX_MATE_DISTANCE; n++)
    ixs[n] = n;

  if (bSort)
    qsort(ixs,MAX_MATE_DISTANCE,sizeof (int),elem_compare);

  for (n = 0; n < MAX_MATE_DISTANCE; n++) {
    if (mate_distance_counts[ixs[n]]) {
      printf("%5d %d\n",mate_distance_counts[ixs[n]],ixs[n] + 1);
      total_counts += mate_distance_counts[ixs[n]];
    }
  }

  printf("\n%5d total\n",total_counts);

  return 0;
}

int get_mate_distance(int mating_square,int mated_square)
{
  int mating_file;
  int mating_rank;
  int mated_file;
  int mated_rank;
  int file_distance;
  int rank_distance;

  mating_file = FILE_OF(mating_square);
  mating_rank = RANK_OF(mating_square);
  mated_file = FILE_OF(mated_square);
  mated_rank = RANK_OF(mated_square);

  if (mating_file > mated_file)
    file_distance = mating_file - mated_file;
  else
    file_distance = mated_file - mating_file;

  if (mating_rank > mated_rank)
    rank_distance = mating_rank - mated_rank;
  else
    rank_distance = mated_rank - mating_rank;

  return file_distance + rank_distance - 1;
}

int elem_compare(const void *elem1,const void *elem2)
{
  int ix1;
  int ix2;

  ix1 = *(int *)elem1;
  ix2 = *(int *)elem2;

  return mate_distance_counts[ix2] - mate_distance_counts[ix1];
}
