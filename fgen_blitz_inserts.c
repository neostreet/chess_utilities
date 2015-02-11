#include <stdio.h>
#include <string.h>

#define WHITE 0
#define BLACK 1

#define BLITZ_GAME_DATE                 0
#define BLITZ_GAME_TIME_CONTROL         1
#define BLITZ_GAME_OPPONENT_NAME        2
#define BLITZ_GAME_COLOR                3
#define BLITZ_GAME_NUM_HALF_MOVES       4
#define BLITZ_GAME_RESULT               5
#define BLITZ_GAME_TERMINATION          6
#define BLITZ_GAME_WHITE_ELO            7
#define BLITZ_GAME_BLACK_ELO            8
#define BLITZ_GAME_FIRST_MOVE           9
#define BLITZ_GAME_FIRST_TWO_MOVES     10
#define BLITZ_GAME_FIRST_THREE_MOVES   11
#define BLITZ_GAME_FIRST_FOUR_MOVES    12
#define NUM_BLITZ_GAME_ITEMS           13

#define MAX_FILENAME_LEN 1024
static char filename[MAX_FILENAME_LEN];

#define MAX_LINE_LEN 8192
static char line[MAX_LINE_LEN];

static char usage[] = "usage: fgen_blitz_inserts filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static char blitz_game_date_str[] = "[Date \"";
#define BLITZ_GAME_DATE_STR_LEN (sizeof (blitz_game_date_str) - 1)

static char *color_str[] = {
  "[White \"",
  "[Black \""
};

static char result_str[] = "[Result \"";
#define RESULT_STR_LEN (sizeof (result_str) - 1)

static char *elo_str[] = {
  "[WhiteElo \"",
  "[BlackElo \""
};

static char time_control_str[] = "[TimeControl \"";
#define TIME_CONTROL_STR_LEN (sizeof (time_control_str) - 1)

static char termination_str[] = "[Termination \"";
#define TERMINATION_STR_LEN (sizeof (termination_str) - 1)

static char won_on_str[] = " won on ";
#define WON_ON_STR_LEN (sizeof (won_on_str) - 1)

static char won_by_str[] = " won by ";
#define WON_BY_STR_LEN (sizeof (won_by_str) - 1)

static char won_hyphen_str[] = " won - ";
#define WON_HYPHEN_STR_LEN (sizeof (won_hyphen_str) - 1)

static char drawn_by_str[] = " drawn by ";
#define DRAWN_BY_STR_LEN (sizeof (drawn_by_str) - 1)

static char drawn_hyphen_str[] = " drawn - ";
#define DRAWN_HYPHEN_STR_LEN (sizeof (drawn_hyphen_str) - 1)

static char first_move_str[] = "1.";
#define FIRST_MOVE_STR_LEN (sizeof (first_move_str) - 1)

static char second_move_str[] = " 2.";
#define SECOND_MOVE_STR_LEN (sizeof (second_move_str) - 1)

static char third_move_str[] = " 3.";
#define THIRD_MOVE_STR_LEN (sizeof (third_move_str) - 1)

static char fourth_move_str[] = " 4.";
#define FOURTH_MOVE_STR_LEN (sizeof (fourth_move_str) - 1)

static char fifth_move_str[] = " 5.";
#define FIFTH_MOVE_STR_LEN (sizeof (fifth_move_str) - 1)

#define BLITZ_GAME_DATE_LEN 10
static char blitz_game_date[BLITZ_GAME_DATE_LEN+1];

#define COLOR_LEN 1
static char color[COLOR_LEN+1];

#define OPPONENT_NAME_MAX_LEN 25
static char opponent_name[OPPONENT_NAME_MAX_LEN+1];

#define RESULT_MAX_LEN 10
static char result[RESULT_MAX_LEN+1];

#define MY_RESULT_LEN 1
static char my_result[MY_RESULT_LEN+1];

#define ELO_MAX_LEN 10
static char elo[2][ELO_MAX_LEN+1];

#define TIME_CONTROL_MAX_LEN 10
static char time_control[TIME_CONTROL_MAX_LEN+1];

#define TERMINATION_MAX_LEN 50
static char termination[TERMINATION_MAX_LEN+1];

#define FIRST_MOVE_MAX_LEN 10
static char first_move[FIRST_MOVE_MAX_LEN+1];

#define FIRST_TWO_MOVES_MAX_LEN 25
static char first_two_moves[FIRST_TWO_MOVES_MAX_LEN+1];

#define FIRST_THREE_MOVES_MAX_LEN 35
static char first_three_moves[FIRST_THREE_MOVES_MAX_LEN+1];

#define FIRST_FOUR_MOVES_MAX_LEN 45
static char first_four_moves[FIRST_FOUR_MOVES_MAX_LEN+1];

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
static int Contains(bool bCaseSens,char *line,int line_len,
  char *string,int string_len,int *index);
static int get_blitz_game_date(char *line,int line_len,int ix,
  char *blitz_game_date);
static int process_color_str(
  int color,
  char *line,int line_len,int ix,
  char *opponent_name,int opponent_name_max_len,
  char *color_ptr);
static int get_result(char *line,int line_len,int ix,
  char *result,int result_max_len,char *color,char *my_result);
static int get_elo(char *line,int line_len,int ix,
  char *elo,int elo_max_len);
static int get_time_control(char *line,int line_len,int ix,
  char *time_control,int time_control_max_len);
static int get_termination(char *line,int line_len,int ix,
  char *termination,int termination_max_len);
static int get_num_half_moves(char *line,int line_len,int *num_half_moves_ptr);

int main(int argc,char **argv)
{
  int n;
  int color_str_len[2];
  int elo_str_len[2];
  FILE *fptr0;
  int filename_len;
  FILE *fptr;
  int line_len;
  int line_no;
  int ix;
  int retval;
  int num_half_moves;
  bool bHaveItem[NUM_BLITZ_GAME_ITEMS];

  if (argc != 2) {
    printf(usage);
    return 1;
  }

  if ((fptr0 = fopen(argv[1],"r")) == NULL) {
    printf(couldnt_open,argv[1]);
    return 2;
  }

  printf("use chess\n\n");

  for (n = 0; n < 2; n++) {
    color_str_len[n] = strlen(color_str[n]);
    elo_str_len[n] = strlen(elo_str[n]);
  }

  for ( ; ; ) {
    GetLine(fptr0,filename,&filename_len,MAX_FILENAME_LEN);

    if (feof(fptr0))
      break;

    if ((fptr = fopen(filename,"r")) == NULL) {
      printf(couldnt_open,filename);
      continue;
    }

    line_no = 0;

    for (n = 0; n < NUM_BLITZ_GAME_ITEMS; n++)
      bHaveItem[n] = false;

    for ( ; ; ) {
      GetLine(fptr,line,&line_len,MAX_LINE_LEN);

      if (feof(fptr))
        break;

      line_no++;

      if (Contains(true,
        line,line_len,
        blitz_game_date_str,BLITZ_GAME_DATE_STR_LEN,
        &ix)) {

        retval = get_blitz_game_date(line,line_len,ix+BLITZ_GAME_DATE_STR_LEN,
          blitz_game_date);

        if (retval) {
          printf("get_date() failed on line %d: %d\n",line_no,retval);
          fclose(fptr);
          continue;
        }

        bHaveItem[BLITZ_GAME_DATE] = true;
      }
      else if (Contains(true,
        line,line_len,
        color_str[WHITE],color_str_len[WHITE],
        &ix)) {

        retval = process_color_str(WHITE,line,line_len,ix+color_str_len[WHITE],
          opponent_name,OPPONENT_NAME_MAX_LEN,color);

        if (retval) {
          printf("process_color_str() failed on line %d: %d\n",line_no,retval);
          fclose(fptr);
          continue;
        }
      }
      else if (Contains(true,
        line,line_len,
        color_str[BLACK],color_str_len[BLACK],
        &ix)) {

        retval = process_color_str(BLACK,line,line_len,ix+color_str_len[BLACK],
          opponent_name,OPPONENT_NAME_MAX_LEN,color);

        if (retval) {
          printf("process_color_str() failed on line %d: %d\n",line_no,retval);
          fclose(fptr);
          continue;
        }

        bHaveItem[BLITZ_GAME_OPPONENT_NAME] = true;
        bHaveItem[BLITZ_GAME_COLOR] = true;
      }
      else if (Contains(true,
        line,line_len,
        result_str,RESULT_STR_LEN,
        &ix)) {

        retval = get_result(line,line_len,ix+RESULT_STR_LEN,
          result,RESULT_MAX_LEN,color,my_result);

        if (retval) {
          printf("get_result() failed on line %d: %d\n",line_no,retval);
          fclose(fptr);
          continue;
        }

        bHaveItem[BLITZ_GAME_RESULT] = true;
      }
      else if (Contains(true,
        line,line_len,
        elo_str[WHITE],elo_str_len[WHITE],
        &ix)) {

        retval = get_elo(line,line_len,ix+elo_str_len[WHITE],
          elo[WHITE],ELO_MAX_LEN);

        if (retval) {
          printf("get_elo() failed on line %d: %d\n",line_no,retval);
          fclose(fptr);
          continue;
        }

        bHaveItem[BLITZ_GAME_WHITE_ELO] = true;
      }
      else if (Contains(true,
        line,line_len,
        elo_str[BLACK],elo_str_len[BLACK],
        &ix)) {

        retval = get_elo(line,line_len,ix+elo_str_len[BLACK],
          elo[BLACK],ELO_MAX_LEN);

        if (retval) {
          printf("get_elo() failed on line %d: %d\n",line_no,retval);
          fclose(fptr);
          continue;
        }

        bHaveItem[BLITZ_GAME_BLACK_ELO] = true;
      }
      else if (Contains(true,
        line,line_len,
        time_control_str,TIME_CONTROL_STR_LEN,
        &ix)) {

        retval = get_time_control(line,line_len,ix+TIME_CONTROL_STR_LEN,
          time_control,TIME_CONTROL_MAX_LEN);

        if (retval) {
          printf("get_time_control() failed on line %d: %d\n",line_no,retval);
          fclose(fptr);
          continue;
        }

        bHaveItem[BLITZ_GAME_TIME_CONTROL] = true;
      }
      else if (Contains(true,
        line,line_len,
        termination_str,TERMINATION_STR_LEN,
        &ix)) {

        retval = get_termination(line,line_len,ix+TERMINATION_STR_LEN,
          termination,TERMINATION_MAX_LEN);

        if (retval) {
          printf("get_termination() failed on line %d: %d\n",line_no,retval);
          fclose(fptr);
          continue;
        }

        bHaveItem[BLITZ_GAME_TERMINATION] = true;
      }
      else if (Contains(true,
        line,line_len,
        first_move_str,FIRST_MOVE_STR_LEN,
        &ix)) {

        retval = get_num_half_moves(line,line_len,&num_half_moves);

        if (retval) {
          printf("get_num_half_moves() failed on line %d: %d\n",line_no,retval);
          fclose(fptr);
          continue;
        }

        bHaveItem[BLITZ_GAME_NUM_HALF_MOVES] = true;

        if (!ix) {
          if (Contains(true,
            line,line_len,
            fifth_move_str,FIFTH_MOVE_STR_LEN,
            &ix)) {

            if (ix <= FIRST_FOUR_MOVES_MAX_LEN) {
              line[ix] = 0;
              strcpy(first_four_moves,line);
              bHaveItem[BLITZ_GAME_FIRST_FOUR_MOVES] = true;
            }
          }

          if (Contains(true,
            line,line_len,
            fourth_move_str,FOURTH_MOVE_STR_LEN,
            &ix)) {

            if (ix <= FIRST_THREE_MOVES_MAX_LEN) {
              line[ix] = 0;
              strcpy(first_three_moves,line);
              bHaveItem[BLITZ_GAME_FIRST_THREE_MOVES] = true;
            }
          }

          if (Contains(true,
            line,line_len,
            third_move_str,THIRD_MOVE_STR_LEN,
            &ix)) {

            if (ix <= FIRST_TWO_MOVES_MAX_LEN) {
              line[ix] = 0;
              strcpy(first_two_moves,line);
              bHaveItem[BLITZ_GAME_FIRST_TWO_MOVES] = true;
            }
          }

          if (Contains(true,
            line,line_len,
            second_move_str,SECOND_MOVE_STR_LEN,
            &ix)) {

            if (ix <= FIRST_MOVE_MAX_LEN) {
              line[ix] = 0;
              strcpy(first_move,line);
              bHaveItem[BLITZ_GAME_FIRST_MOVE] = true;
            }
          }
        }
      }
    }

    fclose(fptr);

    for (n = 0; n < NUM_BLITZ_GAME_ITEMS; n++) {
      if (!bHaveItem[n])
        break;
    }

    if (n != NUM_BLITZ_GAME_ITEMS) {
      printf("%s: missing information: %d\n",
        filename,n);
    }
    else {
      printf("insert into blitz_games(\n");
      printf("  blitz_game_date,\n");
      printf("  time_control,\n");
      printf("  opponent_name,\n");
      printf("  color,\n");
      printf("  num_half_moves,\n");
      printf("  result,\n");
      printf("  result_detail,\n");
      printf("  opponent_elo_after,\n");
      printf("  my_elo_after,\n");
      printf("  first_move,\n");
      printf("  first_two_moves,\n");
      printf("  first_three_moves,\n");
      printf("  first_four_moves\n");
      printf(") values (\n");
      printf("  '%s',\n",blitz_game_date);
      printf("  '%s',\n",time_control);
      printf("  '%s',\n",opponent_name);
      printf("  '%s',\n",color);
      printf("  %d,\n",num_half_moves);
      printf("  '%s',\n",my_result);
      printf("  '%s',\n",termination);
      printf("  %s,\n",
        ((color[0] == 'W') ? elo[BLACK] : elo[WHITE]));
      printf("  %s,\n",
        ((color[0] == 'W') ? elo[WHITE] : elo[BLACK]));
      printf("  '%s',\n",first_move);
      printf("  '%s',\n",first_two_moves);
      printf("  '%s',\n",first_three_moves);
      printf("  '%s'\n",first_four_moves);
      printf(");\n");
    }
  }

  printf("\nquit\n");

  fclose(fptr0);

  return 0;
}

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen)
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

