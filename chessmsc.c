#include <stdio.h>

#include "chess.h"
#include "chess.glb"
#include "chess.fun"
#include "chess.mac"

int format_square(int square)
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

void print_bd0(unsigned char *board,int orientation)
{
  int m;
  int n;
  int square;

  for (m = 0; m < 8; m++) {
    for (n = 0; n < 8; n++) {
      if (!orientation)
        square = get_piece2(board,7 - m,n);
      else
        square = get_piece2(board,m,7 - n);

      printf("%c",format_square(square));

      if (n < 7)
        putchar(' ');
    }

    putchar(0x0a);
  }
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

      printf("%c",format_square(square));

      if (n < 7)
        putchar(' ');
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

    update_board(gamept->board,&gamept->moves[gamept->curr_move],gamept->curr_move & 0x1);
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
      update_board(gamept->board,&gamept->moves[gamept->curr_move],gamept->curr_move & 0x1);
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

void print_special_moves(struct game *gamept)
{
  int n;
  int and_val;
  int hit;

  and_val = 0x1;
  hit = 0;

  for (n = 0; n < num_special_moves; n++) {
    if (gamept->moves[gamept->curr_move].special_move_info & and_val) {
      if (hit)
        putchar(' ' );

      printf("%s",special_moves[n]);

      hit++;
    }

    and_val <<= 1;
  }

  if (hit)
    putchar(0x0a);
}

int match_board(unsigned char *board1,unsigned char *board2,bool bExactMatch)
{
  int m;
  int n;
  int square1;
  int square2;

  for (m = 0; m < 8; m++) {
    for (n = 0; n < 8; n++) {
      square1 = get_piece2(board1,7 - m,n);
      square2 = get_piece2(board2,7 - m,n);

      if (!bExactMatch) {
        if (square2) {
          if (square2 == EMPTY_ID) {
            if (square1)
              break;
          }
          else if (square1 != square2)
            break;
        }
      }
      else {
        if (square2 == EMPTY_ID) {
          if (square1)
            break;
        }
        else if (square1 != square2)
          break;
      }
    }

    if (n < 8)
      break;
  }

  if (m < 8)
    return 0;

  return 1;
}

bool multiple_queens(unsigned char *board)
{
  int n;
  int piece;
  int num_white_queens = 0;
  int num_black_queens = 0;

  for (n = 0; n < NUM_BOARD_SQUARES; n++) {
    piece = get_piece1(board,n);

    if (piece == QUEEN_ID)
      num_white_queens++;
    else if (piece == QUEEN_ID * -1)
      num_black_queens++;
  }

  if ((num_white_queens > 1) || (num_black_queens > 1))
    return true;

  return false;
}

int get_enemy_king_file_and_rank(struct game *gamept,int *file_pt,int *rank_pt)
{
  int m;
  int n;
  int enemy_king_id;
  int square;

  if (!gamept->orientation)
    enemy_king_id = KING_ID * -1;
  else
    enemy_king_id = KING_ID;

  for (m = 0; m < 8; m++) {
    for (n = 0; n < 8; n++) {
      square = get_piece2(gamept->board,m,n);

      if (square == enemy_king_id)
        break;
    }

    if (n < 8)
      break;
  }

  if (m < 8) {
    *file_pt = n;
    *rank_pt = m;
    return 1;
  }

  return 0;
}

void copy_board(unsigned char *from_board,unsigned char *to_board)
{
  int n;

  for (n = 0; n < CHARS_IN_BOARD; n++)
    to_board[n] = from_board[n];
}
