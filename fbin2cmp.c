#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FILENAME_LEN 1024

static char filename[MAX_FILENAME_LEN];
static char cmp_filename[MAX_FILENAME_LEN];

#define MAX_LINE_LEN 8192
static char line[MAX_LINE_LEN];

#define MAX_CANDIDATES 24

#include "chess.h"
#define MAKE_GLOBALS_HERE
#include "chess.glb"
#include "chess.fun"
#include "chess.mac"

static char usage[] =
"usage: fbin2cmp (-debug) seed filename\n";

static struct game curr_game;
static struct board_comparison curr_comparison;
static unsigned char altered_board[CHARS_IN_BOARD];

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

static int build_cmp_filename(
  char *bin_filename,
  int bin_filename_len,
  char *cmp_filename,
  int max_filename_len);

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bDebug;
  int seed;
  int work;
  int modulo1;
  int modulo2;
  FILE *fptr0;
  int file_len;
  int bin_filename_len;
  int retval;
  int candidates[MAX_CANDIDATES];
  int num_candidates;

  if ((argc < 3) || (argc > 4)) {
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

  if (argc - curr_arg != 2) {
    printf(usage);
    return 2;
  }

  sscanf(argv[1],"%d",&seed);
  srand(seed);

  if ((fptr0 = fopen(argv[curr_arg+1],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg+1]);
    return 3;
  }

  for ( ; ; ) {
    GetLine(fptr0,filename,&file_len,MAX_FILENAME_LEN);

    if (feof(fptr0))
      break;

    work = rand();
    modulo1 = work % 2;

    bin_filename_len = strlen(filename);

    retval = build_cmp_filename(filename,bin_filename_len,cmp_filename,MAX_FILENAME_LEN);

    if (retval) {
      printf("build_cmp_filename failed on %s: %d\n",filename,retval);
      continue;
    }

    bzero(&curr_game,sizeof (struct game));

    retval = read_binary_game(filename,&curr_game);

    if (retval) {
      printf("read_binary_game of %s failed: %d\n",filename,retval);
      printf("curr_move = %d\n",curr_game.curr_move);
      continue;
    }

    num_candidates = 0;

    for (n = 0; n < NUM_PIECES_PER_PLAYER; n++) {
      if (curr_game.white_pieces[n].current_board_position != -1) {
        if ((curr_game.white_pieces[n].piece_id == PAWN_ID) ||
          (curr_game.white_pieces[n].piece_id == KNIGHT_ID) ||
          (curr_game.white_pieces[n].piece_id == BISHOP_ID)) {

          candidates[num_candidates++] = curr_game.white_pieces[n].current_board_position;
        }
      }

      if (curr_game.black_pieces[n].current_board_position != -1) {
        if ((curr_game.black_pieces[n].piece_id == PAWN_ID * -1) ||
          (curr_game.black_pieces[n].piece_id == KNIGHT_ID * -1) ||
          (curr_game.black_pieces[n].piece_id == BISHOP_ID * -1)) {

          candidates[num_candidates++] = curr_game.black_pieces[n].current_board_position;
        }
      }
    }

    modulo2 = work % num_candidates;

    copy_board(curr_game.board,altered_board);
    set_piece1(altered_board,candidates[modulo2],0);

    curr_comparison.orientation = curr_game.orientation;

    if (!modulo1) {
      copy_board(curr_game.board,curr_comparison.board[0]);
      copy_board(altered_board,curr_comparison.board[1]);
    }
    else {
      copy_board(curr_game.board,curr_comparison.board[1]);
      copy_board(altered_board,curr_comparison.board[0]);
    }

    retval = write_board_comparison(cmp_filename,&curr_comparison);

    if (retval) {
      printf("write_board_comparison of %s failed: %d\n",filename,retval);
      continue;
    }
  }

  return 0;
}

static int build_cmp_filename(
  char *bin_filename,
  int bin_filename_len,
  char *cmp_filename,
  int max_filename_len)
{
  int n;

  for (n = 0; n < bin_filename_len; n++) {
    if (bin_filename[n] == '.')
      break;
  }

  if (n == bin_filename_len)
    return 1;

  if (n + 4 > max_filename_len - 1)
    return 2;

  strcpy(cmp_filename,bin_filename);
  strcpy(&cmp_filename[n+1],"cmp");

  return 0;
}
