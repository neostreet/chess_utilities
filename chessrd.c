#include <vector>

using namespace std;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

#include "chess.h"
#include "chess.fun"
#include "chess.glb"
#include "chess.mac"
#include "bitfuns.h"

static unsigned char initial_board[] = {
  (unsigned char)0x23, (unsigned char)0x45, (unsigned char)0x64, (unsigned char)0x32,
  (unsigned char)0x11, (unsigned char)0x11, (unsigned char)0x11, (unsigned char)0x11,
  (unsigned char)0x00, (unsigned char)0x00, (unsigned char)0x00, (unsigned char)0x00,
  (unsigned char)0x00, (unsigned char)0x00, (unsigned char)0x00, (unsigned char)0x00,
  (unsigned char)0x00, (unsigned char)0x00, (unsigned char)0x00, (unsigned char)0x00,
  (unsigned char)0x00, (unsigned char)0x00, (unsigned char)0x00, (unsigned char)0x00,
  (unsigned char)0xff, (unsigned char)0xff, (unsigned char)0xff, (unsigned char)0xff,
  (unsigned char)0xed, (unsigned char)0xcb, (unsigned char)0xac, (unsigned char)0xde
};

static int force_values[] = {
  FORCE_VALUE_PAWN,
  FORCE_VALUE_ROOK,
  FORCE_VALUE_KNIGHT,
  FORCE_VALUE_BISHOP,
  FORCE_VALUE_QUEEN,
  FORCE_VALUE_KING
};

static char corrupted_msg[] = "game corrupted\n";
static char bad_castle[] = "bad castle";
static char bad_pawn_move[] = "bad pawn move";

static char *bad_piece_move[] = {
  "bad rook move",
  "bad knight move",
  "bad bishop move",
  "bad queen move",
  "bad king move"
};

void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
static int fen2pos(char *line,int line_len,unsigned char *pos_buf,int *black_to_play);

short force_value_of(short piece)
{
  if (piece < 0)
    piece *= -1;

  piece--;

  return force_values[piece];
}

int line_number(char *word,int wordlen)
{
  int n;

  for (n = 0; n < wordlen; n++) {
    if ((word[n] < '0') || (word[n] > '9')) {
      if ((word[n] == '.') && (n == wordlen - 1))
        break;

      return false;
    }
  }

  return true;
}

int get_piece_type_ix(int chara)
{
  int n;

  for (n = 0; n < NUM_PIECE_TYPES; n++)
    if (chara == piece_ids[n])
      return n;

  return 0; /* should never happen */
}

void set_initial_board(struct game *gamept)
{
  int n;

  for (n = 0; n < CHARS_IN_BOARD; n++)
    gamept->board[n] = initial_board[n];

  for (n = 0; n < 2; n++)
    gamept->seirawan_count[n] = 0;

  refresh_force_count(gamept);
  set_piece_offsets(gamept);
}

void set_piece_offsets(struct game *gamept)
{
  int m;
  int n;
  int p;
  int piece;

  for (n = 0; n < NUM_PLAYERS; n++)
    gamept->num_pieces[n] = 0;

  p = 0;

  for (m = 0; m < NUM_RANKS; m++) {
    for (n = 0; n < NUM_FILES; n++) {
      piece = get_piece2(gamept->board,m,n);

      if (piece > 0)
        gamept->piece_offsets[WHITE][gamept->num_pieces[WHITE]++] = p;
      else if (piece < 0)
        gamept->piece_offsets[BLACK][gamept->num_pieces[BLACK]++] = p;

      p++;
    }
  }
}

