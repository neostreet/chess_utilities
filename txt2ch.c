#include <stdio.h>
#include <string.h>

#define MAX_FILENAME_LEN 1024
static char ch_filename[MAX_FILENAME_LEN];

#define MAX_LINE_LEN 8192
static char line[MAX_LINE_LEN];

static char usage[] = "usage: txt2ch filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
static int build_ch_filename(
  char *pgn_filename,
  int pgn_filename_len,
  char *ch_filename,
  int max_filename_len);
static bool Contains(int bCaseSens,char *line,int line_len,
  char *string,int string_len,int *index);
int split_line(char *line,int line_len,FILE *ch_fptr);
void remove_checks(char *line);

int main(int argc,char **argv)
{
  FILE *fptr;
  FILE *ch_fptr;
  int pgn_filename_len;
  int retval;
  int line_len;
  int line_no;
  int ix;

  if (argc != 2) {
    printf(usage);
    return 1;
  }

  pgn_filename_len = strlen(argv[1]);

  retval = build_ch_filename(argv[1],pgn_filename_len,ch_filename,MAX_FILENAME_LEN);

  if (retval) {
    printf("build_ch_filename failed on %s: %d\n",argv[1],retval);
    return 2;
  }

  if ((fptr = fopen(argv[1],"r")) == NULL) {
    printf(couldnt_open,argv[1]);
    return 3;
  }

  if ((ch_fptr = fopen(ch_filename,"w")) == NULL) {
    printf(couldnt_open,ch_filename);
    return 4;
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
        fprintf(ch_fptr,"0\n\n");
      }
      else
        fprintf(ch_fptr,"1\n\n");
    }
    else if (!strncmp(line,"1. ",3)) {
      retval = split_line(line,line_len,ch_fptr);

      if (retval) {
        printf("split_line failed on line %d\n",line_no);
        return 5;
      }

      break;
    }
  }

  fclose(ch_fptr);
  fclose(fptr);

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

int split_line(char *line,int line_len,FILE *ch_fptr)
{
  int m;
  int n;
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
      else {
        remove_checks(&line[ix]);
        fprintf(ch_fptr,"%s\n",&line[ix]);
        break;
      }
    }

    for (n = ix + n - 1; (n >= 0); n--) {
      if (line[n] == ' ')
        break;
    }

    if (n < 0)
      return 2;

    line[n] = 0;
    remove_checks(&line[ix]);
    fprintf(ch_fptr,"%s\n",&line[ix]);
    ix = n + 1;
  }

  return 0;
}

void remove_checks(char *line)
{
  int m;
  int n;
  int len;
  int removed;

  len = strlen(line);

  m = 0;
  removed = 0;

  for (n = 0; n < len; n++) {
    if ((line[n] != '+') && (line[n] != '#')) {
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
