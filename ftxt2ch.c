#include <stdio.h>
#include <string.h>

#define MAX_FILENAME_LEN 1024

static char filename[MAX_FILENAME_LEN];
static char ch_filename[MAX_FILENAME_LEN];

#define MAX_LINE_LEN 8192
static char line[MAX_LINE_LEN];

#define MAX_DATE_LEN 10
static char date[MAX_DATE_LEN+1];

#define MAX_OPPONENT_NAME_LEN 50
static char opponent_name[MAX_OPPONENT_NAME_LEN+1];

#define MAX_RESULT_LEN 7
static char result[MAX_RESULT_LEN+1];

#define MAX_ECO_LEN 3
static char eco[MAX_ECO_LEN+1];

static char usage[] = "usage: ftxt2ch (-dont_do_removes) (-skip_mate) player_name filename\n";
static char couldnt_open[] = "couldn't open %s\n";
static char couldnt_determine_color[] = "%s: couldn't determine color\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
static int build_ch_filename(
  char *pgn_filename,
  int pgn_filename_len,
  char *ch_filename,
  int max_filename_len);
static bool Contains(int bCaseSens,char *line,int line_len,
  char *string,int string_len,int *index);
int split_line(char *line,int line_len,FILE *ch_fptr,bool bDontDoRemoves,bool bSkipMate);
void remove_checks_and_promotions(char *line);
void get_date(char *line,int line_len,char *date,int max_date_len);
void get_opponent_name(char *line,int line_len,char *opponent_name,int max_opponent_name_len);
void get_result(char *line,int line_len,char *result,int max_result_len);
void get_eco(char *line,int line_len,char *eco,int max_eco_len);

int main(int argc,char **argv)
{
  int curr_arg;
  bool bDontDoRemoves;
  bool bSkipMate;
  int player_name_ix;
  int player_name_len;
  FILE *fptr0;
  int file_len;
  int file_no;
  FILE *fptr;
  FILE *ch_fptr;
  int color;
  int pgn_filename_len;
  int retval;
  int line_len;
  int line_no;
  int ix;

  if ((argc < 3) || (argc > 5)) {
    printf(usage);
    return 1;
  }

  bDontDoRemoves = false;
  bSkipMate = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-dont_do_removes"))
      bDontDoRemoves = true;
    else if (!strcmp(argv[curr_arg],"-skip_mate"))
      bSkipMate = true;
    else
      break;
  }

  if (argc - curr_arg != 2) {
    printf(usage);
    return 2;
  }

  player_name_ix = curr_arg;
  player_name_len = strlen(argv[player_name_ix]);

  if ((fptr0 = fopen(argv[curr_arg + 1],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg + 1]);
    return 3;
  }

  file_no = 0;

  for ( ; ; ) {
    GetLine(fptr0,filename,&file_len,MAX_FILENAME_LEN);

    if (feof(fptr0))
      break;

    pgn_filename_len = strlen(filename);

    retval = build_ch_filename(filename,pgn_filename_len,ch_filename,MAX_FILENAME_LEN);

    if (retval) {
      printf("build_ch_filename failed on %s: %d\n",filename,retval);
      continue;
    }

    if ((fptr = fopen(filename,"r")) == NULL) {
      printf(couldnt_open,filename);
      continue;
    }

    color = -1;
    line_no = 0;

    for ( ; ; ) {
      GetLine(fptr,line,&line_len,MAX_LINE_LEN);

      if (feof(fptr))
        break;

      line_no++;

      if (Contains(true,
        line,line_len,
        (char *)"[Date ",6,
        &ix)) {

        get_date(line,line_len,date,MAX_DATE_LEN);
      }
      else if (Contains(true,
        line,line_len,
        (char *)"[White ",7,
        &ix)) {

        if (Contains(true,
          line,line_len,
          argv[player_name_ix],player_name_len,
          &ix)) {

          color = 0;
        }
        else {
          color = 1;
          get_opponent_name(line,line_len,opponent_name,MAX_OPPONENT_NAME_LEN);
        }
      }
      else if (Contains(true,
        line,line_len,
        (char *)"[Black ",7,
        &ix)) {

        if (!color)
          get_opponent_name(line,line_len,opponent_name,MAX_OPPONENT_NAME_LEN);
      }
      else if (Contains(true,
        line,line_len,
        (char *)"[Result ",8,
        &ix)) {

        get_result(line,line_len,result,MAX_RESULT_LEN);
      }
      else if (Contains(true,
        line,line_len,
        (char *)"[ECO ",5,
        &ix)) {

        get_eco(line,line_len,eco,MAX_ECO_LEN);
        break;
      }
    }

    if (color == -1) {
      fclose(fptr);
      printf(couldnt_determine_color,ch_filename);
      continue;
    }

    if ((ch_fptr = fopen(ch_filename,"w")) == NULL) {
      printf(couldnt_open,ch_filename);
      continue;
    }

    if (!color) {
      fprintf(ch_fptr,"0\n\n");
      fprintf(ch_fptr,"title%s\\vs\\%s\\\\%s\\\\%s\\\\%s\n\n",argv[player_name_ix],opponent_name,date,eco,result);;
    }
    else {
      fprintf(ch_fptr,"1\n\n");
      fprintf(ch_fptr,"title%s\\vs\\%s\\\\%s\\\\%s\\\\%s\n\n",opponent_name,argv[player_name_ix],date,eco,result);;
    }

    fseek(fptr,0L,SEEK_SET);

    line_no = 0;

    for ( ; ; ) {
      GetLine(fptr,line,&line_len,MAX_LINE_LEN);

      if (feof(fptr))
        break;

      line_no++;

      if ((line_len >= 1) && (line[0] == '['))
        fprintf(ch_fptr,"/ %s\n",line);
      else if (!strncmp(line,"1. ",3)) {
        retval = split_line(line,line_len,ch_fptr,bDontDoRemoves,bSkipMate);

        if (retval) {
          printf("split_line failed on line %d\n",line_no);
          continue;
        }

        break;
      }
    }

    fclose(ch_fptr);
    fclose(fptr);
  }

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

