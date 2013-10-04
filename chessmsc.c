#include <vector>

using namespace std;

#include <stdio.h>

#include "chess.h"
#include "chess.glb"
#include "chess.fun"
#include "chess.mac"

extern char piece_ids[]; /* "RNBQK" */
extern char fmt_str[];

static int format_square(int square)
{
  bool bBlack;
  int return_char;

  if (!square)
    return (int)'.';

  if (square < 0) {
    bBlack = true;
    square *= -1;
  }
  else
    bBlack = false;

  if (square == 1)
    return_char = 'P';
  else
    return_char = piece_ids[square - 2];

  if (!bBlack)
    return_char += ('a' - 'A');

  return return_char;
}

void print_bd(struct game *gamept)
{
  int m;
  int n;
  int square;

  for (m = 0; m < 8; m++) {
    for (n = 0; n < 8; n++) {
      if (!gamept->orientation)
        square = get_piece2(gamept->board,7 - m,n);
      else
        square = get_piece2(gamept->board,m,7 - n);

      printf("%c ",format_square(square));
    }

    putchar(0x0a);
  }
}

void fprint_game_bin(struct game *gamept,char *filename)
{
  FILE *fptr;

  if ((fptr = fopen(filename,"w")) == NULL)
    return;

  fprintf(fptr,fmt_str,gamept->title);

  set_initial_board(gamept);

  for (gamept->curr_move = 0;
       gamept->curr_move <= gamept->num_moves;
       gamept->curr_move++) {

    fprintf_move(fptr,gamept);

    update_board(gamept,false);
  }

  fclose(fptr);
}

void fprint_game(struct game *gamept,char *filename)
{
  FILE *fptr;
  char buf[20];

  if ((fptr = fopen(filename,"w")) == NULL)
    return;

  fprintf(fptr,fmt_str,gamept->title);

  set_initial_board(gamept);

  for (gamept->curr_move = 0;
       gamept->curr_move <= gamept->num_moves;
       gamept->curr_move++) {

    sprintf_move(gamept,buf,20,true);
    fprintf(fptr,"%s",buf);

    if (gamept->curr_move < gamept->num_moves)
      update_board(gamept,false);
  }

  fclose(fptr);
}

void fprint_bd(struct game *gamept,char *filename)
{
  int m;
  int n;
  FILE *fptr;
  int square;

  if ((fptr = fopen(filename,"w")) == NULL)
    return;

  for (m = 0; m < 8; m++) {
    for (n = 0; n < 8; n++) {
      square = get_piece2(gamept->board,7 - m,n);
      fprintf(fptr,"%c ",format_square(square));
    }

    fputc(0x0a,fptr);
  }

  fclose(fptr);
}

void print_pieces(struct game *gamept)
{
  int m;
  int n;
  int piece;

  for (m = 0; m < NUM_PLAYERS; m++) {
    if (!m)
      printf("White pieces: ");
    else
      printf("Black pieces: ");

    for (n = 0; n < gamept->num_pieces[m]; n++) {
      piece = get_piece1(gamept->board,(int)gamept->piece_offsets[m][n]);
      printf("%c",format_square(piece));

      if (n < gamept->num_pieces[m] - 1)
        putchar(' ');
      else
        putchar(0x0a);
    }
  }
}