int read_game(char *filename,struct game *gamept,char *err_msg)
{
  FILE *fptr;
  int chara;
  int end_of_file;
  int m;
  int n;
  char word[WORDLEN];
  int wordlen;
  bool bHaveFirstWord;
  int direction;
  int word_no;
  int dbg;
  int retval;
  int got_error;
  struct move move;

  gamept->curr_move = -1;

  if ((fptr = fopen(filename,"r")) == NULL)
    return 1;

  strcpy(gamept->chessfilename,filename);

  fscanf(fptr,"%d",&gamept->orientation);  /* get board orientation */
                                   /* 0 = standard, 1 = black on bottom */

  end_of_file = get_word(fptr,word,WORDLEN,&wordlen);

  if (!strncmp(word,"FEN:",4)) {
    retval = read_fen(fptr,gamept);

    if (retval) {
      printf("read_fen() failed: %d\n",retval);
      fclose(fptr);
      return 2;
    }

    bHaveFirstWord = false;
  }
  else
    bHaveFirstWord = true;

  set_initial_board(gamept);

  end_of_file = 0;
  gamept->curr_move = 0;

  gamept->title[0] = 0;
  word_no = 0;
  got_error = 0;

  gamept->moves.clear();

  for ( ; ; ) {
    if (word_no || !bHaveFirstWord)
      end_of_file = get_word(fptr,word,WORDLEN,&wordlen);

    if (end_of_file)
      break;

    word_no++;

    if (gamept->curr_move == dbg_move)
      dbg = 1;

    /* ignore line numbers: */
    if (line_number(word,wordlen))
      continue;

    if (!strncmp(word,"title",5)) {
      m = 5;
      n = 0;

      for ( ; ; ) {
        chara = word[m++];

        if (!chara)
          break;

        if (n < 79)
          if (chara == '\\')
            gamept->title[n++] = ' ';
          else
            gamept->title[n++] = chara;
      }

      gamept->title[n] = 0;

      continue;
    }

    if (gamept->curr_move & 0x1)
      direction = -1;           /* black's move */
    else
      direction = 1;            /* white's move */

    move.special_move_info = 0;

    switch(word[0]) {
      case 'O':
      case '0':
        retval = do_castle(gamept,direction,word,wordlen,&move);

        if (retval) {
          /*printf(corrupted_msg);*/
          strcpy(err_msg,bad_castle);

          got_error = 1;
        }

        break;

      case 'R':
      case 'N':
      case 'B':
      case 'Q':
      case 'K':
        retval = do_piece_move(gamept,direction,word,wordlen,&move);

        if (retval) {
          /*printf(corrupted_msg);*/
          strcpy(err_msg,bad_piece_move[get_piece_type_ix(word[0])]);

          got_error = 1;
        }

        break;

      default:
        retval = do_pawn_move(gamept,direction,word,wordlen,&move);

        if (retval) {
          /*printf(corrupted_msg);*/
          strcpy(err_msg,bad_pawn_move);

          got_error = 1;
        }

        break;
    }

    if (got_error)
      break;

    gamept->moves.push_back(move);

    update_board(gamept,false);

    if (player_is_in_check(((gamept->curr_move + 1) % 2),gamept)) {
      got_error = 1;
      break;
    }

    gamept->curr_move++;
  }

  gamept->num_moves = gamept->curr_move;
  calculate_seirawan_counts(gamept);

  fclose(fptr);

  if (got_error)
    return 3;

  return 0;
}

int read_binary_game(char *filename,struct game *gamept)
{
  int fhndl;
  unsigned int bytes_to_read;
  unsigned int bytes_read;

  if ((fhndl = open(filename,O_RDONLY | O_BINARY)) == -1)
    return 1;

  bytes_to_read = sizeof (struct game) - sizeof gamept->moves;

  bytes_read = read(fhndl,(char *)gamept,bytes_to_read);

  if (bytes_read != bytes_to_read) {
    close(fhndl);
    return 2;
  }

  bytes_to_read = gamept->num_moves * sizeof (struct move);

  //bytes_read = read(fhndl,(char *)gamept->moves,bytes_to_read);

  bytes_read = 0;

  if (bytes_read != bytes_to_read) {
    close(fhndl);
    return 3;
  }

  close(fhndl);

  return 0;
}