static int Contains(bool bCaseSens,char *line,int line_len,
  char *string,int string_len,int *index)
{
  int m;
  int n;
  int tries;
  char chara;

  tries = line_len - string_len + 1;

  if (tries <= 0)
    return false;

  for (m = 0; m < tries; m++) {
    for (n = 0; n < string_len; n++) {
      chara = line[m + n];

      if (!bCaseSens) {
        if ((chara >= 'A') && (chara <= 'Z'))
          chara += 'a' - 'A';
      }

      if (chara != string[n])
        break;
    }

    if (n == string_len) {
      *index = m;
      return true;
    }
  }

  return false;
}

static int get_blitz_game_date(char *line,int line_len,int ix,
  char *blitz_game_date)
{
  int n;

  if (line_len - ix < BLITZ_GAME_DATE_LEN + 1)
    return 1;

  for (n = 0; n < BLITZ_GAME_DATE_LEN; n++) {
    if (line[ix + n] == '.')
      blitz_game_date[n] = '-';
    else
      blitz_game_date[n] = line[ix + n];
  }

  blitz_game_date[n] = 0;

  return 0;
}

static int process_color_str(
  int color,
  char *line,int line_len,int ix,
  char *opponent_name,int opponent_name_max_len,
  char *color_ptr)
{
  int n;

  if (!strncmp(&line[ix],"neostreet",9)) {
    if (color == WHITE)
      color_ptr[0] = 'W';
    else
      color_ptr[0] = 'B';

    color_ptr[1] = 0;
  }
  else {
    for (n = 0; n < opponent_name_max_len; n++) {
      if (line[ix + n] == '"')
        break;

      opponent_name[n] = line[ix + n];
    }

    if (n == opponent_name_max_len)
      return 1;

    opponent_name[n] = 0;
  }

  return 0;
}

