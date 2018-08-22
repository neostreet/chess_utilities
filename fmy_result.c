#include <stdio.h>
#include <string.h>

#define MAX_FILENAME_LEN 1024

static char filename[MAX_FILENAME_LEN];

#define MAX_LINE_LEN 8192
static char line[MAX_LINE_LEN];

enum {
  RESULT_WIN,
  RESULT_LOSS,
  RESULT_DRAW
};

static char usage[] =
"usage: fmy_resut [win | loss | draw] filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
static bool Contains(int bCaseSens,char *line,int line_len,
  char *string,int string_len,int *index);

int main(int argc,char **argv)
{
  FILE *fptr0;
  int file_len;
  int file_no;
  FILE *fptr;
  int line_len;
  int line_no;
  int ix;
  bool bWhite;
  int result;

  if (argc != 3) {
    printf(usage);
    return 1;
  }

  if (!strcmp(argv[1],"win"))
    result = RESULT_WIN;
  else if (!strcmp(argv[1],"loss"))
    result = RESULT_LOSS;
  else if (!strcmp(argv[1],"draw"))
    result = RESULT_DRAW;
  else {
    printf(usage);
    return 2;
  }

  if ((fptr0 = fopen(argv[2],"r")) == NULL) {
    printf(couldnt_open,argv[2]);
    return 3;
  }

  file_no = 0;

  for ( ; ; ) {
    GetLine(fptr0,filename,&file_len,MAX_FILENAME_LEN);

    if (feof(fptr0))
      break;

    if ((fptr = fopen(filename,"r")) == NULL) {
      printf(couldnt_open,filename);
      continue;
    }

    line_no = 0;

    for ( ; ; ) {
      GetLine(fptr,line,&line_len,MAX_LINE_LEN);

      if (feof(fptr))
        break;

      line_no++;

      if (Contains(true,
        line,line_len,
        (char *)"neostreet",9,
        &ix)) {

        if (Contains(true,
          line,line_len,
          (char *)"White",5,
          &ix)) {

          bWhite = true;
        }
        else
          bWhite = false;
      }
      else if (Contains(true,
        line,line_len,
        (char *)"[Result ",8,
        &ix)) {

        switch(result) {
          case RESULT_WIN:
            if (bWhite) {
              if (Contains(true,
                line,line_len,
                (char *)"1-0",3,
                &ix)) {

                printf("%s\n",filename);
              }
            }
            else {
              if (Contains(true,
                line,line_len,
                (char *)"0-1",3,
                &ix)) {

                printf("%s\n",filename);
              }
            }

            break;
          case RESULT_LOSS:
            if (bWhite) {
              if (Contains(true,
                line,line_len,
                (char *)"0-1",3,
                &ix)) {

                printf("%s\n",filename);
              }
            }
            else {
              if (Contains(true,
                line,line_len,
                (char *)"1-0",3,
                &ix)) {

                printf("%s\n",filename);
              }
            }

            break;
          case RESULT_DRAW:
            if (Contains(true,
              line,line_len,
              (char *)"1/2-1/2",7,
              &ix)) {

              printf("%s\n",filename);
            }

            break;
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
