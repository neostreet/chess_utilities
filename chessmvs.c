#include <stdio.h>
#include <string.h>

#include "chess.h"
#include "chess.glb"
#include "chess.mac"
#include "chess.fun"

using namespace std;

int do_castle(struct game *gamept,int direction,char *word,int wordlen,struct move *move_ptr)
{
  int rank;

  if (direction == 1)  /* if white's move: */
    rank = 0;
  else
    rank = 7;

  /* make sure the king is on his original square: */
  if (get_piece2(gamept->board,rank,4) != 6 * direction) {
    do_castle_failures++;
    return 1;
  }

  if (wordlen == 3) {  /* kingside castle */
    /* make sure there is a rook in the corner: */
    if (get_piece2(gamept->board,rank,7) != 2 * direction) {
      do_castle_failures++;
      return 2;
    }

    /* make sure there are empty squares between king and rook: */
    if (get_piece2(gamept->board,rank,5) || get_piece2(gamept->board,rank,6)) {
      do_castle_failures++;
      return 3;
    }

    if (direction == 1) {  /* if white's move: */
      move_ptr->from = 4;
      move_ptr->to = 6;
    }
    else {
      move_ptr->from = 60;
      move_ptr->to = 62;
    }
  }
  else if (wordlen == 5) {  /* queenside castle */
    /* make sure there is a rook in the corner: */
    if (get_piece2(gamept->board,rank,0) != 2 * direction) {
      do_castle_failures++;
      return 4;
    }

    /* make sure there are empty squares between king and rook: */
    if (get_piece2(gamept->board,rank,1) || get_piece2(gamept->board,rank,2) || get_piece2(gamept->board,rank,3)) {
      do_castle_failures++;
      return 5;
    }

    if (direction == 1) {  /* if white's move: */
      move_ptr->from = 4;
      move_ptr->to = 2;
    }
    else {
      move_ptr->from = 60;
      move_ptr->to = 58;
    }
  }
  else {
    do_castle_failures++;
    return 6;
  }

  move_ptr->special_move_info = SPECIAL_MOVE_CASTLE;
  do_castle_successes++;

  return 0;  /* success */
}

