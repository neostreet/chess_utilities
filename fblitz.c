#include <stdio.h>
#include <string.h>

#define MAX_FILENAME_LEN 256
static char filename[MAX_FILENAME_LEN];

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] = "usage: fblitz (-debug) (-not) (-minsmins) (-incinc) filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static char timecontrol[] = "TimeControl \"";
#define TIMECONTROL_LEN (sizeof (timecontrol) - 1)

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
static int Contains(bool bCaseSens,char *line,int line_len,
  char *string,int string_len,int *index);

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bDebug;
  bool bNot;
  int mins;
  int secs;
  int inc;
  FILE *fptr0;
  int filename_len;
  FILE *fptr;
  int line_len;
  int ix;
  int seconds;
  int increment;
  int match;

  if ((argc < 2) || (argc > 6)) {
    printf(usage);
    return 1;
  }

  bDebug = false;
  bNot = false;
  mins = -1;
  inc = -1;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = true;
    else if (!strcmp(argv[curr_arg],"-not"))
      bNot = true;
    else if (!strncmp(argv[curr_arg],"-mins",5)) {
      sscanf(&argv[curr_arg][5],"%d",&mins);
      secs = mins * 60;
    }
    else if (!strncmp(argv[curr_arg],"-inc",4))
      sscanf(&argv[curr_arg][4],"%d",&inc);
    else
      break;
  }

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  if ((mins == -1) && (inc != -1 )) {
    printf("must specify mins if you specify inc\n");
    return 3;
  }

  if ((fptr0 = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    return 4;
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
        timecontrol,TIMECONTROL_LEN,
        &ix)) {

        for (n = ix + TIMECONTROL_LEN; n < line_len; n++) {
          if (line[n] == '+')
            break;
        }

        if (n < line_len) {
          line[n++] = 0;
          sscanf(&line[ix + TIMECONTROL_LEN],"%d",&seconds);
          sscanf(&line[n],"%d",&increment);

          match = 0;

          if ((mins == -1) && (inc == -1)) {
            if ((seconds == 180) || (seconds == 300))
              match = 1;
          }
          else if (inc != -1) {
            if ((seconds == secs) && (increment == inc))
              match = 1;
          }
          else {
            if (seconds == secs)
              match = 1;
          }

          if (match) {
            if (!bNot)
              printf("%s\n",filename);
          }
          else {
            if (bNot)
              printf("%s\n",filename);
          }
        }

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
