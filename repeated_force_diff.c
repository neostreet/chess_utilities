#include <stdio.h>
#include <string.h>

#define MAX_FILENAME_LEN 256
static char filename[MAX_FILENAME_LEN];

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] = "usage: repeated_force_diff force diff repeated filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int n;
  int force;
  int diff;
  int repeated;
  int curr_force;
  int curr_diff;
  int repeat_count;
  FILE *fptr0;
  int filename_len;
  FILE *fptr;
  int line_no;
  int linelen;

  if (argc != 5) {
    printf(usage);
    return 1;
  }

  sscanf(argv[1],"%d",&force);
  sscanf(argv[2],"%d",&diff);
  sscanf(argv[3],"%d",&repeated);

  if ((fptr0 = fopen(argv[4],"r")) == NULL) {
    printf(couldnt_open,argv[4]);
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
    repeat_count = 0;

    for ( ; ; ) {
      GetLine(fptr,line,&linelen,MAX_LINE_LEN);

      if (feof(fptr))
        break;

      line_no++;
      sscanf(line,"%d %d",&curr_force,&curr_diff);

      if ((curr_force == force) && (curr_diff == diff)) {
        repeat_count++;

        if (repeat_count == repeated) {
          printf("%s : %d\n",filename,line_no);
          break;
        }
      }
      else
        repeat_count = 0;
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
