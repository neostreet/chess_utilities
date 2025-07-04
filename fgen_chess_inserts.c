#include <stdio.h>
#include <string.h>

#define WHITE 0
#define BLACK 1

enum {
  CHESS_GAME_SITE,
  CHESS_GAME_DATE,
  CHESS_GAME_TIME_CONTROL,
  CHESS_GAME_ECO,
  CHESS_GAME_OPENING,
  CHESS_GAME_OPPONENT_NAME,
  CHESS_GAME_COLOR,
  CHESS_GAME_NUM_MOVES,
  CHESS_GAME_MY_CHECKS,
  CHESS_GAME_OPPONENT_CHECKS,
  CHESS_GAME_RESULT,
  CHESS_GAME_MATE,
  CHESS_GAME_TERMINATION,
  CHESS_GAME_WHITE_ELO,
  CHESS_GAME_BLACK_ELO,
  CHESS_GAME_WHITE_RATING_DIFF,
  CHESS_GAME_BLACK_RATING_DIFF,
  NUM_CHESS_GAME_ITEMS
};

#define MAX_FILENAME_LEN 1024
static char filename[MAX_FILENAME_LEN];

#define MAX_LINE_LEN 8192
static char line[MAX_LINE_LEN];

static char usage[] =
"usage: fgen_chess_inserts (-debug) (-no_elo) player_name table_name filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static char chess_game_event_str[] = "[Event \"";
#define CHESS_GAME_EVENT_STR_LEN (sizeof (chess_game_event_str) - 1)

static char game_site_str[] = "[Site \"";
#define CHESS_GAME_SITE_STR_LEN (sizeof (game_site_str) - 1)

static char game_date_str[] = "[Date \"";
#define CHESS_GAME_DATE_STR_LEN (sizeof (game_date_str) - 1)

static char *color_str[] = {
  "[White \"",
  "[Black \""
};

static char result_str[] = "[Result \"";
#define RESULT_STR_LEN (sizeof (result_str) - 1)

static char mate[] = "#";
#define MATE_LEN (sizeof (mate) - 1)

static char *elo_str[] = {
  "[WhiteElo \"",
  "[BlackElo \""
};

static char *rating_diff_str[] = {
  "[WhiteRatingDiff \"",
  "[BlackRatingDiff \""
};

static char time_control_str[] = "[TimeControl \"";
#define TIME_CONTROL_STR_LEN (sizeof (time_control_str) - 1)

static char eco_str[] = "[ECO \"";
#define ECO_STR_LEN (sizeof (eco_str) - 1)

static char opening_str[] = "[Opening \"";
#define OPENING_STR_LEN (sizeof (opening_str) - 1)

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

#define CHESS_GAME_SITE_LEN 28
static char game_site[CHESS_GAME_SITE_LEN+1];

#define CHESS_GAME_DATE_LEN 10
static char game_date[CHESS_GAME_DATE_LEN+1];

#define COLOR_LEN 1
static char color[COLOR_LEN+1];

#define OPPONENT_NAME_MAX_LEN 25
static char opponent_name[OPPONENT_NAME_MAX_LEN+1];

#define RESULT_MAX_LEN 10
static char result[RESULT_MAX_LEN+1];

#define MY_RESULT_LEN 1
static char my_result[MY_RESULT_LEN+1];

#define MATE_LEN 1
static char my_mate[MATE_LEN+1];

#define ELO_MAX_LEN 10
static char elo[2][ELO_MAX_LEN+1];

#define RATING_DIFF_MAX_LEN 10
static char rating_diff[2][RATING_DIFF_MAX_LEN+1];

#define TIME_CONTROL_MAX_LEN 10
static char time_control[TIME_CONTROL_MAX_LEN+1];

#define ECO_MAX_LEN 4
static char eco[ECO_MAX_LEN+1];

#define OPENING_MAX_LEN 150
static char opening[OPENING_MAX_LEN+1];

#define TERMINATION_MAX_LEN 50
static char termination[TERMINATION_MAX_LEN+1];

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
static int Contains(bool bCaseSens,char *line,int line_len,
  char *string,int string_len,int *index);
static int get_game_site(char *line,int line_len,int ix,
  char *game_site);
static int get_game_date(char *line,int line_len,int ix,
  char *game_date);
static int process_color_str(
  char *player_name,
  int player_name_len,
  int color,
  char *line,int line_len,int ix,
  char *opponent_name,int opponent_name_max_len,
  char *color_ptr);
static int get_result(char *line,int line_len,int ix,
  char *result,int result_max_len,char *color,char *my_result);
static int get_elo(char *line,int line_len,int ix,
  char *elo,int elo_max_len);
static int get_rating_diff(char *line,int line_len,int ix,
  char *rating_diff,int rating_diff_max_len);
static int get_time_control(char *line,int line_len,int ix,
  char *time_control,int time_control_max_len);
static int get_eco(char *line,int line_len,int ix,
  char *eco,int eco_max_len);
static int get_opening(char *line,int line_len,int ix,
  char *opening,int opening_max_len);
static int get_termination(char *line,int line_len,int ix,
  char *termination,int termination_max_len);
static int get_num_moves(char *line,int line_len,int *num_moves_ptr);
static void output_game_insert_statement(
  char *filename,
  char *table_name,
  bool *bHaveItem,
  char *game_site,
  char *game_date,
  char *time_control,
  char *eco,
  char *opening,
  char *opponent_name,
  char *color,
  int num_moves,
  int my_checks,
  int opponent_checks,
  char *my_result,
  char *my_mate,
  char *termination,
  bool bNoElo
);
static void count_checks(char *line,int line_len,int *white_checks_pt,int *black_checks_pt);

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bDebug;
  bool bNoElo;
  char *player_name;
  int player_name_len;
  char *table_name;
  int color_str_len[2];
  int elo_str_len[2];
  int rating_diff_str_len[2];
  FILE *fptr0;
  int filename_len;
  FILE *fptr;
  int line_len;
  int line_no;
  int ix;
  int retval;
  int num_moves;
  int white_checks;
  int black_checks;
  int my_checks;
  int opponent_checks;
  bool bHaveItem[NUM_CHESS_GAME_ITEMS];

  if ((argc < 4) || (argc > 6)) {
    printf(usage);
    return 1;
  }

  bDebug = false;
  bNoElo = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = true;
    else if (!strcmp(argv[curr_arg],"-no_elo"))
      bNoElo = true;
    else
      break;
  }

  if (argc - curr_arg != 3) {
    printf(usage);
    return 2;
  }

  player_name = argv[curr_arg];
  player_name_len = strlen(player_name);

  table_name = argv[curr_arg+1];

  if ((fptr0 = fopen(argv[curr_arg+2],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg+2]);
    return 3;
  }

  if (!bDebug)
    printf("use chess\n\n");

  for (n = 0; n < 2; n++) {
    color_str_len[n] = strlen(color_str[n]);
    elo_str_len[n] = strlen(elo_str[n]);
    rating_diff_str_len[n] = strlen(rating_diff_str[n]);
  }

  for ( ; ; ) {
    GetLine(fptr0,filename,&filename_len,MAX_FILENAME_LEN);

    if (feof(fptr0))
      break;

    if (bDebug)
      printf("reading %s\n",filename);

    if ((fptr = fopen(filename,"r")) == NULL) {
      printf(couldnt_open,filename);
      continue;
    }

    line_no = 0;

    for (n = 0; n < NUM_CHESS_GAME_ITEMS; n++)
      bHaveItem[n] = false;

    for ( ; ; ) {
      GetLine(fptr,line,&line_len,MAX_LINE_LEN);

      if (feof(fptr))
        break;

      line_no++;

      if (Contains(true,
        line,line_len,
        chess_game_event_str,CHESS_GAME_EVENT_STR_LEN,
        &ix)) {

        if (bHaveItem[CHESS_GAME_SITE]) {
          if (!bDebug) {
            output_game_insert_statement(
              filename,
              table_name,
              bHaveItem,
              game_site,
              game_date,
              time_control,
              eco,
              opening,
              opponent_name,
              color,
              num_moves,
              my_checks,
              opponent_checks,
              my_result,
              my_mate,
              termination,
              bNoElo
            );
          }
        }

        for (n = 0; n < NUM_CHESS_GAME_ITEMS; n++)
          bHaveItem[n] = false;
      }
      else if (Contains(true,
        line,line_len,
        game_site_str,CHESS_GAME_SITE_STR_LEN,
        &ix)) {

        retval = get_game_site(line,line_len,ix+CHESS_GAME_SITE_STR_LEN,
          game_site);

        if (retval)
          printf("get_game_site() failed on line %d: %d\n",line_no,retval);
        else
          bHaveItem[CHESS_GAME_SITE] = true;
      }
      else if (Contains(true,
        line,line_len,
        game_date_str,CHESS_GAME_DATE_STR_LEN,
        &ix)) {

        retval = get_game_date(line,line_len,ix+CHESS_GAME_DATE_STR_LEN,
          game_date);

        if (retval)
          printf("get_game_date() failed on line %d: %d\n",line_no,retval);
        else
          bHaveItem[CHESS_GAME_DATE] = true;
      }
      else if (Contains(true,
        line,line_len,
        color_str[WHITE],color_str_len[WHITE],
        &ix)) {

        retval = process_color_str(player_name,player_name_len,
          WHITE,line,line_len,ix+color_str_len[WHITE],
          opponent_name,OPPONENT_NAME_MAX_LEN,color);

        if (retval)
          printf("process_color_str() failed on line %d: %d\n",line_no,retval);
      }
      else if (Contains(true,
        line,line_len,
        color_str[BLACK],color_str_len[BLACK],
        &ix)) {

        retval = process_color_str(player_name,player_name_len,
          BLACK,line,line_len,ix+color_str_len[BLACK],
          opponent_name,OPPONENT_NAME_MAX_LEN,color);

        if (retval)
          printf("process_color_str() failed on line %d: %d\n",line_no,retval);
        else {
          bHaveItem[CHESS_GAME_OPPONENT_NAME] = true;
          bHaveItem[CHESS_GAME_COLOR] = true;

          if (bDebug)
            printf("  %s\n",opponent_name);
        }
      }
      else if (Contains(true,
        line,line_len,
        result_str,RESULT_STR_LEN,
        &ix)) {

        retval = get_result(line,line_len,ix+RESULT_STR_LEN,
          result,RESULT_MAX_LEN,color,my_result);

        if (retval)
          printf("get_result() failed on line %d: %d\n",line_no,retval);
        else
          bHaveItem[CHESS_GAME_RESULT] = true;
      }
      else if (Contains(true,
        line,line_len,
        elo_str[WHITE],elo_str_len[WHITE],
        &ix)) {

        retval = get_elo(line,line_len,ix+elo_str_len[WHITE],
          elo[WHITE],ELO_MAX_LEN);

        if (retval)
          printf("get_elo() failed on line %d: %d\n",line_no,retval);
        else
          bHaveItem[CHESS_GAME_WHITE_ELO] = true;
      }
      else if (Contains(true,
        line,line_len,
        elo_str[BLACK],elo_str_len[BLACK],
        &ix)) {

        retval = get_elo(line,line_len,ix+elo_str_len[BLACK],
          elo[BLACK],ELO_MAX_LEN);

        if (retval)
          printf("get_elo() failed on line %d: %d\n",line_no,retval);
        else
          bHaveItem[CHESS_GAME_BLACK_ELO] = true;
      }
      else if (Contains(true,
        line,line_len,
        rating_diff_str[WHITE],rating_diff_str_len[WHITE],
        &ix)) {

        retval = get_rating_diff(line,line_len,ix+rating_diff_str_len[WHITE],
          rating_diff[WHITE],RATING_DIFF_MAX_LEN);

        if (retval)
          printf("get_rating_diff() failed on line %d: %d\n",line_no,retval);
        else
          bHaveItem[CHESS_GAME_WHITE_RATING_DIFF] = true;
      }
      else if (Contains(true,
        line,line_len,
        rating_diff_str[BLACK],rating_diff_str_len[BLACK],
        &ix)) {

        retval = get_rating_diff(line,line_len,ix+rating_diff_str_len[BLACK],
          rating_diff[BLACK],RATING_DIFF_MAX_LEN);

        if (retval)
          printf("get_rating_diff() failed on line %d: %d\n",line_no,retval);
        else
          bHaveItem[CHESS_GAME_BLACK_RATING_DIFF] = true;
      }
      else if (Contains(true,
        line,line_len,
        time_control_str,TIME_CONTROL_STR_LEN,
        &ix)) {

        retval = get_time_control(line,line_len,ix+TIME_CONTROL_STR_LEN,
          time_control,TIME_CONTROL_MAX_LEN);

        if (retval)
          printf("get_time_control() failed on line %d: %d\n",line_no,retval);
        else
          bHaveItem[CHESS_GAME_TIME_CONTROL] = true;
      }
      else if (Contains(true,
        line,line_len,
        eco_str,ECO_STR_LEN,
        &ix)) {

        retval = get_eco(line,line_len,ix+ECO_STR_LEN,
          eco,ECO_MAX_LEN);

        if (retval)
          printf("get_eco() failed on line %d: %d\n",line_no,retval);
        else
          bHaveItem[CHESS_GAME_ECO] = true;
      }
      else if (Contains(true,
        line,line_len,
        opening_str,OPENING_STR_LEN,
        &ix)) {

        retval = get_opening(line,line_len,ix+OPENING_STR_LEN,
          opening,OPENING_MAX_LEN);

        if (retval)
          printf("get_opening() failed on line %d: %d\n",line_no,retval);
        else
          bHaveItem[CHESS_GAME_OPENING] = true;
      }
      else if (Contains(true,
        line,line_len,
        termination_str,TERMINATION_STR_LEN,
        &ix)) {

        retval = get_termination(line,line_len,ix+TERMINATION_STR_LEN,
          termination,TERMINATION_MAX_LEN);

        if (retval)
          printf("get_termination() failed on line %d: %d\n",line_no,retval);
        else
          bHaveItem[CHESS_GAME_TERMINATION] = true;
      }
      else if (Contains(true,
        line,line_len,
        first_move_str,FIRST_MOVE_STR_LEN,
        &ix)) {

        retval = get_num_moves(line,line_len,&num_moves);

        if (retval)
          printf("get_num_moves() failed on line %d: %d\n",line_no,retval);
        else {
          bHaveItem[CHESS_GAME_NUM_MOVES] = true;

          if (Contains(true,
            line,line_len,
            mate,MATE_LEN,
            &ix)) {

            my_mate[0] = '1';
          }
          else
            my_mate[0] = '0';

          my_mate[1] = 0;
          bHaveItem[CHESS_GAME_MATE] = true;

          count_checks(line,line_len,&white_checks,&black_checks);

          if (color[0] == 'W') {
            my_checks = white_checks;
            opponent_checks = black_checks;
          }
          else {
            my_checks = black_checks;
            opponent_checks = white_checks;
          }

          bHaveItem[CHESS_GAME_MY_CHECKS] = true;
          bHaveItem[CHESS_GAME_OPPONENT_CHECKS] = true;
        }
      }
    }

    fclose(fptr);

    if (!bDebug) {
      output_game_insert_statement(
        filename,
        table_name,
        bHaveItem,
        game_site,
        game_date,
        time_control,
        eco,
        opening,
        opponent_name,
        color,
        num_moves,
        my_checks,
        opponent_checks,
        my_result,
        my_mate,
        termination,
        bNoElo
      );
    }
  }

  if (!bDebug)
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