int write_binary_game(char *filename,struct game *gamept)
{
  int fhndl;
  unsigned int bytes_to_write;
  unsigned int bytes_written;

  if ((fhndl = open(filename,O_CREAT | O_TRUNC | O_WRONLY | O_BINARY,
      S_IREAD | S_IWRITE)) == -1)
    return 1;

  bytes_to_write = sizeof (struct game) - sizeof (struct move *);

  bytes_written = write(fhndl,(char *)gamept,bytes_to_write);

  if (bytes_written != bytes_to_write) {
    close(fhndl);
    return 2;
  }

  bytes_to_write = gamept->num_moves * sizeof (struct move);

  //bytes_written = write(fhndl,(char *)gamept->moves,bytes_to_write);

  bytes_written = 0;

  if (bytes_written != bytes_to_write) {
    close(fhndl);
    return 3;
  }

  close(fhndl);

  return 0;
}

int ignore_character(int chara)
{
  if ((chara == 0x0d) ||
    (chara == '(') ||
    (chara == ')') ||
    (chara == 'x') ||
    (chara == '=') ||
    (chara == '+'))
    return true;

  return false;
}

int get_word(FILE *fptr,char *word,int maxlen,int *wordlenpt)
{
  int chara;
  int started;
  int comment;
  int end_of_file;
  int wordlen;

  wordlen = 0;
  started = 0;
  comment = 0;
  end_of_file = 0;

  for ( ; ; ) {
    chara = fgetc(fptr);

    /* end of file ? */
    if (feof(fptr)) {
      end_of_file = 1;
      fclose(fptr);
      break;
    }

    // ignore carriage returns and other characters
    if (ignore_character(chara))
      continue;

    /* end of line ? */
    if (chara == 0x0a) {
      if (started)
        break;
      else {
        comment = 0;
        continue;
      }
    }

    /* in comment ? */
    if (comment)
      continue;

    /* comment marker ? */
    if (chara == '/') {
      comment = 1;
      continue;
    }

    /* white space ? */
    if ((chara == 0x09) || (chara == ' '))
      if (started)
        break;
      else
        continue;

    if (!(started))
      started = 1;

    if (wordlen < maxlen - 1)
      word[wordlen++] = chara;
  }

  word[wordlen] = 0;
  *wordlenpt = wordlen;

  return end_of_file;
}

