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
"usage: find_mates (-binary_format) (-mine) (-not_mine) (-white) (-black) (-mating_squaresquare)\n"
"   (-mated_squaresquare) (-mate_distancedistance) filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

int get_mate_distance(int mating_square,int mated_square);

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bBinaryFormat;
  bool bMine;
  bool bNotMine;
  bool bWhite;
  bool bBlack;
  int file;
  int rank;
  int mating_square;
  int mated_square;
  int curr_mated_square;
  int mate_distance;
  int curr_mate_distance;
  int retval;
  FILE *fptr;
  int filename_len;
  struct game curr_game;
  unsigned char board[CHARS_IN_BOARD];
  int dbg;

  if ((argc < 2) || (argc > 10)) {
    printf(usage);
    return 1;
  }

  bBinaryFormat = false;
  bMine = false;
  bNotMine = false;
  bWhite = false;
  bBlack = false;
  mating_square = -1;
  mated_square = -1;
  mate_distance = -1;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-binary_format"))
      bBinaryFormat = true;
    else if (!strcmp(argv[curr_arg],"-mine"))
      bMine = true;
    else if (!strcmp(argv[curr_arg],"-not_mine"))
      bNotMine = true;
    else if (!strcmp(argv[curr_arg],"-white"))
      bWhite = true;
    else if (!strcmp(argv[curr_arg],"-black"))
      bBlack = true;
    else if ((strlen(argv[curr_arg]) == 16) &&
      !strncmp(argv[curr_arg],"-mating_square",14) &&
      (argv[curr_arg][14] >= 'a') && (argv[curr_arg][14] <= 'h') &&
      (argv[curr_arg][15] >= '1') && (argv[curr_arg][15] <= '8')) {

      file = argv[curr_arg][14] - 'a';
      rank = argv[curr_arg][15] - '1';
      mating_square = POS_OF(rank,file);
    }
    else if ((strlen(argv[curr_arg]) == 15) &&
      !strncmp(argv[curr_arg],"-mated_square",13) &&
      (argv[curr_arg][13] >= 'a') && (argv[curr_arg][13] <= 'h') &&
      (argv[curr_arg][14] >= '1') && (argv[curr_arg][14] <= '8')) {

      file = argv[curr_arg][13] - 'a';
      rank = argv[curr_arg][14] - '1';
      mated_square = POS_OF(rank,file);
    }
    else if (!strncmp(argv[curr_arg],"-mate_distance",14)) {
      sscanf(&argv[curr_arg][14],"%d",&mate_distance);
      mate_distance--;
    }
    else
      break;
  }

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  if (bMine && bNotMine) {
    printf("can't specify both -mine and -not_mine\n");
    return 3;
  }

  if (bWhite && bBlack) {
    printf("can't specify both -white and -black\n");
    return 4;
  }

  if ((mating_square != -1) && (mated_square != -1)) {
    printf("can't specify both -mating_square and -mated_square\n");
    return 5;
  }

  if ((mating_square != -1) && (mate_distance != -1)) {
    printf("can't specify both -mating_square and -mate_distance\n");
    return 6;
  }

  if ((mated_square != -1) && (mate_distance != -1)) {
    printf("can't specify both -mated_square and -mate_distance\n");
    return 7;
  }

  if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    return 8;
  }

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
      if (mating_square != -1 ) {
        if (curr_game.moves[curr_game.num_moves-1].to != mating_square)
          continue;
      }

      if (mated_square != -1 ) {
        if (curr_game.num_moves % 2)
          curr_mated_square = curr_game.black_pieces[12].current_board_position;
        else
          curr_mated_square = curr_game.white_pieces[4].current_board_position;

        if (curr_mated_square != mated_square)
          continue;
      }

      if (mate_distance != -1) {
        mating_square = curr_game.moves[curr_game.num_moves-1].to;

        if (curr_game.num_moves % 2)
          mated_square = curr_game.black_pieces[12].current_board_position;
        else
          mated_square = curr_game.white_pieces[4].current_board_position;

        curr_mate_distance = get_mate_distance(mating_square,mated_square);

        if (curr_mate_distance != mate_distance)
          continue;

        dbg = 1;
      }

      if (bWhite) {
        if (!(curr_game.num_moves % 2))
          continue;
      }
      else if (bBlack) {
        if (curr_game.num_moves % 2)
          continue;
      }

      if (!bMine && !bNotMine)
        printf("%s\n",filename);
      else if (bMine) {
        if (curr_game.num_moves % 2) {
          if (!curr_game.orientation)
            printf("%s\n",filename);
        }
        else {
          if (curr_game.orientation)
            printf("%s\n",filename);
        }
      }
      else {
        if (curr_game.num_moves % 2) {
          if (curr_game.orientation)
            printf("%s\n",filename);
        }
        else {
          if (!curr_game.orientation)
            printf("%s\n",filename);
        }
      }
    }
  }

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