static int get_game_site(char *line,int line_len,int ix,
  char *game_site)
{
  int n;

  if (line_len - ix < CHESS_GAME_SITE_LEN + 1)
    return 1;

  for (n = 0; n < CHESS_GAME_SITE_LEN; n++) {
    game_site[n] = line[ix + n];
  }

  game_site[n] = 0;

  return 0;
}

static int get_game_date(char *line,int line_len,int ix,
  char *game_date)
{
  int n;

  if (line_len - ix < CHESS_GAME_DATE_LEN + 1)
    return 1;

  for (n = 0; n < CHESS_GAME_DATE_LEN; n++) {
    if (line[ix + n] == '.')
      game_date[n] = '-';
    else
      game_date[n] = line[ix + n];
  }

  game_date[n] = 0;

  return 0;
}

static int process_color_str(
  char *player_name,
  int player_name_len,
  int color,
  char *line,int line_len,int ix,
  char *opponent_name,int opponent_name_max_len,
  char *color_ptr)
{
  int n;

  if (!strncmp(&line[ix],player_name,player_name_len)) {
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

static int get_rating_diff(char *line,int line_len,int ix,
  char *rating_diff,int rating_diff_max_len)
{
  int n;

  for (n = 0; n < rating_diff_max_len; n++) {
    if (line[ix + n] == '"')
      break;

    rating_diff[n] = line[ix + n];
  }

  if (n == rating_diff_max_len)
    return 1;

  rating_diff[n] = 0;

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

static int get_eco(char *line,int line_len,int ix,
  char *eco,int eco_max_len)
{
  int n;

  for (n = 0; n < eco_max_len; n++) {
    if (line[ix + n] == '"')
      break;

    eco[n] = line[ix + n];
  }

  if (n == eco_max_len)
    return 1;

  eco[n] = 0;

  return 0;
}

static int get_opening(char *line,int line_len,int ix,
  char *opening,int opening_max_len)
{
  int m;
  int n;

  for (m = 0,n = 0; n < opening_max_len; n++) {
    if (line[ix + n] == '"')
      break;

    if ((line[ix + n] >= ' ') && (line[ix + n] < 0x7f)) {
      if (m == opening_max_len)
        return 1;

      opening[m++] = line[ix + n];
    }
  }

  opening[m] = 0;

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

static int get_num_moves(char *line,int line_len,int *num_moves_ptr)
{
  int n;
  int last_move_number;

  for (n = line_len - 1; n >= 0; n--) {
    if (line[n] == '.')
      break;
  }

  if (n < 0)
    return 1;

  for ( ; n >= 0; n--) {
    if (line[n] == ' ')
      break;
  }

  sscanf(&line[n+1],"%d",&last_move_number);

  *num_moves_ptr = last_move_number;

  return 0;
}

static void output_game_insert_statement(
  char *filename,
  char *table_name,
  bool *bHaveItem,
  char *game_site,
  char *game_date,
  char *time_control,
  char *eco,
  char *opening,
  char *opponent_name,
  char *color,
  int num_moves,
  int my_checks,
  int opponent_checks,
  char *my_result,
  char *my_mate,
  char *termination,
  bool bNoElo
)
{
  int n;

  if (!bNoElo) {
    for (n = 0; n < NUM_CHESS_GAME_ITEMS; n++) {
      if (!bHaveItem[n])
        break;
    }
  }
  else {
    for (n = 0; n < NUM_CHESS_GAME_ITEMS - 4; n++) {
      if (!bHaveItem[n])
        break;
    }
  }

  if ((!bNoElo && (n < NUM_CHESS_GAME_ITEMS)) || (bNoElo && (n < NUM_CHESS_GAME_ITEMS - 4))) {
    printf("%s: missing information: %d\n",
      filename,n);

    if (bHaveItem[CHESS_GAME_OPPONENT_NAME])
      printf("  %s\n",opponent_name);
  }
  else {
    if (!bNoElo) {
      printf("insert into %s(\n",table_name);
      printf("  game_filename,\n");
      printf("  game_site,\n");
      printf("  game_date,\n");
      printf("  time_control,\n");
      printf("  eco,\n");
      printf("  opening,\n");
      printf("  opponent_name,\n");
      printf("  color,\n");
      printf("  num_moves,\n");
      printf("  my_checks,\n");
      printf("  opponent_checks,\n");
      printf("  result,\n");
      printf("  mate,\n");
      printf("  result_detail,\n");
      printf("  my_elo_before,\n");
      printf("  my_elo_delta,\n");
      printf("  opponent_elo_before,\n");
      printf("  opponent_elo_delta\n");
      printf(") values (\n");
      printf("  '%s',\n",filename);
      printf("  '%s',\n",game_site);
      printf("  '%s',\n",game_date);
      printf("  '%s',\n",time_control);
      printf("  '%s',\n",eco);
      printf("  \"%s\",\n",opening);
      printf("  '%s',\n",opponent_name);
      printf("  '%s',\n",color);
      printf("  %d,\n",num_moves);
      printf("  %d,\n",my_checks);
      printf("  %d,\n",opponent_checks);
      printf("  '%s',\n",my_result);
      printf("  '%s',\n",my_mate);
      printf("  '%s',\n",termination);
      printf("  %s,\n",
        ((color[0] == 'W') ? elo[WHITE] : elo[BLACK]));
      printf("  %s,\n",
        ((color[0] == 'W') ? rating_diff[WHITE] : rating_diff[BLACK]));
      printf("  %s,\n",
        ((color[0] == 'W') ? elo[BLACK] : elo[WHITE]));
      printf("  %s\n",
        ((color[0] == 'W') ? rating_diff[BLACK] : rating_diff[WHITE]));
      printf(");\n");
    }
    else {
      printf("insert into %s(\n",table_name);
      printf("  game_filename,\n");
      printf("  game_site,\n");
      printf("  game_date,\n");
      printf("  time_control,\n");
      printf("  eco,\n");
      printf("  opening,\n");
      printf("  opponent_name,\n");
      printf("  color,\n");
      printf("  num_moves,\n");
      printf("  my_checks,\n");
      printf("  opponent_checks,\n");
      printf("  result,\n");
      printf("  mate,\n");
      printf("  result_detail\n");
      printf(") values (\n");
      printf("  '%s',\n",filename);
      printf("  '%s',\n",game_site);
      printf("  '%s',\n",game_date);
      printf("  '%s',\n",time_control);
      printf("  '%s',\n",eco);
      printf("  \"%s\",\n",opening);
      printf("  '%s',\n",opponent_name);
      printf("  '%s',\n",color);
      printf("  %d,\n",num_moves);
      printf("  %d,\n",my_checks);
      printf("  %d,\n",opponent_checks);
      printf("  '%s',\n",my_result);
      printf("  '%s',\n",my_mate);
      printf("  '%s'\n",termination);
      printf(");\n");
    }
  }
}

static void count_checks(char *line,int line_len,int *white_checks_pt,int *black_checks_pt)
{
  int n;
  int space_count;
  int white_checks;
  int black_checks;

  space_count = 0;
  white_checks = 0;
  black_checks = 0;

  for (n = 0; n < line_len; n++) {
    if (line[n] == '.')
      space_count = 0;
    else if (line[n] == ' ')
      space_count++;

    if ((line[n] == '+') || (line[n] == '#')) {
      if (space_count == 1)
        white_checks++;
      else
        black_checks++;
    }
  }

  *white_checks_pt = white_checks;
  *black_checks_pt = black_checks;
}
