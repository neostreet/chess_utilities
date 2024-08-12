#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];

static char usage[] = "usage: ev_counts filename\n";
static char couldnt_open[] = "couldn't open %s\n";

struct ev_count {
  char *tc;
  int count;
};

static struct ev_count ev_counts[] = {
  "Rated blitz game", 0,
  "Rated rapid game", 0,
  "Agadmator CakeDeFi Christmas Arena", 0,
  "Hourly Blitz Arena", 0,
  "Rated correspondence game", 0,
  "≤1500 Blitz Arena", 0,
  "≤1500 SuperBlitz Arena", 0,
  "Rated classical game", 0,
  "Rated bullet game", 0,
  "Casual blitz game", 0
};
#define NUM_EVENTS (sizeof ev_counts / sizeof (struct ev_count))

static int ixs[NUM_EVENTS];

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

    for (n = 0; n < NUM_EVENTS; n++) {
       if (!strcmp(line,ev_counts[n].tc))
         break;
    }

    if (n == NUM_EVENTS) {
      printf("unknown event %s on line %d\n",line,line_no);
      return 3;
    }

    ev_counts[n].count++;
  }

  fclose(fptr);

  for (n = 0; n < NUM_EVENTS; n++)
    ixs[n] = n;

  qsort(ixs,NUM_EVENTS,sizeof (int),compare);

  for (n = 0; n < NUM_EVENTS; n++) {
    printf("%d %s\n",ev_counts[ixs[n]].count,ev_counts[ixs[n]].tc);
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

  return ev_counts[ix2].count - ev_counts[ix1].count;
}
