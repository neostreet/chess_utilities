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

static char usage[] =
"usage: fprint_forced_moves (-verbose) (-both) (-total_first) (-debug)\n"
"  (-only_wins) (-only_draws) (-only_losses) (-i_am_white) (-i_am_black) filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bVerbose;
  bool bBoth;
  bool bTotalFirst;
  bool bDebug;
  bool bOnlyWins;
  bool bOnlyDraws;
  bool bOnlyLosses;
  bool bIAmWhite;
  bool bIAmBlack;
  FILE *fptr;
  FILE *out_fptr;
  int filename_len;
  int retval;
  struct game curr_game;
  bool bWhite;
  int found_white;
  int found_black;

  if ((argc < 2) || (argc > 11)) {
    printf(usage);
    return 1;
  }

  bVerbose = false;
  bBoth = false;
  bTotalFirst = false;
  bDebug = false;
  bOnlyWins = false;
  bOnlyDraws = false;
  bOnlyLosses = false;
  bIAmWhite = false;
  bIAmBlack = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else if (!strcmp(argv[curr_arg],"-both"))
      bBoth = true;
    else if (!strcmp(argv[curr_arg],"-total_first"))
      bTotalFirst = true;
    else if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = true;
    else if (!strcmp(argv[curr_arg],"-only_wins"))
      bOnlyWins = true;
    else if (!strcmp(argv[curr_arg],"-only_draws"))
      bOnlyDraws = true;
    else if (!strcmp(argv[curr_arg],"-only_losses"))
      bOnlyLosses = true;
    else if (!strcmp(argv[curr_arg],"-i_am_white"))
      bIAmWhite = true;
    else if (!strcmp(argv[curr_arg],"-i_am_black"))
      bIAmBlack = true;
    else
      break;
  }

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  if (bOnlyWins && bOnlyDraws) {
    printf("can't specify both -only_wins and -only_draws\n");
    return 3;
  }

  if (bOnlyWins && bOnlyLosses) {
    printf("can't specify both -only_wins and -only_losses\n");
    return 4;
  }

  if (bOnlyDraws && bOnlyLosses) {
    printf("can't specify both -only_draws and -only_losses\n");
    return 5;
  }

  if (bIAmWhite and bIAmBlack) {
    printf("can't specify both -i_am_white and -i_am_black\n");
    return 6;
  }

  if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    return 7;
  }

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

  if (bIAmWhite && curr_game.orientation)
    continue;

  if (bIAmBlack && !curr_game.orientation)
    continue;

  if (bOnlyWins || bOnlyDraws || bOnlyLosses) {
    if (bOnlyWins) {
      if (curr_game.result != RESULT_WIN)
        continue;
    }
    else if (bOnlyDraws) {
      if (curr_game.result != RESULT_DRAW)
        continue;
    }
    else if (bOnlyLosses) {
      if (curr_game.result != RESULT_LOSS)
        continue;
    }
  }

  if (bVerbose) {
    sprintf(out_filename,"%s.%s%s%s%s%s%s%sforced_moves",
      filename,
      (bBoth ? "both." : ""),
      (bTotalFirst ? "total_first." : ""),
      (bOnlyWins ? "only_wins." : ""),
      (bOnlyDraws ? "only_draws." : ""),
      (bOnlyLosses ? "only_losses." : ""),
      (bIAmWhite ? "i_am_white." : ""),
      (bIAmBlack ? "i_am_black." : ""));

    if ((out_fptr = fopen(out_filename,"w")) == NULL) {
      printf(couldnt_open,out_filename);
      return 8;
    }
  }

  found_white = 0;
  found_black = 0;

  set_initial_board(&curr_game);

  for (curr_game.curr_move = 0; curr_game.curr_move < curr_game.num_moves; curr_game.curr_move++) {
    legal_moves_count =  0;

    get_legal_moves(&curr_game,legal_moves,&legal_moves_count);

    if (bDebug)
      printf("move %d: %d legal moves\n",curr_game.curr_move,legal_moves_count);

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

  if (!bBoth || ((found_white > 0) && (found_black > 0))) {
    if (!bTotalFirst) {
      printf("white: %d, black: %d, total: %d %s\n",
        found_white,found_black,found_white + found_black,filename);
    }
    else {
      printf("%d total, white: %d, black: %d, %s\n",
        found_white + found_black,found_white,found_black,filename);
    }
  }

  }

  return 0;
}