void update_board(struct game *gamept,short bCalcCounts)
{
  int dbg;
  int rank;
  int file;
  int old_piece;
  int piece;

  if (gamept->moves[gamept->curr_move].special_move_info) {
    switch (gamept->moves[gamept->curr_move].special_move_info & 0x1f) {
      case SPECIAL_MOVE_PROMOTION_QUEEN:
        if (gamept->curr_move & 0x1)
          // black
          set_piece1(gamept->board,gamept->moves[gamept->curr_move].from,QUEEN_ID * -1);
        else
          set_piece1(gamept->board,gamept->moves[gamept->curr_move].from,QUEEN_ID);

        break;
      case SPECIAL_MOVE_PROMOTION_ROOK:
        if (gamept->curr_move & 0x1)
          // black
          set_piece1(gamept->board,gamept->moves[gamept->curr_move].from,ROOK_ID * -1);
        else
          set_piece1(gamept->board,gamept->moves[gamept->curr_move].from,ROOK_ID);

        break;
      case SPECIAL_MOVE_PROMOTION_KNIGHT:
        if (gamept->curr_move & 0x1)
          // black
          set_piece1(gamept->board,gamept->moves[gamept->curr_move].from,KNIGHT_ID * -1);
        else
          set_piece1(gamept->board,gamept->moves[gamept->curr_move].from,KNIGHT_ID);

        break;
      case SPECIAL_MOVE_PROMOTION_BISHOP:
        if (gamept->curr_move & 0x1)
          // black
          set_piece1(gamept->board,gamept->moves[gamept->curr_move].from,BISHOP_ID * -1);
        else
          set_piece1(gamept->board,gamept->moves[gamept->curr_move].from,BISHOP_ID);

        break;
      case SPECIAL_MOVE_EN_PASSANT:
        rank = RANK_OF(gamept->moves[gamept->curr_move].from);
        file = FILE_OF(gamept->moves[gamept->curr_move].to);
        set_piece1(gamept->board,POS_OF(rank,file),0);

        break;
    }
  }

  old_piece = get_piece1(gamept->board,gamept->moves[gamept->curr_move].to);

  if (old_piece)
    gamept->moves[gamept->curr_move].special_move_info |= SPECIAL_MOVE_CAPTURE;

  piece = get_piece1(gamept->board,gamept->moves[gamept->curr_move].from);

  set_piece1(gamept->board,gamept->moves[gamept->curr_move].to,piece);

  set_piece1(gamept->board,gamept->moves[gamept->curr_move].from,0);  /* vacate previous square */

  // now move the rook if this was a castling move
  if (piece == KING_ID) {
    // white

    if ((gamept->moves[gamept->curr_move].from == 4) &&
      (gamept->moves[gamept->curr_move].to == 6)) {
      // kingside castle
      set_piece1(gamept->board,5,ROOK_ID);
      set_piece1(gamept->board,7,0);
    }
    else if ((gamept->moves[gamept->curr_move].from == 4) &&
      (gamept->moves[gamept->curr_move].to == 2)) {
      // kingside castle
      set_piece1(gamept->board,3,ROOK_ID);
      set_piece1(gamept->board,0,0);
    }
  }
  else if (piece == KING_ID * -1) {
    // black

    if ((gamept->moves[gamept->curr_move].from == 60) &&
      (gamept->moves[gamept->curr_move].to == 62)) {
      // kingside castle
      set_piece1(gamept->board,61,ROOK_ID * -1);
      set_piece1(gamept->board,63,0);
    }
    else if ((gamept->moves[gamept->curr_move].from == 60) &&
      (gamept->moves[gamept->curr_move].to == 58)) {
      // kingside castle
      set_piece1(gamept->board,59,ROOK_ID * -1);
      set_piece1(gamept->board,56,0);
    }
  }

  if (bCalcCounts)
    calculate_seirawan_counts(gamept);

  set_piece_offsets(gamept);

  // now check if this move has put the other player in check
  if (player_is_in_check(!((gamept->curr_move + 1) % 2),gamept)) {
    // mark this move as a check
    gamept->moves[gamept->curr_move].special_move_info |= SPECIAL_MOVE_CHECK;
  }
}

int get_piece1(unsigned char *board,int board_offset)
{
  unsigned int bit_offset;
  unsigned short piece;
  int piece_int;

  bit_offset = board_offset * BITS_PER_BOARD_SQUARE;

  piece = get_bits(BITS_PER_BOARD_SQUARE,board,bit_offset);
  piece_int = piece;

  if (piece & 0x8)
    piece_int |= 0xfffffff0;

  return piece_int;
}

int get_piece2(unsigned char *board,int row,int column)
{
  int board_offset;

  board_offset = row * 8 + column;
  return get_piece1(board,board_offset);
}

void set_piece1(unsigned char *board,int board_offset,int piece)
{
  unsigned int bit_offset;

  bit_offset = board_offset * BITS_PER_BOARD_SQUARE;
  set_bits(BITS_PER_BOARD_SQUARE,board,bit_offset,piece);
}

void set_piece2(unsigned char *board,int row,int column,int piece)
{
  int board_offset;

  board_offset = row * 8 + column;
  set_piece1(board,board_offset,piece);
}

