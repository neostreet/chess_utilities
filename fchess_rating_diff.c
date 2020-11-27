#include <stdio.h>
#include <string.h>

#define MAX_FILENAME_LEN 256
static char filename[MAX_FILENAME_LEN];

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];
static char date[MAX_LINE_LEN];

static char usage[] = "usage: fchess_rating_diff (-terse) (-date) player_name filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static char white[] = "White";
#define WHITE_LEN (sizeof (white) - 1)
static char black[] = "Black";
#define BLACK_LEN (sizeof (black) - 1)
static char white_rating_diff[] = "WhiteRatingDiff \"";
#define WHITE_RATING_DIFF_LEN (sizeof (white_rating_diff) - 1)
static char black_rating_diff[] = "BlackRatingDiff \"";
#define BLACK_RATING_DIFF_LEN (sizeof (black_rating_diff) - 1)
static char utcdate[] = "UTCDate";
#define UTCDATE_LEN (sizeof (utcdate) - 1)

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
static int Contains(bool bCaseSens,char *line,int line_len,
  char *string,int string_len,int *index);

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bTerse;
  int player_name_ix;
  int player_name_len;
  FILE *fptr0;
  int filename_len;
  FILE *fptr;
  int line_len;
  int line_no;
  bool bPlayerIsWhite;
  bool bDate;
  int ix;
  int rating_diff;

  if ((argc < 3) || (argc > 5)) {
    printf(usage);
    return 1;
  }

  bTerse = false;
  bDate = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-terse"))
      bTerse = true;
    else if (!strcmp(argv[curr_arg],"-date"))
      bDate = true;
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
        utcdate,UTCDATE_LEN,
        &ix)) {

        if (bDate)
          strcpy(date,line);
      }
      else if (Contains(true,
        line,line_len,
        white_rating_diff,WHITE_RATING_DIFF_LEN,
        &ix)) {

        if (bPlayerIsWhite) {
          sscanf(&line[ix + WHITE_RATING_DIFF_LEN],"%d",&rating_diff);

          if (!bTerse) {
            if (!bDate)
              printf("%4d %s\n",rating_diff,filename);
            else
              printf("%4d %s %s\n",rating_diff,filename,date);
          }
          else
            printf("%d\n",rating_diff);

          break;
        }
      }
      else if (Contains(true,
        line,line_len,
        black_rating_diff,BLACK_RATING_DIFF_LEN,
        &ix)) {

        if (!bPlayerIsWhite) {
          sscanf(&line[ix + BLACK_RATING_DIFF_LEN],"%d",&rating_diff);

          if (!bTerse) {
            if (!bDate)
              printf("%4d %s\n",rating_diff,filename);
            else
              printf("%4d %s %s\n",rating_diff,filename,date);
          }
          else
            printf("%d\n",rating_diff);

          break;
        }
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
          return 4;
        }
      }
    }

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