int do_pawn_move(struct game *gamept,int direction,char *word,int wordlen,struct move *move_ptr)
{
  int n;
  int file;
  int to_rank;
  int rank;
  int capture_file;
  int piece;
  int which_piece;

  /*printf("%s\n",word);/*for now*/
  file = word[0] - 'a';

  if ((file < 0) || (file > 7))
    return 1;

  /* pawn advance or capture: */
  rank = word[1] - '1';

  if ((rank >= 0) && (rank <= 7)) {
    to_rank = rank;

    /* pawn advance */
    move_ptr->special_move_info = 0;
    move_ptr->to = POS_OF(rank,file);

    for (n = 0; n < 2; n++) {
      rank -= direction;

      if ((rank >= 1) && (rank <= 6)) {
        piece = get_piece2(gamept->board,rank,file);

        if (piece == direction)
          break;

        if (piece)
          return 2;
      }
    }

    if (n == 2)
      return 3;

    if (n == 1) {
      /* only allow two square move from initial position */
      if (direction == 1) {
        if (rank != 1)
          return 4;
      }
      else if (rank != 6)
        return 5;
    }

    move_ptr->from = POS_OF(rank,file);

    /* handle pawn promotion */
    if (direction == 1) {
      /* white's move */
      if (to_rank == 7) {
        if (wordlen != 3)
          return 6;

        switch (word[2]) {
          case 'Q':
            move_ptr->special_move_info =
              SPECIAL_MOVE_PROMOTION_QUEEN;

            break;
          case 'R':
            move_ptr->special_move_info =
              SPECIAL_MOVE_PROMOTION_ROOK;

            break;
          case 'N':
            move_ptr->special_move_info =
              SPECIAL_MOVE_PROMOTION_KNIGHT;

            break;
          case 'B':
            move_ptr->special_move_info =
              SPECIAL_MOVE_PROMOTION_BISHOP;

            break;
        }
      }
    }
    else {
      /* black's move */
      if (to_rank == 0) {
        if (wordlen != 3)
          return 8;

        switch (word[2]) {
          case 'Q':
            move_ptr->special_move_info =
              SPECIAL_MOVE_PROMOTION_QUEEN;

            break;
          case 'R':
            move_ptr->special_move_info =
              SPECIAL_MOVE_PROMOTION_ROOK;

            break;
          case 'N':
            move_ptr->special_move_info =
              SPECIAL_MOVE_PROMOTION_KNIGHT;

            break;
          case 'B':
            move_ptr->special_move_info =
              SPECIAL_MOVE_PROMOTION_BISHOP;

            break;
        }
      }
    }

    return 0;
  }

  /* pawn capture */
  capture_file = word[1] - 'a';

  if ((capture_file < 0) || (capture_file > 7))
    return 10;

  if (capture_file > file) {
    if (capture_file - file != 1)
      return 11;
  }
  else if (file - capture_file != 1)
    return 12;

  if ((wordlen >= 2) && (wordlen <= 4)) {
    if (direction == 1)
      rank = 1;
    else
      rank = 6;

    for (n = 0; n < 6; n++, rank += direction) {
      /*printf("%d %d\n",get_piece2(gamept->board,rank,file),
        get_piece2(gamept->board,rank+direction,capture_file));/*for now*/
      if ((get_piece2(gamept->board,rank,file) == direction) &&
        (get_piece2(gamept->board,rank+direction,capture_file) * direction < 0)) {

        if ((wordlen == 3) || (wordlen == 4))
          if (word[2] - '1' != rank+direction)
            continue;

        move_ptr->from = POS_OF(rank,file);
        move_ptr->to = POS_OF(rank+direction,capture_file);

        if (wordlen == 4) {
          switch (word[3]) {
            case 'Q':
              move_ptr->special_move_info =
                SPECIAL_MOVE_PROMOTION_QUEEN;

              break;
            case 'R':
              move_ptr->special_move_info =
                SPECIAL_MOVE_PROMOTION_ROOK;

              break;
            case 'N':
              move_ptr->special_move_info =
                SPECIAL_MOVE_PROMOTION_KNIGHT;

              break;
            case 'B':
              move_ptr->special_move_info =
                SPECIAL_MOVE_PROMOTION_BISHOP;

              break;
          }
        }

        return 0;
      }
    }

    // handle en passant captures
    if (wordlen == 3) {
      if (direction == -1) {
        // black
        if (word[2] == '3') {
          if ((get_piece2(gamept->board,3,file) == direction) &&
              (get_piece2(gamept->board,3,capture_file) == direction * -1)) {
            move_ptr->from = POS_OF(3,file);
            move_ptr->to = POS_OF(2,capture_file);
            move_ptr->special_move_info |=
              SPECIAL_MOVE_CAPTURE | SPECIAL_MOVE_EN_PASSANT;
            return 0;
          }
        }
      }
      else {
        // white
        if (word[2] == '6') {
          if ((get_piece2(gamept->board,4,file) == direction) &&
              (get_piece2(gamept->board,4,capture_file) == direction * -1)) {
            move_ptr->from = POS_OF(4,file);
            move_ptr->to = POS_OF(5,capture_file);
            move_ptr->special_move_info |=
              SPECIAL_MOVE_CAPTURE | SPECIAL_MOVE_EN_PASSANT;
            return 0;
          }
        }
      }
    }

    return 13;
  }

  return 14;
}

int get_piece_id_ix(char piece)
{
  int which_piece;

  /* calculate the id to search for: */
  for (which_piece = 0; which_piece < NUM_PIECE_TYPES; which_piece++) {
    if (piece_ids[which_piece] == piece)
      break;
  }

  return which_piece;
}

int (*piece_functions[])(struct game *,int,int,int,int) = {
  rook_move,
  knight_move,
  bishop_move,
  queen_move,
  king_move
};

int do_piece_move(struct game *gamept,int direction,char *word,int wordlen,struct move *move_ptr)
{
  int which_piece;
  int search_piece;
  int curr_file;
  int file_start;
  int file_end;
  int curr_rank;
  int rank_start;
  int rank_end;
  int where;
  int to_file;
  int to_rank;
  int to_piece;
  int retval;
  unsigned char board[CHARS_IN_BOARD];
  bool bBlack;
  int dbg;

  if (wordlen == 4) {
    where = word[1];

    if ((where >= 'a') && (where <= 'h')) {
      file_start = where - 'a';
      file_end = file_start + 1;
      rank_start = 0;
      rank_end = 8;
    }
    else if ((where >= '1') && (where <= '8')) {
      file_start = 0;
      file_end = 8;
      rank_start = where - '1';
      rank_end = rank_start + 1;
    }
    else
      return 1;
  }
  else {
    file_start = 0;
    file_end = 8;
    rank_start = 0;
    rank_end = 8;
  }

  if (!get_to_position(word,wordlen,&to_file,&to_rank))
    return 2;

  which_piece = get_piece_id_ix(word[0]);

  /* error if not found (should never happen): */
  if (which_piece == NUM_PIECE_TYPES)
    return 3;

  search_piece = (which_piece + 2) * direction; /* calculate search id */
  to_piece = get_piece2(gamept->board,to_rank,to_file);

  /* don't allow a capture of same color piece*/
  if (to_piece * direction > 0)
    return 4;

  move_ptr->special_move_info = 0;

  /* search the board for the search piece: */
  for (curr_rank = rank_start; curr_rank < rank_end; curr_rank++) {
    for (curr_file = file_start; curr_file < file_end; curr_file++) {
      if (get_piece2(gamept->board,curr_rank,curr_file) == search_piece) {
        /* see if a possible piece move: */
        retval = (*piece_functions[which_piece])(
          gamept,curr_file,curr_rank,to_file,to_rank);

        if (!retval) {
          move_ptr->from = POS_OF(curr_rank,curr_file);
          move_ptr->to = POS_OF(to_rank,to_file);

          // don't allow the move if it would put the mover in check

          copy_board(gamept->board,board);
          bBlack = gamept->curr_move & 0x1;
          update_board(board,move_ptr,bBlack);

          if (gamept->curr_move == dbg_move)
            dbg = 1;

          if (!player_is_in_check(bBlack,board))
            return 0;  /* success */
        }
      }
    }
  }

  return 5;
}