void calculate_seirawan_counts(struct game *gamept)
{
  int m;
  int n;
  int o;
  int direction;
  int piece;
  int low;
  int high;

  for (m = 0; m < 2; m++) {
    gamept->seirawan_count[m] = 0;

    if (!m)
      direction = 1;
    else
      direction = -1;

    for (n = 0; n < NUM_BOARD_SQUARES; n++) {
      piece = get_piece1(gamept->board,n);

      if (piece * direction > 0) {
        if (piece < 0) {
          low = 0;
          high = 32;
        }
        else {
          low = 32;
          high = 64;
        }

        for (o = low; o < high; o++) {
          if (square_attacks_square(gamept,n,o))
            gamept->seirawan_count[m]++;
        }
      }
    }
  }
}

#define MAX_FEN_LINE_LEN 256
static char fen_line[MAX_FEN_LINE_LEN];

int read_fen(FILE *fptr,struct game *gamept)
{
  int fen_line_len;
  int retval;

  GetLine(fptr,fen_line,&fen_line_len,MAX_FEN_LINE_LEN);

  retval = fen2pos(fen_line,fen_line_len,initial_board,
    &gamept->black_to_play);

  if (retval)
    return retval;

  return 0;
}

void GetLine(FILE *fptr,char *line,int *line_len,int maxllen)
{
  int chara;
  int local_line_len;

  local_line_len = 0;

  for ( ; ; ) {
    chara = fgetc(fptr);

    if (feof(fptr))
      break;

    if (chara == '\n')
      break;

    if (local_line_len < maxllen - 1)
      line[local_line_len++] = (char)chara;
  }

  line[local_line_len] = 0;
  *line_len = local_line_len;
}

static int fen2pos(char *line,int line_len,unsigned char *pos_buf,int *black_to_play)
{
  int m;
  int n;
  int p;
  int forward_slash_count;
  int rank_count;
  int chara;
  int contig_square_no_piece_count;
  int piece;

  m = 0;

  for (n = 0; n < CHARS_IN_BOARD; n++)
    pos_buf[n] = 0;

  forward_slash_count = 0;
  rank_count = 0;

  *black_to_play = -1;

  for (n = 0; n < line_len; n++) {
    chara = line[n];

    if (chara == ' ') {
      forward_slash_count++;

      continue;
    }

    if (forward_slash_count == 8) {
      if (chara == 'w') {
        *black_to_play = 0;
        break;
      }
      if (chara == 'b') {
        *black_to_play = 1;
        break;
      }
    }

    if (chara == '/') {
      forward_slash_count++;

      if (rank_count != 8)
        return 1;

      rank_count = 0;
    }
    else if ((chara >= '1') && (chara <= '8'))
      rank_count += chara - '1' + 1;
    else {
      if ((chara == 'P') ||
          (chara == 'R') ||
          (chara == 'N') ||
          (chara == 'B') ||
          (chara == 'Q') ||
          (chara == 'K') ||
          (chara == 'p') ||
          (chara == 'r') ||
          (chara == 'n') ||
          (chara == 'b') ||
          (chara == 'q') ||
          (chara == 'k'))
        rank_count++;
      else
        return 2;
    }
  }

  if (*black_to_play == -1)
    return 3;

  p = 0;

  for (m = 0; m < 8; m++) {
    for (n = 0; n < 8; n++) {
      chara = line[p++];

      if ((chara >= '1') && (chara <= '8')) {
        contig_square_no_piece_count = chara - '1' + 1;
        n += contig_square_no_piece_count - 1;
      }
      else {
        if (chara == 'P')
          piece = PAWN_ID;
        else if (chara == 'R')
          piece = ROOK_ID;
        else if (chara == 'N')
          piece = KNIGHT_ID;
        else if (chara == 'B')
          piece = BISHOP_ID;
        else if (chara == 'Q')
          piece = QUEEN_ID;
        else if (chara == 'K')
          piece = KING_ID;
        else if (chara == 'p')
          piece = PAWN_ID * -1;
        else if (chara == 'r')
          piece = ROOK_ID * -1;
        else if (chara == 'n')
          piece = KNIGHT_ID * -1;
        else if (chara == 'b')
          piece = BISHOP_ID * -1;
        else if (chara == 'q')
          piece = QUEEN_ID * -1;
        else
          piece = KING_ID * -1;

        set_bits(4,pos_buf,((7 - m) * 8 * 4) + (n * 4),piece);
      }
    }

    if (m < 7)
      p++;
  }

  return 0;
}

