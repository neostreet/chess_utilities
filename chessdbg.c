#include <vector>

using namespace std;

#include <stdio.h>
#include <string.h>

#include "chess.h"
#include "chess.fun"
#include "chess.glb"
#include "chess.mac"

extern char piece_ids[]; /* "RNBQK" */

static char buf[20];
char fmt_str[] = "%s\n";

bool kingside_castle(struct game *gamept)
{
  if (!((gamept->curr_move-1) % 2)) {
    // white

    if ((gamept->moves[gamept->curr_move-1].from == 4) &&
      (gamept->moves[gamept->curr_move-1].to == 6))
      return true;
  }
  else {
    // black

    if ((gamept->moves[gamept->curr_move-1].from == 60) &&
      (gamept->moves[gamept->curr_move-1].to == 62))
      return true;
  }

  return false;
}

bool queenside_castle(struct game *gamept)
{
  if (!((gamept->curr_move-1) % 2)) {
    // white

    if ((gamept->moves[gamept->curr_move-1].from == 4) &&
      (gamept->moves[gamept->curr_move-1].to == 2))
      return true;
  }
  else {
    // black

    if ((gamept->moves[gamept->curr_move-1].from == 60) &&
      (gamept->moves[gamept->curr_move-1].to == 58))
      return true;
  }

  return false;
}

char decode_piece(int piece,bool bShowBlack)
{
  bool bBlack;

  if (piece < 0) {
    bBlack = true;
    piece *= -1;
  }
  else
    bBlack = false;

  if (!piece)
    return '.';

  if (piece == 1) {
    if (!bShowBlack || bBlack)
      return 'P';
    else
      return 'p';
  }

  piece -= 2;

  if (!bShowBlack || bBlack)
    return piece_ids[piece];
  else
    return piece_ids[piece] + 'a' - 'A';
}

char get_decoded_piece(struct game *gamept)
{
  int piece;

  piece = get_piece1(gamept->board,
    gamept->moves[gamept->curr_move-1].to);
  return decode_piece(piece,false);
}

void print_game(struct game *gamept)
{
  printf(fmt_str,gamept->title);

  set_initial_board(gamept);

  for (gamept->curr_move = 0;
       gamept->curr_move <= gamept->num_moves;
       gamept->curr_move++) {

    if (gamept->curr_move) {
      sprintf_move(gamept,buf,20,true);
      printf("%s",buf);

      if (!(gamept->curr_move % 2))
        putchar(0x0a);
    }

    if (gamept->curr_move < gamept->num_moves)
      update_board(gamept,false);
  }
}

void fprintf_move(FILE *fptr,struct game *gamept)
{
  if (gamept->curr_move == gamept->num_moves)
    return;

  fprintf(fptr,"%d %d %d\n",
    gamept->moves[gamept->curr_move].from,
    gamept->moves[gamept->curr_move].to,
    gamept->moves[gamept->curr_move].special_move_info);
}

void sprintf_move(struct game *gamept,char *buf,int buf_len,bool bInline)
{
  bool bWhite;
  int put_count;
  char decoded_piece;
  bool bDone;
  int from;
  int from_file;
  int to;
  int to_file;
  int to_rank;

  if (!gamept->curr_move) {
    strcpy(buf,"Starting position");
    put_count = strlen(buf);

    for ( ; put_count < buf_len - 1; put_count++)
      buf[put_count] = ' ';

    buf[put_count] = 0;

    return;
  }

  bWhite = !((gamept->curr_move-1) % 2);

  if (bWhite || !bInline) {
    sprintf(buf,"%2d. ",((gamept->curr_move-1) / 2) + 1);
    put_count = 4;
  }
  else
    put_count = 0;

  if (!bInline && !bWhite) {
    sprintf(&buf[put_count],"...  ");
    put_count += 5;
  }

  if (kingside_castle(gamept)) {
    sprintf(&buf[put_count],"O-O");
    put_count += 3;
  }
  else if (queenside_castle(gamept)) {
    sprintf(&buf[put_count],"O-O-O");
    put_count += 5;
  }
  else {
    decoded_piece = get_decoded_piece(gamept);

    if (decoded_piece == 'P')
      bDone = false;
    else {
      buf[put_count++] = decoded_piece;
      bDone = false;
    }

    if (!bDone) {
      to = gamept->moves[gamept->curr_move-1].to;
      to_file = FILE_OF(to);
      to_rank = RANK_OF(to);
      buf[put_count++] = 'a' + to_file;
      buf[put_count++] = '1' + to_rank;
    }
  }

  if (!bInline) {
    for ( ; put_count < buf_len - 1; put_count++)
      buf[put_count] = ' ';
  }
  else if (bWhite) {
    for ( ; put_count < 12; put_count++)
      buf[put_count] = ' ';
  }

  buf[put_count] = 0;
}

void print_from_to(struct game *gamept)
{
  int n;
  printf(fmt_str,gamept->title);

  set_initial_board(gamept);

  for (n = 0; n < gamept->num_moves; n++)
    printf("%d %d\n",gamept->moves[n].from,gamept->moves[n].to);
}
