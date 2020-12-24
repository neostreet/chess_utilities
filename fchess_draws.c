#include <stdio.h>
#include <string.h>

#define MAX_FILENAME_LEN 256
static char filename[MAX_FILENAME_LEN];

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];
static char date[MAX_LINE_LEN];

static char usage[] =
"usage: fchess_draws (-terse) (-is_draw) (-date) filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static char result[] = "Result \"";
#define RESULT_LEN (sizeof (result) - 1)
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
  bool bIsDraw;
  bool bDate;
  FILE *fptr0;
  int filename_len;
  FILE *fptr;
  int line_len;
  int line_no;
  int ix;

  if ((argc < 2) || (argc > 5)) {
    printf(usage);
    return 1;
  }

  bTerse = false;
  bIsDraw = false;
  bDate = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-terse"))
      bTerse = true;
    else if (!strcmp(argv[curr_arg],"-is_draw"))
      bIsDraw = true;
    else if (!strcmp(argv[curr_arg],"-date"))
      bDate = true;
    else
      break;
  }

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  if ((fptr0 = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
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
          get_date(date,line);
      }
      else if (Contains(true,
        line,line_len,
        result,RESULT_LEN,
        &ix)) {

        if (Contains(true,
          line,line_len,
          draw,DRAW_LEN,
          &ix)) {

          if (bIsDraw) {
            if (!bTerse)
              printf("1 %s\n",filename);
            else
              printf("1\n");
          }
          else {
            if (!bDate)
              printf("%s\n",filename);
            else
              printf("%s %s\n",filename,date);
          }
        }
        else if (bIsDraw)
          printf("0 %s\n",filename);
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

static void get_date(char *date,char *line)
{
  strncpy(date,&line[10],10);
  date[4] = '-';
  date[7] = '-';
}