#define MAX_LINE_LEN 256
static char line[MAX_LINE_LEN];

int populate_board_from_board_file(unsigned char *board,char *filename)
{
  int m;
  int n;
  FILE *fptr;
  int line_len;
  int line_no;
  int chara;
  int piece;

  if ((fptr = fopen(filename,"r")) == NULL)
    return 1;

  line_no = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    if (line_len != 15)
      return 2;

    for (n = 0; n < NUM_FILES; n++) {
      chara = line[n*2];

      if (chara == '.') {
        piece = 0;
        set_piece2(board,7 - line_no,n,piece);
      }
      else {
        if (chara == 'p')
          set_piece2(board,7 - line_no,n,1);
        else if (chara == 'P')
          set_piece2(board,7 - line_no,n,-1);
        else {
          for (m = 0; m < NUM_PIECE_TYPES; m++) {
            if (chara == piece_ids[m]) {
              piece = (m + 2) * -1;
              break;
            }
            else if (chara == piece_ids[m] - 'A' + 'a') {
              piece = (m + 2);
              break;
            }
          }

          if (m < NUM_PIECE_TYPES)
            set_piece2(board,7 - line_no,n,piece);
        }
      }
    }

    line_no++;
  }

  fclose(fptr);

  return 0;
}

int populate_initial_board_from_board_file(char *filename)
{
  return populate_board_from_board_file(initial_board,filename);
}

int populate_board_from_bin_board_file(unsigned char *board,char *filename)
{
  struct stat stat_buf;
  int fhndl;
  unsigned int bytes_to_read;
  unsigned int bytes_read;

  if (stat(filename,&stat_buf) == -1)
    return 1;

  if (stat_buf.st_size != CHARS_IN_BOARD)
    return 2;

  if ((fhndl = open(filename,O_RDONLY | O_BINARY)) == -1)
    return 3;

  bytes_to_read = CHARS_IN_BOARD;

  bytes_read = read(fhndl,(char *)board,bytes_to_read);

  if (bytes_read != bytes_to_read) {
    close(fhndl);
    return 4;
  }

  close(fhndl);

  return 0;
}

int populate_initial_board_from_bin_board_file(char *filename)
{
  return populate_board_from_bin_board_file(initial_board,filename);
}

int write_board_to_binfile(unsigned char *board,char *filename)
{
  int fhndl;
  unsigned int bytes_to_write;
  unsigned int bytes_written;

  if ((fhndl = open(filename,O_CREAT | O_TRUNC | O_WRONLY | O_BINARY,
      S_IREAD | S_IWRITE)) == -1)
    return 1;

  bytes_to_write = CHARS_IN_BOARD;

  bytes_written = write(fhndl,(char *)board,bytes_to_write);

  if (bytes_written != bytes_to_write) {
    close(fhndl);
    return 2;
  }

  close(fhndl);

  return 0;
}

int refresh_force_count(struct game *gamept)
{
  int n;
  short piece;

  for (n = 0; n < 2; n++)
    gamept->force_count[n] = 0;

  for (n = 0; n < NUM_BOARD_SQUARES; n++) {
    piece = get_piece1(gamept->board,n);

    if (!piece)
      continue;

    if (piece < 0)
      gamept->force_count[BLACK] += force_value_of(piece);
    else
      gamept->force_count[WHITE] += force_value_of(piece);
  }

  if (gamept->force_count[WHITE] > gamept->force_count[BLACK])
    return gamept->force_count[WHITE] - gamept->force_count[BLACK];
  else
    return gamept->force_count[BLACK] - gamept->force_count[WHITE];
}