static int get_result(char *line,int line_len,int ix,
  char *result,int result_max_len,char *color,char *my_result)
{
  int n;

  for (n = 0; n < result_max_len; n++) {
    if (line[ix + n] == '"')
      break;

    result[n] = line[ix + n];
  }

  if (n == result_max_len)
    return 1;

  result[n] = 0;

  if (!strcmp(result,"1-0")) {
    if (color[0] == 'W')
      my_result[0] = 'W';
    else
      my_result[0] = 'L';
  }
  else if (!strcmp(result,"0-1")) {
    if (color[0] == 'W')
      my_result[0] = 'L';
    else
      my_result[0] = 'W';
  }
  else if (!strcmp(result,"1/2-1/2"))
    my_result[0] = 'D';

  my_result[1] = 0;

  return 0;
}

static int get_elo(char *line,int line_len,int ix,
  char *elo,int elo_max_len)
{
  int n;

  for (n = 0; n < elo_max_len; n++) {
    if (line[ix + n] == '"')
      break;

    elo[n] = line[ix + n];
  }

  if (n == elo_max_len)
    return 1;

  elo[n] = 0;

  return 0;
}

static int get_time_control(char *line,int line_len,int ix,
  char *time_control,int time_control_max_len)
{
  int n;

  for (n = 0; n < time_control_max_len; n++) {
    if (line[ix + n] == '"')
      break;

    time_control[n] = line[ix + n];
  }

  if (n == time_control_max_len)
    return 1;

  time_control[n] = 0;

  return 0;
}

