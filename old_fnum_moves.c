#include <stdio.h>
#include <string.h>

#define MAX_FILENAME_LEN 256
static char filename[MAX_FILENAME_LEN];

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];
static char prev_line[MAX_LINE_LEN];

static char usage[] = "usage: old_fnum_moves (-num_moves_first) filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int curr_arg;
  bool bNumMovesFirst;
  int n;
  FILE *fptr0;
  int filename_len;
  FILE *fptr;
  int linelen;
  int num_moves;

  if ((argc < 2) || (argc > 3)) {
    printf(usage);
    return 1;
  }

  bNumMovesFirst = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-num_moves_first"))
      bNumMovesFirst = true;
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
      GetLine(fptr,line,&linelen,MAX_LINE_LEN);

      if (feof(fptr))
        break;

      strcpy(prev_line,line);
    }

    fclose(fptr);

    sscanf(prev_line,"%d",&num_moves);

    if (!bNumMovesFirst)
      printf("%s %d\n",filename,num_moves);
    else
      printf("%d %s\n",num_moves,filename);
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
