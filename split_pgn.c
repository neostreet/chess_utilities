#include <stdio.h>
#include <string.h>

#define MAX_LINE_LEN 8192
static char line[MAX_LINE_LEN];
static char filename[MAX_LINE_LEN];

static char usage[] = "usage: split_pgn (-skip_casual) filename variant\n";
static char couldnt_open[] = "couldn't open %s\n";

static char game_list_fmt[] = "%s_games.lst";
#define MAX_LIST_FILE_LEN 64
static char game_list_file[MAX_LIST_FILE_LEN+1];
static char game_filename_fmt[] = "%s_game%05d.txt";

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bSkipCasual;
  FILE *fptr[3];
  int game_count;
  int line_len;
  int line_no;
  int game;
  char buf[20];

  if ((argc < 3) || (argc > 4)) {
    printf(usage);
    return 1;
  }

  bSkipCasual = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-skip_casual"))
      bSkipCasual = true;
    else
      break;
  }

  if (argc - curr_arg != 2) {
    printf(usage);
    return 2;
  }

  if ((fptr[0] = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    return 3;
  }

  sprintf(game_list_file,game_list_fmt,argv[curr_arg+1]);

  if ((fptr[1] = fopen(game_list_file,"w")) == NULL) {
    printf(couldnt_open,game_list_file);
    return 4;
  }

  game_count = 0;

  for ( ; ; ) {
    GetLine(fptr[0],line,&line_len,MAX_LINE_LEN);

    if (feof(fptr[0]))
      break;

    if (!strncmp(line,"[Event ",7) && (!bSkipCasual || strncmp(&line[8],"Casual ",7)))
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
      if (fptr[2]) {
        fclose(fptr[2]);
        fptr[2] = NULL;
      }

      if (bSkipCasual && !strncmp(&line[8],"Casual ",7))
        continue;

      sprintf(buf,game_filename_fmt,argv[curr_arg+1],game_count - game);
      fprintf(fptr[curr_arg],"%s\n",buf);

      game++;

      if ((fptr[2] = fopen(buf,"w")) == NULL) {
        printf(couldnt_open,buf);
        return 5;
      }

      fprintf(fptr[2],"%s\n",line);
    }
    else if (fptr[2])
      fprintf(fptr[2],"%s\n",line);
  }

  if (fptr[2])
    fclose(fptr[2]);

  fclose(fptr[curr_arg]);
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