static int get_termination(char *line,int line_len,int ix,
  char *termination,int termination_max_len)
{
  int n;
  int ix2;

  if (Contains(true,
    line,line_len,
    won_on_str,WON_ON_STR_LEN,
    &ix2)) {

    ix = ix2 + WON_ON_STR_LEN;
  }
  else if (Contains(true,
    line,line_len,
    won_by_str,WON_BY_STR_LEN,
    &ix2)) {

    ix = ix2 + WON_BY_STR_LEN;
  }
  else if (Contains(true,
    line,line_len,
    won_hyphen_str,WON_HYPHEN_STR_LEN,
    &ix2)) {

    ix = ix2 + WON_HYPHEN_STR_LEN;
  }
  else if (Contains(true,
    line,line_len,
    drawn_by_str,DRAWN_BY_STR_LEN,
    &ix2)) {

    ix = ix2 + DRAWN_BY_STR_LEN;
  }
  else if (Contains(true,
    line,line_len,
    drawn_hyphen_str,DRAWN_HYPHEN_STR_LEN,
    &ix2)) {

    ix = ix2 + DRAWN_HYPHEN_STR_LEN;
  }

  for (n = 0; n < termination_max_len; n++) {
    if (line[ix + n] == '"')
      break;

    termination[n] = line[ix + n];
  }

  if (n == termination_max_len)
    return 1;

  termination[n] = 0;

  return 0;
}

static int get_num_half_moves(char *line,int line_len,int *num_half_moves_ptr)
{
  int n;
  bool bSpace;
  int last_move_number;
  int num_half_moves;

  bSpace = false;

  for (n = line_len - 1; n >= 0; n--) {
    if (line[n] == ' ')
      bSpace = true;
    else if (line[n] == '.')
      break;
  }

  if (n < 0)
    return 1;

  for ( ; n >= 0; n--) {
    if (line[n] == ' ')
      break;
  }

  if (n < 0)
    return 2;

  sscanf(&line[n+1],"%d",&last_move_number);

  num_half_moves = last_move_number * 2;

  if (!bSpace)
    num_half_moves--;

  *num_half_moves_ptr = num_half_moves;

  return 0;
}
