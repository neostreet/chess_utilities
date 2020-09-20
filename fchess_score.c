#include <stdio.h>
#include <string.h>

#define MAX_FILENAME_LEN 256
static char filename[MAX_FILENAME_LEN];

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] =
"usage: fchess_score (-verbose) (-runtots) (-colorcolor) player_name filename\n";
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

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bVerbose;
  bool bRuntots;
  int color;
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

  if ((argc < 3) || (argc > 6)) {
    printf(usage);
    return 1;
  }

  bVerbose = false;
  bRuntots = false;
  color = -1;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-verbose"))
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
    else
      break;
  }

  if (argc - curr_arg != 2) {
    printf(usage);
    return 3;
  }

  player_name_ix = curr_arg++;
  player_name_len = strlen(argv[player_name_ix]);

  if ((fptr0 = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    return 4;
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
              if (bVerbose)
                printf("%6.2lf %s\n",(double)1,filename);

              wins++;
            }
          }
          else {
            if ((color == -1) || (color == BLACK)) {
              if (bVerbose)
                printf("%6.2lf %s\n",(double)0,filename);

              losses++;
            }
          }
        }
        else if (Contains(true,
          line,line_len,
          black_wins,BLACK_WINS_LEN,
          &ix)) {

          if (!bPlayerIsWhite) {
            if ((color == -1) || (color == BLACK)) {
              if (bVerbose)
                printf("%6.2lf %s\n",(double)1,filename);

              wins++;
            }
          }
          else {
            if ((color == -1) || (color == WHITE)) {
              if (bVerbose)
                printf("%6.2lf %s\n",(double)0,filename);

              losses++;
            }
          }
        }
        else if (Contains(true,
          line,line_len,
          draw,DRAW_LEN,
          &ix)) {

          if ((color == -1) || ((color == WHITE) && bPlayerIsWhite) || ((color == BLACK) && !bPlayerIsWhite)) {
            if (bVerbose)
              printf("%6.2lf %s\n",(double).5,filename);

            draws++;
          }
        }
        else {
          printf("%s: couldn't determine result\n",filename);
          return 5;
        }
      }
      else if (Contains(true,
        line,line_len,
        utcdate,UTCDATE_LEN,
        &ix)) {

        if (bRuntots)
          printf("# %d %d %d %d %s %s\n",wins - losses,wins,draws,losses,filename,line);

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
          return 6;
        }
      }
    }

    fclose(fptr);
  }

  fclose(fptr0);

  total_games = wins + losses + draws;

  if (bVerbose)
    putchar(0x0a);

  if (!bRuntots) {
    printf("%6d wins\n",wins);
    printf("%6d draws\n",draws);
    printf("%6d losses\n",losses);

    points = (double)wins + ((double)(0.5) * (double)draws);

    printf("\n%6.2lf out of %6.2lf\n",points,(double)total_games);
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