static int build_ch_filename(
  char *pgn_filename,
  int pgn_filename_len,
  char *ch_filename,
  int max_filename_len)
{
  int n;

  for (n = 0; n < pgn_filename_len; n++) {
    if (pgn_filename[n] == '.')
      break;
  }

  if (n == pgn_filename_len)
    return 1;

  if (n + 3 > max_filename_len - 1)
    return 2;

  strncpy(ch_filename,pgn_filename,n);
  strcpy(&ch_filename[n],".ch");

  return 0;
}

static bool Contains(int bCaseSens,char *line,int line_len,
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

int split_line(char *line,int line_len,FILE *ch_fptr,bool bDontDoRemoves,bool bSkipMate)
{
  int m;
  int n;
  int o;
  int ix;

  ix = 0;

  for ( ; ; ) {
    m = 0;

    for (n = 0; (ix + n < line_len); n++) {
      if (line[ix + n] == '.') {
        m++;

        if (m == 2)
          break;
      }
    }

    if (m != 2) {
      if (m != 1)
        return 1;
    }

    for (n = ix + n - 1; (n > ix); n--) {
      if (line[n] == ' ')
        break;
    }

    if (n == ix)
      return 2;

    line[n] = 0;

    if (bSkipMate) {
      o = n - 1;

      if (line[o] == '#') {
        for (o--; (o > ix); o--) {
          if (line[o] == ' ')
            break;
        }

        if (o == ix)
          return 3;

        line[o] = 0;
        fprintf(ch_fptr,"%s\n",&line[ix]);

        return 0;
      }
    }

    if (!bDontDoRemoves)
      remove_checks_and_promotions(&line[ix]);

    fprintf(ch_fptr,"%s\n",&line[ix]);

    if (m == 1)
      break;

    ix = n + 1;
  }

  return 0;
}

void remove_checks_and_promotions(char *line)
{
  int m;
  int n;
  int len;
  int removed;

  len = strlen(line);

  m = 0;
  removed = 0;

  for (n = 0; n < len; n++) {
    if ((line[n] != '+') && (line[n] != '#') && (line[n] != '=')) {
      if (m != n)
        line[m++] = line[n];
      else
        m++;
    }
    else
      removed = 1;
  }

  if (removed)
    line[m] = 0;
}

void get_date(char *line,int line_len,char *date,int max_date_len)
{
  int n;

  for (n = 0; (7 + n < line_len) && (n < max_date_len); n++) {
    if (line[7 + n] == '"')
      break;

    date[n] = line[7 + n];
  }

  date[n] = 0;
}

void get_opponent_name(char *line,int line_len,char *opponent_name,int max_opponent_name_len)
{
  int n;

  for (n = 0; (8 + n < line_len) && (n < max_opponent_name_len); n++) {
    if (line[8 + n] == '"')
      break;

    opponent_name[n] = line[8 + n];
  }

  opponent_name[n] = 0;
}

void get_result(char *line,int line_len,char *result,int max_result_len)
{
  int n;

  for (n = 0; (9 + n < line_len) && (n < max_result_len); n++) {
    if (line[9 + n] == '"')
      break;

    result[n] = line[9 + n];
  }

  result[n] = 0;
}

void get_eco(char *line,int line_len,char *eco,int max_eco_len)
{
  int n;

  for (n = 0; (6 + n < line_len) && (n < max_eco_len); n++) {
    if (line[6 + n] == '"')
      break;

    eco[n] = line[6 + n];
  }

  eco[n] = 0;
}
