#include <stdio.h>
#include <string.h>

#define MAX_FILENAME_LEN 1024

static char ch_filename[MAX_FILENAME_LEN];

#define MAX_LINE_LEN 1024

static char line[MAX_LINE_LEN];

static char usage[] =
"usage: build_ch_file filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
static int build_ch_filename(
  char *txt_filename,
  int txt_filename_len,
  char *ch_filename,
  int max_filename_len);

int main(int argc,char **argv)
{
  int n;
  int txt_filename_len;
  int retval;
  FILE *fptr0;
  FILE *fptr1;
  int line_no;
  int line_len;

  if (argc != 2) {
    printf(usage);
    return 1;
  }

  txt_filename_len = strlen(argv[1]);

  retval = build_ch_filename(argv[1],txt_filename_len,ch_filename,MAX_FILENAME_LEN);

  if (retval) {
    printf("build_ch_filename failed on %s: %d\n",argv[1],retval);
    return 2;
  }

  if ((fptr0 = fopen(argv[1],"r")) == NULL) {
    printf(couldnt_open,argv[1]);
    return 3;
  }

  if ((fptr1 = fopen(ch_filename,"w")) == NULL) {
    printf(couldnt_open,ch_filename);
    return 4;
  }

  line_no = 0;

  for ( ; ; ) {
    GetLine(fptr0,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr0))
      break;

    line_no++;

    for (n = 0; n < line_len; n++) {
      if (line[n] == '.')
        break;
    }

    if (n == line_len) {
      printf("no dot found on line %d\n",line_no);
      return 5;
    }

    strcpy(&line[n],".ch");
    fprintf(fptr1,"%s\n",line);
  }

  fclose(fptr0);
  fclose(fptr1);

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
  char *txt_filename,
  int txt_filename_len,
  char *ch_filename,
  int max_filename_len)
{
  int n;

  for (n = 0; n < txt_filename_len; n++) {
    if (txt_filename[n] == '.')
      break;
  }

  if (n == txt_filename_len)
    return 1;

  if (n + 7 > max_filename_len - 1)
    return 2;

  strncpy(ch_filename,txt_filename,n);
  strcpy(&ch_filename[n],".ch.lst");

  return 0;
}
