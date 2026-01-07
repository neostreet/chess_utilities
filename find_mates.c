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
"usage: find_mates (-debug) (-verbose) (-mine) (-not_mine) (-mating_squaresquare)\n"
"  (-mated_squaresquare) (-mate_distancedistance) (-mating_piecepiece) (-discovered) (-back_rank)\n"
"  (-mated_king_not_on_back_rank) [white | black] filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

int get_mate_distance(int mating_square,int mated_square);

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bDebug;
  bool bVerbose;
  bool bMine;
  bool bNotMine;
  int file;
  int rank;
  int mating_square;
  int curr_mating_square;
  int mated_square;
  int curr_mated_square;
  int mate_distance;
  int curr_mate_distance;
  int mating_piece;
  bool bDiscovered;
  bool bBackRank;
  bool bMatedKingNotOnBackRank;
  int by_white;
  int retval;
  FILE *fptr;
  int filename_len;
  struct game curr_game;
  int read_count;
  int match_count;
  int last_piece;

  if ((argc < 3) || (argc > 14)) {
    printf(usage);
    return 1;
  }

  bDebug = false;
  bVerbose = false;
  bMine = false;
  bNotMine = false;
  mating_square = -1;
  mated_square = -1;
  mate_distance = -1;
  mating_piece = 0;
  bDiscovered = false;
  bBackRank = false;
  bMatedKingNotOnBackRank = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = true;
    else if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else if (!strcmp(argv[curr_arg],"-mine"))
      bMine = true;
    else if (!strcmp(argv[curr_arg],"-not_mine"))
      bNotMine = true;
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
    else if (!strncmp(argv[curr_arg],"-mating_piece",13))
      sscanf(&argv[curr_arg][13],"%d",&mating_piece);
    else if (!strcmp(argv[curr_arg],"-discovered"))
      bDiscovered = true;
    else if (!strcmp(argv[curr_arg],"-back_rank"))
      bBackRank = true;
    else if (!strcmp(argv[curr_arg],"-mated_king_not_on_back_rank"))
      bMatedKingNotOnBackRank = true;
    else
      break;
  }

  if (argc - curr_arg != 2) {
    printf(usage);
    return 2;
  }

  by_white = -1;

  if (!strcmp(argv[curr_arg],"white"))
    by_white = 1;
  else if (!strcmp(argv[curr_arg],"black"))
    by_white = 0;

  if (by_white == -1) {
    printf(usage);
    return 3;
  }

  if (bMine && bNotMine) {
    printf("can't specify both -mine and -not_mine\n");
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

  if ((fptr = fopen(argv[curr_arg+1],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg+1]);
    return 8;
  }

  read_count = 0;
  match_count = 0;

  for ( ; ; ) {
    GetLine(fptr,filename,&filename_len,MAX_FILENAME_LEN);

    if (feof(fptr))
      break;

    if (bDebug) {
      read_count++;

      if (!(read_count % 100))
        printf("about to read %s\n",filename);
    }

    bzero(&curr_game,sizeof (struct game));

    retval = read_game(filename,&curr_game);

    if (retval) {
      printf("read_game of %s failed: %d\n",filename,retval);
      printf("curr_move = %d\n",curr_game.curr_move);

      continue;
    }

    if (curr_game.moves[curr_game.num_moves-1].special_move_info & SPECIAL_MOVE_MATE) {
      if (by_white) {
        if (!(curr_game.num_moves % 2))
          continue;
      }
      else {
        if (curr_game.num_moves % 2)
          continue;
      }

      if (bMine) {
        if (curr_game.num_moves % 2) {
          if (curr_game.orientation)
            continue;
        }
        else {
          if (!curr_game.orientation)
            continue;
        }
      }
      else if (bNotMine) {
        if (curr_game.num_moves % 2) {
          if (!curr_game.orientation)
            continue;
        }
        else {
          if (curr_game.orientation)
            continue;
        }
      }

      if (bBackRank) {
        if (!back_rank_mate(&curr_game))
          continue;
      }

      if (bMatedKingNotOnBackRank) {
        if (!mated_king_not_on_back_rank(&curr_game))
          continue;
      }

      if (mating_piece) {
        last_piece = get_piece1(curr_game.board,curr_game.moves[curr_game.num_moves-1].to);

        if (last_piece != mating_piece)
          continue;
      }

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
        curr_mating_square = curr_game.moves[curr_game.num_moves-1].to;

        if (curr_game.num_moves % 2)
          curr_mated_square = curr_game.black_pieces[12].current_board_position;
        else
          curr_mated_square = curr_game.white_pieces[4].current_board_position;

        curr_mate_distance = get_mate_distance(curr_mating_square,curr_mated_square);

        if (curr_mate_distance != mate_distance)
          continue;
      }

      if (bDiscovered) {
        curr_mating_square = curr_game.moves[curr_game.num_moves-1].to;

        if (curr_game.num_moves % 2)
          curr_mated_square = curr_game.black_pieces[12].current_board_position;
        else
          curr_mated_square = curr_game.white_pieces[4].current_board_position;

        if (square_attacks_square(curr_game.board,curr_mating_square,curr_mated_square))
          continue;
      }

      printf("%s\n",filename);
      match_count++;

      if (bVerbose)
        print_bd(&curr_game);
    }
  }

  fclose(fptr);

  if (bDebug)
    printf("read_count = %d, match_count = %d\n",read_count,match_count);

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
