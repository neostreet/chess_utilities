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

static char usage[] = "usage: fprint_forced_moves (-verbose) filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bVerbose;
  FILE *fptr;
  FILE *out_fptr;
  int filename_len;
  int retval;
  struct game curr_game;
  bool bWhite;
  int found_white;
  int found_black;

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

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    return 3;
  }

  found_white = 0;
  found_black = 0;

  for ( ; ; ) {

  GetLine(fptr,filename,&filename_len,MAX_FILENAME_LEN);

  if (feof(fptr))
    break;

  retval = read_game(filename,&curr_game);

  if (retval) {
    printf("read_game of %s failed: %d\n",filename,retval);
    printf("curr_move = %d\n",curr_game.curr_move);
    continue;
  }

  if (bVerbose) {
    sprintf(out_filename,"%s.forced_moves",filename);

    if ((out_fptr = fopen(out_filename,"w")) == NULL) {
      printf(couldnt_open,out_filename);
      return 4;
    }
  }

  set_initial_board(&curr_game);

  for (curr_game.curr_move = 0; curr_game.curr_move < curr_game.num_moves; curr_game.curr_move++) {
    legal_moves_count =  0;

    get_legal_moves(&curr_game,legal_moves,&legal_moves_count);

    if (legal_moves_count == 1) {
      if (!(curr_game.curr_move % 2)) {
        bWhite = true;
        found_white++;
      }
      else {
        bWhite = false;
        found_black++;
      }

      if (bVerbose) {
        fprintf(out_fptr,"move %d\n\n",curr_game.curr_move);
        fprint_bd3(curr_game.board,curr_game.orientation,out_fptr);
        fprintf(out_fptr,"\n");

        if (bWhite) {
          fprintf(out_fptr,"White to move\n");
          fprint_piece_info2(out_fptr,curr_game.white_pieces,true,true,true);
        }
        else {
          fprintf(out_fptr,"Black to move\n");
          fprint_piece_info2(out_fptr,curr_game.black_pieces,false,true,true);
        }

        fprintf(out_fptr,"\n");
        fprint_moves3(out_fptr,legal_moves,legal_moves_count,false,false);
        fprintf(out_fptr,"\n");
      }
    }

    update_board(&curr_game,NULL,NULL,false);
  }

  if (bVerbose) {
    fprintf(out_fptr,"move %d\n\n",curr_game.curr_move);
    fprint_bd3(curr_game.board,curr_game.orientation,out_fptr);

    fclose(out_fptr);
  }

  printf("white: %d, black: %d, total: %d %s\n",
    found_white,found_black,found_white + found_black,filename);

  }

  return 0;
}
