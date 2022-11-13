#include <stdio.h>
#include <string.h>

#define MAX_FILENAME_LEN 256
static char filename[MAX_FILENAME_LEN];

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];
static char date[MAX_LINE_LEN];
static char time[MAX_LINE_LEN];

static char usage[] =
"usage: fchess_losses (-terse) (-is_loss) (-is_not_win) (-mate) (-date_time) player_name filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static char white[] = "White";
#define WHITE_LEN (sizeof (white) - 1)
static char black[] = "Black";
#define BLACK_LEN (sizeof (black) - 1)
static char result[] = "Result \"";
#define RESULT_LEN (sizeof (result) - 1)
static char white_wins[] = "1-0";
#define WHITE_WINS_LEN (sizeof (white_wins) - 1)
static char black_wins[] = "0-1";
#define BLACK_WINS_LEN (sizeof (black_wins) - 1)
static char mate[] = "#";
#define MATE_LEN (sizeof (mate) - 1)
static char utcdate[] = "UTCDate";
#define UTCDATE_LEN (sizeof (utcdate) - 1)
static char utctime[] = "UTCTime";
#define UTCTIME_LEN (sizeof (utctime) - 1)

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
static int Contains(bool bCaseSens,char *line,int line_len,
  char *string,int string_len,int *index);
static void get_date(char *date,char *line);
static void get_time(char *time,char *line);

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bTerse;
  bool bIsLoss;
  bool bIsNotWin;
  bool bMate;
  bool bDateTime;
  int player_name_ix;
  int player_name_len;
  FILE *fptr0;
  int filename_len;
  FILE *fptr;
  int line_len;
  int line_no;
  bool bPlayerIsWhite;
  int ix;
  bool bLost;
  bool bHaveMate;
  bool bNotWin;

  if ((argc < 3) || (argc > 8)) {
    printf(usage);
    return 1;
  }

  bTerse = false;
  bIsLoss = false;
  bIsNotWin = false;
  bMate = false;
  bDateTime = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-terse"))
      bTerse = true;
    else if (!strcmp(argv[curr_arg],"-is_loss"))
      bIsLoss = true;
    else if (!strcmp(argv[curr_arg],"-is_not_win"))
      bIsNotWin = true;
    else if (!strcmp(argv[curr_arg],"-mate"))
      bMate = true;
    else if (!strcmp(argv[curr_arg],"-date_time"))
      bDateTime = true;
    else
      break;
  }

  if (bIsLoss && bIsNotWin) {
    printf("can't specify both -is_loss and -is_not_win\n");
    return 2;
  }

  if (argc - curr_arg != 2) {
    printf(usage);
    return 3;
  }

  player_name_ix = curr_arg;
  player_name_len = strlen(argv[player_name_ix]);

  if ((fptr0 = fopen(argv[curr_arg + 1],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg + 1]);
    return 4;
  }

  for ( ; ; ) {
    GetLine(fptr0,filename,&filename_len,MAX_FILENAME_LEN);

    if (feof(fptr0))
      break;

    if ((fptr = fopen(filename,"r")) == NULL) {
      printf(couldnt_open,filename);
      continue;
    }

    bLost = false;
    bHaveMate = false;
    bNotWin = false;

    for ( ; ; ) {
      GetLine(fptr,line,&line_len,MAX_LINE_LEN);

      if (feof(fptr))
        break;

      if (Contains(true,
        line,line_len,
        argv[player_name_ix],player_name_len,
        &ix)) {

        if (Contains(true,
          line,line_len,
          white,WHITE_LEN,
          &ix)) {

          bPlayerIsWhite = true;
        }
        else if (Contains(true,
          line,line_len,
          black,BLACK_LEN,
          &ix)) {

          bPlayerIsWhite = false;
        }
        else {
          printf("%s: couldn't determine whether %s is white or black\n",
            filename,argv[player_name_ix]);
          return 5;
        }
      }
      else if (Contains(true,
        line,line_len,
        result,RESULT_LEN,
        &ix)) {

        if (Contains(true,
          line,line_len,
          white_wins,WHITE_WINS_LEN,
          &ix)) {

          bLost = !bPlayerIsWhite;

          if (bLost)
            bNotWin = true;
        }
        else if (Contains(true,
          line,line_len,
          black_wins,BLACK_WINS_LEN,
          &ix)) {

          bLost = bPlayerIsWhite;

          if (bLost)
            bNotWin = true;
        }
        else
          bNotWin = true;
      }
      else if (Contains(true,
        line,line_len,
        mate,MATE_LEN,
        &ix)) {

        bHaveMate = true;
      }
      else if (Contains(true,
        line,line_len,
        utcdate,UTCDATE_LEN,
        &ix)) {

        if (bDateTime)
          get_date(date,line);
      }
      else if (Contains(true,
        line,line_len,
        utctime,UTCTIME_LEN,
        &ix)) {

        if (bDateTime)
          get_time(time,line);
      }
    }

    fclose(fptr);

    if (!bMate || bHaveMate) {
      if (bIsLoss) {
        if (!bTerse) {
          if (!bDateTime)
            printf("%d %s\n",bLost,filename);
          else
            printf("%d %s %s %s\n",bLost,filename,date,time);
        }
        else {
          if (!bDateTime)
            printf("%d\n",bLost);
          else
            printf("%d %s %s\n",bLost,date,time);
        }
      }
      else if (bIsNotWin) {
        if (!bTerse) {
          if (!bDateTime)
            printf("%d %s\n",bNotWin,filename);
          else
            printf("%d %s %s %s\n",bNotWin,filename,date,time);
        }
        else {
          if (!bDateTime)
            printf("%d\n",bNotWin);
          else
            printf("%d %s %s\n",bNotWin,date,time);
        }
      }
      else if (bLost) {
        if (!bDateTime)
          printf("%s\n",filename);
        else
          printf("%s %s %s\n",filename,date,time);
      }
    }
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

static void get_date(char *date,char *line)
{
  strncpy(date,&line[10],10);
  date[4] = '-';
  date[7] = '-';
}

static void get_time(char *time,char *line)
{
  strncpy(time,&line[10],8);
}
