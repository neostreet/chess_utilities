#define WHITE 0
#define BLACK 1
#define NUM_PLAYERS 2

#define NUM_RANKS 8
#define NUM_FILES 8

#define NUM_BOARD_SQUARES (NUM_RANKS * NUM_FILES)

#define CHARS_IN_BOARD \
(NUM_BOARD_SQUARES / 2)  // 64 squares / 2 (nibbles per char)

#define PAWN_ID           1
#define ROOK_ID           2
#define KNIGHT_ID         3
#define BISHOP_ID         4
#define QUEEN_ID          5
#define KING_ID           6
#define NUM_PIECE_TYPES_0 6
#define EMPTY_ID          7

#define FORCE_VALUE_QUEEN  9
#define FORCE_VALUE_ROOK   5
#define FORCE_VALUE_KNIGHT 3
#define FORCE_VALUE_BISHOP 3
#define FORCE_VALUE_PAWN   1
#define FORCE_VALUE_KING   0

#define SPECIAL_MOVE_EN_PASSANT       0x01
#define SPECIAL_MOVE_PROMOTION_QUEEN  0x02
#define SPECIAL_MOVE_PROMOTION_ROOK   0x04
#define SPECIAL_MOVE_PROMOTION_KNIGHT 0x08
#define SPECIAL_MOVE_PROMOTION_BISHOP 0x10
#define SPECIAL_MOVE_CHECK            0x20
#define SPECIAL_MOVE_CAPTURE          0x40
#define SPECIAL_MOVE_CASTLE           0x80
#define SPECIAL_MOVE_MATE            0x100

#define ORIG_FORCE_VALUE (FORCE_VALUE_QUEEN + 2 * FORCE_VALUE_ROOK + \
2 * FORCE_VALUE_KNIGHT + 2 * FORCE_VALUE_BISHOP + 8 * FORCE_VALUE_PAWN)

#define WORDLEN 80
#define MAX_MOVES 400

#define WIDTH_IN_PIXELS 50
#define XLEN WIDTH_IN_PIXELS
#define PIXELS_PER_BYTE 8
#define WIDTH_IN_BYTES ((WIDTH_IN_PIXELS + PIXELS_PER_BYTE - 1) / PIXELS_PER_BYTE)

#define HEIGHT_IN_PIXELS 50
#define YLEN HEIGHT_IN_PIXELS

#define IMAGEBUF_LEN (WIDTH_IN_BYTES * HEIGHT_IN_PIXELS)
#define PIECE_WIDTH IMAGEBUF_LEN

#define BITS_PER_BYTE 8

#define SHRUNK_WIDTH_IN_PIXELS (WIDTH_IN_PIXELS / 2)
#define SHRUNK_BITS_PER_PIXEL 2
#define SHRUNK_WIDTH_IN_BITS (SHRUNK_WIDTH_IN_PIXELS * SHRUNK_BITS_PER_PIXEL)
#define SHRUNK_WIDTH_IN_BYTES ((SHRUNK_WIDTH_IN_BITS + \
BITS_PER_BYTE - 1) / BITS_PER_BYTE)

#define SHRUNK_HEIGHT_IN_PIXELS (HEIGHT_IN_PIXELS / 2)

#define SHRUNK_IMAGEBUF_LEN (SHRUNK_WIDTH_IN_BYTES * SHRUNK_HEIGHT_IN_PIXELS)

#define BOARD_WIDTH (NUM_FILES * width_in_pixels)
#define BOARD_HEIGHT (NUM_RANKS * height_in_pixels)

#define NUM_PIECE_TYPES 5

struct move {
  char from;
  char to;
  int special_move_info;
};

#define BITS_PER_BOARD_SQUARE 4

#define FONT_HEIGHT 12
#define FONT_WIDTH 9

#define Y_PIXELS 200

#define MAX_ANNOTATION_LINE_LEN 25
#define MAX_ANNOTATION_LINES (Y_PIXELS / FONT_HEIGHT)

#define ANNOTATION_X (8 * XLEN + 2 + FONT_WIDTH)
#define ANNOTATION_Y 5

#define MAX_FILE_NAME_LEN 128
#define MAX_TITLE_LEN 128

struct game {
  char chessfilename[MAX_FILE_NAME_LEN];
  char title[MAX_TITLE_LEN];
  int orientation;
  int num_moves;
  int curr_move;
  unsigned char board[CHARS_IN_BOARD];  /* 8 columns * 8 rows / 2 (nibbles per char) */
  struct move moves[MAX_MOVES];
};

typedef char (*CHESS_FILE_LIST)[MAX_FILE_NAME_LEN];