int get_to_position(char *word,int wordlen,int *to_filept,int *to_rankpt)
{
  *to_filept = word[wordlen - 2] - 'a';

  if ((*to_filept < 0) || (*to_filept > 7))
    return false;

  *to_rankpt = word[wordlen - 1] - '1';

  if ((*to_rankpt < 0) || (*to_rankpt > 7))
    return false;

  return true;
}

int rook_move(
  struct game *gamept,
  int file1,
  int rank1,
  int file2,
  int rank2
)
{
  int n;

  if (file1 == file2) {
    if (rank1 > rank2) {
      for (n = rank2 + 1; n < rank1; n++)
        if (get_piece2(gamept->board,n,file1))
          return 1;  /* failure */
    }
    else
      for (n = rank1 + 1; n < rank2; n++)
        if (get_piece2(gamept->board,n,file1))
          return 2;  /* failure */

    return 0;  /* success */
  }

  if (rank1 == rank2) {
    if (file1 > file2) {
      for (n = file2 + 1; n < file1; n++)
        if (get_piece2(gamept->board,rank1,n))
          return 1;  /* failure */
    }
    else
      for (n = file1 + 1; n < file2; n++)
        if (get_piece2(gamept->board,rank1,n))
          return 2;  /* failure */

    return 0;  /* success */
  }

  return 3;  /* failure */
}

int knight_move(
  struct game *gamept,
  int file1,
  int rank1,
  int file2,
  int rank2
)
{
  int dist1;
  int dist2;

  dist1 = (file1 - file2);

  if (dist1 < 0)
    dist1 *= -1;

  dist2 = (rank1 - rank2);

  if (dist2 < 0)
    dist2 *= -1;

  if ((dist1 == 1) && (dist2 == 2))
    return 0;  /* success */

  if ((dist1 == 2) && (dist2 == 1))
    return 0;  /* success */

  return 1;    /* failure */
}

int bishop_move(
  struct game *gamept,
  int file1,
  int rank1,
  int file2,
  int rank2
)
{
  int dist1;
  int dist2;
  int file_dir;
  int rank_dir;

  dist1 = (file1 - file2);

  if (dist1 < 0) {
    dist1 *= -1;
    file_dir = 1;
  }
  else
    file_dir = -1;

  dist2 = (rank1 - rank2);

  if (dist2 < 0) {
    dist2 *= -1;
    rank_dir = 1;
  }
  else
    rank_dir = -1;

  if (dist1 != dist2)
    return 1;  /* failure */

  /* make sure there are no intervening pieces */
  for ( ; ; ) {
    file1 += file_dir;

    if (file1 == file2)
      break;

    rank1 += rank_dir;

    if (get_piece2(gamept->board,rank1,file1))
      return 2;  /* failure */
  }

  return 0;  /* success */
}

int queen_move(
  struct game *gamept,
  int file1,
  int rank1,
  int file2,
  int rank2
)
{
  if (!rook_move(gamept,file1,rank1,file2,rank2))
    return 0;  /* success */

  if (!bishop_move(gamept,file1,rank1,file2,rank2))
    return 0;  /* success */

  return 1;    /* failure */
}

int king_move(
  struct game *gamept,
  int file1,
  int rank1,
  int file2,
  int rank2
)
{
  int dist1;
  int dist2;

  dist1 = (file1 - file2);

  if (dist1 < 0)
    dist1 *= -1;

  dist2 = (rank1 - rank2);

  if (dist2 < 0)
    dist2 *= -1;

  if ((dist1 < 2) && (dist2 < 2))
    return 0;  /* success */

  return 1;  /* failure */
}
