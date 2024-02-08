#include <stdio.h>
#include <string.h>

#define MAX_FILENAME_LEN 256
static char filename[MAX_FILENAME_LEN];

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];
static char date[MAX_LINE_LEN];

static char usage[] =
"usage: fchess_score (-terse) (-verbose) (-runtots) (-colorcolor) (-plus_minus) player_name filename\n";
static char couldnt_open[] = "couldn't open %s\n";

enum {
  WHITE,
  BLACK
};

static char white[] = "White";
#define WHITE_LEN (sizeof (white) - 1)
static char black[] = "Black";
#define BLACK_LEN (sizeof (black) - 1)
static char result[] = "Result";
#define RESULT_LEN (sizeof (result) - 1)
static char white_wins[] = "1-0";
#define WHITE_WINS_LEN (sizeof (white_wins) - 1)
static char black_wins[] = "0-1";
#define BLACK_WINS_LEN (sizeof (black_wins) - 1)
static char draw[] = "1/2-1/2";
#define DRAW_LEN (sizeof (draw) - 1)
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
  bool bVerbose;
  bool bRuntots;
  int color;
  bool bPlusMinus;
  int player_name_ix;
  int player_name_len;
  FILE *fptr0;
  int filename_len;
  FILE *fptr;
  int line_len;
  int line_no;
  bool bPlayerIsWhite;
  int wins;
  int losses;
  int draws;
  int total_games;
  int ix;
  double points;
  double pct;

  if ((argc < 3) || (argc > 8)) {
    printf(usage);
    return 1;
  }

  bTerse = false;
  bVerbose = false;
  bRuntots = false;
  color = -1;
  bPlusMinus = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-terse"))
      bTerse = true;
    else if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else if (!strcmp(argv[curr_arg],"-runtots"))
      bRuntots = true;
    else if (!strncmp(argv[curr_arg],"-color",6)) {
      if (!strcmp(&argv[curr_arg][6],white))
        color = WHITE;
      else if (!strcmp(&argv[curr_arg][6],black))
        color = BLACK;
      else {
        printf("color must be %s or %s\n",white,black);
        return 2;
      }
    }
    else if (!strcmp(argv[curr_arg],"-plus_minus"))
      bPlusMinus = true;
    else
      break;
  }

  if (argc - curr_arg != 2) {
    printf(usage);
    return 3;
  }

  if (bTerse && bVerbose) {
    printf("can't specify both -terse and -verbose\n");
    return 4;
  }

  player_name_ix = curr_arg++;
  player_name_len = strlen(argv[player_name_ix]);

  if ((fptr0 = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    return 5;
  }

  wins = 0;
  losses = 0;
  draws = 0;

  for ( ; ; ) {
    GetLine(fptr0,filename,&filename_len,MAX_FILENAME_LEN);

    if (feof(fptr0))
      break;

    if ((fptr = fopen(filename,"r")) == NULL) {
      printf(couldnt_open,filename);
      continue;
    }

    for ( ; ; ) {
      GetLine(fptr,line,&line_len,MAX_LINE_LEN);

      if (feof(fptr))
        break;

      if (Contains(true,
        line,line_len,
        result,RESULT_LEN,
        &ix)) {

        if (Contains(true,
          line,line_len,
          white_wins,WHITE_WINS_LEN,
          &ix)) {

          if (bPlayerIsWhite) {
            if ((color == -1) || (color == WHITE)) {
              wins++;

              if (bVerbose) {
                if (!bPlusMinus)
                  printf("1.0 %s ",filename);
                else
                  printf("%d %s ",wins - losses,filename);
              }
              else if (!bTerse) {
                if (!bPlusMinus)
                  printf("1.0\n");
                else
                  printf("%d\n",wins - losses);
              }
            }
          }
          else {
            if ((color == -1) || (color == BLACK)) {
              losses++;

              if (bVerbose) {
                if (!bPlusMinus)
                  printf("0.0 %s ",filename);
                else
                  printf("%d %s ",wins - losses,filename);
              }
              else if (!bTerse) {
                if (!bPlusMinus)
                  printf("0.0\n");
                else
                  printf("%d\n",wins - losses);
              }
            }
          }
        }
        else if (Contains(true,
          line,line_len,
          black_wins,BLACK_WINS_LEN,
          &ix)) {

          if (!bPlayerIsWhite) {
            if ((color == -1) || (color == BLACK)) {
              wins++;

              if (bVerbose) {
                if (!bPlusMinus)
                  printf("1.0 %s ",filename);
                else
                  printf("%d %s ",wins - losses,filename);
              }
              else if (!bTerse) {
                if (!bPlusMinus)
                  printf("1.0\n");
                else
                  printf("%d\n",wins - losses);
              }
            }
          }
          else {
            if ((color == -1) || (color == WHITE)) {
              losses++;

              if (bVerbose) {
                if (!bPlusMinus)
                  printf("0.0 %s ",filename);
                else
                  printf("%d %s ",wins - losses,filename);
              }
              else if (!bTerse) {
                if (!bPlusMinus)
                  printf("0.0\n");
                else
                  printf("%d\n",wins - losses);
              }
            }
          }
        }
        else if (Contains(true,
          line,line_len,
          draw,DRAW_LEN,
          &ix)) {

          if ((color == -1) || ((color == WHITE) && bPlayerIsWhite) || ((color == BLACK) && !bPlayerIsWhite)) {
            draws++;

            if (bVerbose) {
              if (!bPlusMinus)
                printf("0.5 %s ",filename);
              else
                printf("%d %s ",wins - losses,filename);
            }
            else if (!bTerse) {
              if (!bPlusMinus)
                printf("0.5\n");
              else
                printf("%d\n",wins - losses);
            }
          }
        }
        else {
          printf("%s: couldn't determine result\n",filename);
          return 6;
        }
      }
      else if (Contains(true,
        line,line_len,
        utcdate,UTCDATE_LEN,
        &ix)) {

        if (bVerbose || bRuntots) {
          get_date(date,line);

          if (bRuntots)
            printf("# %d %d %d %d %s %s\n",wins - losses,wins,draws,losses,filename,date);
          else
            printf("%s\n",date);
        }

        break;
      }
      else if (Contains(true,
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
          return 7;
        }
      }
    }

    fclose(fptr);
  }

  fclose(fptr0);

  total_games = wins + draws + losses;

  if (!bRuntots && !bPlusMinus && (bTerse || bVerbose)) {
    printf("\n%6d wins\n",wins);
    printf("%6d draws\n",draws);
    printf("%6d losses\n",losses);

    points = (double)wins + ((double)(0.5) * (double)draws);
    pct = points / (double)total_games * (double)100;

    printf("\n%6.2lf out of %6.2lf (%5.2lf%%)\n",points,(double)total_games,pct);
  }

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
