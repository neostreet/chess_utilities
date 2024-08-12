#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] = "usage: time_controls filename\n";
static char couldnt_open[] = "couldn't open %s\n";

struct tc_count {
  char *tc;
  int count;
};

static struct tc_count tc_counts[] = {
  "300+3", 0,
  "600+5", 0,
  "600+0", 0,
  "180+2", 0,
  "900+10", 0,
  "120+0", 0,
  "300+0", 0,
  "-", 0,
  "180+0", 0,
  "600+3", 0,
  "900+15", 0,
  "120+1", 0
};
#define NUM_TIME_CONTROLS (sizeof tc_counts / sizeof (struct tc_count))

static int ixs[NUM_TIME_CONTROLS];

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
int compare(const void *elem1,const void *elem2);

int main(int argc,char **argv)
{
  int n;
  FILE *fptr;
  int line_len;
  int line_no;

  if (argc != 2) {
    printf(usage);
    return 1;
  }

  if ((fptr = fopen(argv[1],"r")) == NULL) {
    printf(couldnt_open,argv[1]);
    return 2;
  }

  line_no = 0;

  for ( ; ; ) {
    GetLine(fptr,line,&line_len,MAX_LINE_LEN);

    if (feof(fptr))
      break;

    line_no++;

    for (n = 0; n < NUM_TIME_CONTROLS; n++) {
       if (!strcmp(line,tc_counts[n].tc))
         break;
    }

    if (n == NUM_TIME_CONTROLS) {
      printf("unknown time control %s on line %d\n",line,line_no);
      return 3;
    }

    tc_counts[n].count++;
  }

  fclose(fptr);

  for (n = 0; n < NUM_TIME_CONTROLS; n++)
    ixs[n] = n;

  qsort(ixs,NUM_TIME_CONTROLS,sizeof (int),compare);

  for (n = 0; n < NUM_TIME_CONTROLS; n++) {
    printf("%d %s\n",tc_counts[ixs[n]].count,tc_counts[ixs[n]].tc);
  }

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

int compare(const void *elem1,const void *elem2)
{
  int ix1;
  int ix2;

  ix1 = *(int *)elem1;
  ix2 = *(int *)elem2;

  return tc_counts[ix2].count - tc_counts[ix1].count;
}
