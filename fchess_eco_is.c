#include <stdio.h>
#include <string.h>

#define MAX_FILENAME_LEN 256
static char filename[MAX_FILENAME_LEN];

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] =
"usage: fchess_eco_is eco filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static char eco_str[] = "[ECO \"";
#define ECO_STR_LEN (sizeof (eco_str) - 1)

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
static int Contains(bool bCaseSens,char *line,int line_len,
  char *string,int string_len,int *index);
static char *get_eco(char *line,int line_len,int ix);

static int dbg_line_no;
static int afl_dbg;

int main(int argc,char **argv)
{
  int m;
  int n;
  char *eco_to_match;
  FILE *fptr0;
  int filename_len;
  FILE *fptr;
  int line_len;
  int line_no;
  int ix;
  char *eco;

  if (argc != 3) {
    printf(usage);
    return 1;
  }

  eco_to_match = argv[1];

  if ((fptr0 = fopen(argv[2],"r")) == NULL) {
    printf(couldnt_open,argv[2]);
    return 2;
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

    for ( ; ; ) {
      GetLine(fptr,line,&line_len,MAX_LINE_LEN);

      if (feof(fptr))
        break;

      line_no++;

      if (line_no == dbg_line_no)
        afl_dbg = 1;

      if (Contains(true,
        line,line_len,
        eco_str,ECO_STR_LEN,
        &ix)) {
          eco = get_eco(line,line_len,ix + ECO_STR_LEN);

        if (!strcmp(eco,eco_to_match))
          printf("%s\n",filename);

        break;
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

static char *get_eco(char *line,int line_len,int ix)
{
  int n;

  for (n = ix; n < line_len; n++) {
    if (line[n] == '"')
      break;
  }

  if (n < line_len)
    line[n] = 0;

  return &line[ix];
}
