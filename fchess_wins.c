#include <stdio.h>
#include <string.h>

#define MAX_FILENAME_LEN 256
static char filename[MAX_FILENAME_LEN];

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];
static char date[MAX_LINE_LEN];

static char usage[] =
"usage: fchess_wins (-terse) (-is_win) (-is_not_loss) (-mate) (-date) (-date_first) player_name filename\n";
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

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
static int Contains(bool bCaseSens,char *line,int line_len,
  char *string,int string_len,int *index);
static void get_date(char *date,char *line);

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bTerse;
  bool bIsWin;
  bool bIsNotLoss;
  bool bMate;
  bool bDate;
  bool bDateFirst;
  int player_name_ix;
  int player_name_len;
  FILE *fptr0;
  int filename_len;
  FILE *fptr;
  int line_len;
  int line_no;
  bool bPlayerIsWhite;
  int ix;
  bool bWon;
  bool bHaveMate;
  bool bNotLoss;

  if ((argc < 3) || (argc > 9)) {
    printf(usage);
    return 1;
  }

  bTerse = false;
  bIsWin = false;
  bIsNotLoss = false;
  bMate = false;
  bDate = false;
  bDateFirst = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-terse"))
      bTerse = true;
    else if (!strcmp(argv[curr_arg],"-is_win"))
      bIsWin = true;
    else if (!strcmp(argv[curr_arg],"-is_not_loss"))
      bIsNotLoss = true;
    else if (!strcmp(argv[curr_arg],"-mate"))
      bMate = true;
    else if (!strcmp(argv[curr_arg],"-date"))
      bDate = true;
    else if (!strcmp(argv[curr_arg],"-date_first"))
      bDateFirst = true;
    else
      break;
  }

  if (bIsWin && bIsNotLoss) {
    printf("can't specify both -is_win and -is_not_loss\n");
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

    bWon = false;
    bHaveMate = false;
    bNotLoss = false;

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

          bWon = bPlayerIsWhite;

          if (bWon)
            bNotLoss = true;
        }
        else if (Contains(true,
          line,line_len,
          black_wins,BLACK_WINS_LEN,
          &ix)) {

          bWon = !bPlayerIsWhite;

          if (bWon)
            bNotLoss = true;
        }
        else
          bNotLoss = true;
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

        if (bDate)
          get_date(date,line);
      }
    }

    fclose(fptr);

    if (!bMate || bHaveMate) {
      if (bIsWin) {
        if (!bTerse) {
          if (!bDate)
            printf("%d %s\n",bWon,filename);
          else if (!bDateFirst)
            printf("%d %s %s\n",bWon,filename,date);
          else
            printf("%s\t%d\t%s\n",date,bWon,filename);
        }
        else {
          if (!bDate)
            printf("%d\n",bWon);
          else
            printf("%d %s\n",bWon,date);
        }
      }
      else if (bIsNotLoss) {
        if (!bTerse) {
          if (!bDate)
            printf("%d %s\n",bNotLoss,filename);
          else if (!bDateFirst)
            printf("%d %s %s\n",bNotLoss,filename,date);
          else
            printf("%s\t%d\t%s\n",date,bNotLoss,filename);
        }
        else {
          if (!bDate)
            printf("%d\n",bNotLoss);
          else if (!bDateFirst)
            printf("%d %s\n",bNotLoss,date);
          else
            printf("%s\t%d\n",date,bNotLoss);
        }
      }
      else if (bWon) {
        if (!bDate)
          printf("%s\n",filename);
        else if (!bDateFirst)
          printf("%s %s\n",filename,date);
        else
          printf("%s\t%s\n",date,filename);
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
