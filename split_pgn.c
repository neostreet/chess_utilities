#include <stdio.h>
#include <string.h>

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];
static char filename[MAX_LINE_LEN];

static char usage[] = "usage: split_pgn filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static char game_list_file[] = "games.lst";
static char game_filename_fmt[] = "game%d.txt";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int n;
  FILE *fptr[3];
  int game_count;
  int line_len;
  int line_no;
  int game;
  char buf[20];

  if (argc != 2) {
    printf(usage);
    return 1;
  }

  if ((fptr[0] = fopen(argv[1],"r")) == NULL) {
    printf(couldnt_open,argv[1]);
    return 2;
  }

  if ((fptr[1] = fopen(game_list_file,"w")) == NULL) {
    printf(couldnt_open,game_list_file);
    return 3;
  }

  game_count = 0;

  for ( ; ; ) {
    GetLine(fptr[0],line,&line_len,MAX_LINE_LEN);

    if (feof(fptr[0]))
      break;

    if (!strncmp(line,"[Event ",7))
      game_count++;
  }

  fseek(fptr[0],0L,SEEK_SET);

  line_no = 0;
  game = 0;
  fptr[2] = NULL;

  for ( ; ; ) {
    GetLine(fptr[0],line,&line_len,MAX_LINE_LEN);

    if (feof(fptr[0]))
      break;

    line_no++;

    if (!strncmp(line,"[Event ",7)) {
      if (game)
        fclose(fptr[2]);

      sprintf(buf,game_filename_fmt,game_count - game);
      fprintf(fptr[1],"%s\n",buf);

      game++;

      if ((fptr[2] = fopen(buf,"w")) == NULL) {
        printf(couldnt_open,buf);
        return 4;
      }

      fprintf(fptr[2],"%s\n",line);
    }
    else if (game)
      fprintf(fptr[2],"%s\n",line);
  }

  if (game)
    fclose(fptr[2]);

  fclose(fptr[1]);
  fclose(fptr[0]);

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
