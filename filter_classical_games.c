#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FILENAME_LEN 256
static char filename[MAX_FILENAME_LEN];
static char filename2[MAX_FILENAME_LEN];

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] = "usage: filter_classical_games (-verbose) (-rename) filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static char *classical_time_controls[] = {
  "900+15"
};
#define NUM_CLASSICAL_TIME_CONTROLS (sizeof classical_time_controls / sizeof (char *))

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int curr_arg;
  bool bVerbose;
  bool bRename;
  int n;
  FILE *fptr0;
  int filename_len;
  FILE *fptr;
  int linelen;
  int num_classical_files;
  bool bIsClassical;

  if ((argc < 2) || (argc > 4)) {
    printf(usage);
    return 1;
  }

  bVerbose = false;
  bRename = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else if (!strcmp(argv[curr_arg],"-rename"))
      bRename = true;
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

  if (bRename)
    num_classical_files = 0;

  for ( ; ; ) {
    GetLine(fptr0,filename,&filename_len,MAX_FILENAME_LEN);

    if (feof(fptr0))
      break;

    if ((fptr = fopen(filename,"r")) == NULL) {
      printf(couldnt_open,filename);
      continue;
    }

    bIsClassical = false;

    for ( ; ; ) {
      GetLine(fptr,line,&linelen,MAX_LINE_LEN);

      if (feof(fptr))
        break;

      if (!strncmp(line,"[TimeControl ",13)) {
        for (n = 14; n < linelen; n++) {
          if (line[n] == '"') {
            line[n] = 0;
            break;
          }
        }

        if (n < linelen) {
          for (n = 0; n < NUM_CLASSICAL_TIME_CONTROLS; n++) {
            if (!strcmp(&line[14],classical_time_controls[n]))
              break;
          }

          if (n < NUM_CLASSICAL_TIME_CONTROLS) {
            if (!bRename)
              printf("%s\n",filename);
            else {
              num_classical_files++;
              bIsClassical = true;
            }
          }
        }

        break;
      }
    }

    fclose(fptr);

    if (bRename) {
      if (bIsClassical) {
        sprintf(filename2,"classical_game%05d.txt",num_classical_files);
        remove(filename2);
        rename(filename,filename2);
        printf("%s\n",filename2);
      }
      else {
        printf("%s\n",filename);
      }
    }
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
