#include <stdio.h>
#include <string.h>

#define WHITE 0
#define BLACK 1

#define BLITZ_GAME_OPPONENT_NAME        0
#define NUM_BLITZ_GAME_ITEMS            1

#define MAX_FILENAME_LEN 1024
static char filename[MAX_FILENAME_LEN];

#define MAX_LINE_LEN 8192
static char line[MAX_LINE_LEN];

static char usage[] = "usage: fgbu filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static char *color_str[] = {
  "[White \"",
  "[Black \""
};

#define COLOR_LEN 1
static char color[COLOR_LEN+1];

#define OPPONENT_NAME_MAX_LEN 25
static char opponent_name[OPPONENT_NAME_MAX_LEN+1];

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
static int Contains(bool bCaseSens,char *line,int line_len,
  char *string,int string_len,int *index);
static int process_color_str(
  int color,
  char *line,int line_len,int ix,
  char *opponent_name,int opponent_name_max_len,
  char *color_ptr);

int main(int argc,char **argv)
{
  int n;
  int id;
  int color_str_len[2];
  FILE *fptr0;
  int filename_len;
  FILE *fptr;
  int line_len;
  int line_no;
  int ix;
  int retval;
  int num_half_moves;
  bool bHaveItem[NUM_BLITZ_GAME_ITEMS];

  if (argc != 2) {
    printf(usage);
    return 1;
  }

  if ((fptr0 = fopen(argv[1],"r")) == NULL) {
    printf(couldnt_open,argv[1]);
    return 2;
  }

  for (n = 0; n < 2; n++)
    color_str_len[n] = strlen(color_str[n]);

  id = 0;

  for ( ; ; ) {
    GetLine(fptr0,filename,&filename_len,MAX_FILENAME_LEN);

    if (feof(fptr0))
      break;

    if ((fptr = fopen(filename,"r")) == NULL) {
      printf(couldnt_open,filename);
      continue;
    }

    line_no = 0;

    for (n = 0; n < NUM_BLITZ_GAME_ITEMS; n++)
      bHaveItem[n] = false;

    for ( ; ; ) {
      GetLine(fptr,line,&line_len,MAX_LINE_LEN);

      if (feof(fptr))
        break;

      line_no++;

      if (Contains(true,
        line,line_len,
        color_str[WHITE],color_str_len[WHITE],
        &ix)) {

        retval = process_color_str(WHITE,line,line_len,ix+color_str_len[WHITE],
          opponent_name,OPPONENT_NAME_MAX_LEN,color);

        if (retval) {
          printf("process_color_str() failed on line %d: %d\n",line_no,retval);
          fclose(fptr);
          continue;
        }
      }
      else if (Contains(true,
        line,line_len,
        color_str[BLACK],color_str_len[BLACK],
        &ix)) {

        retval = process_color_str(BLACK,line,line_len,ix+color_str_len[BLACK],
          opponent_name,OPPONENT_NAME_MAX_LEN,color);

        if (retval) {
          printf("process_color_str() failed on line %d: %d\n",line_no,retval);
          fclose(fptr);
          continue;
        }

        bHaveItem[BLITZ_GAME_OPPONENT_NAME] = true;
        id++;
      }
    }

    fclose(fptr);

    for (n = 0; n < NUM_BLITZ_GAME_ITEMS; n++) {
      if (!bHaveItem[n])
        break;
    }

    if (n != NUM_BLITZ_GAME_ITEMS)
      printf("  missing information\n");
    else {
      printf("\nupdate blitz_games\n");
      printf("set opponent_elo_before =\n");
      printf("where opponent_name = '%s' and id = %d;\n",
        opponent_name,id);
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

static int process_color_str(
  int color,
  char *line,int line_len,int ix,
  char *opponent_name,int opponent_name_max_len,
  char *color_ptr)
{
  int n;

  if (!strncmp(&line[ix],"neostreet",9)) {
    if (color == WHITE)
      color_ptr[0] = 'W';
    else
      color_ptr[0] = 'B';

    color_ptr[1] = 0;
  }
  else {
    for (n = 0; n < opponent_name_max_len; n++) {
      if (line[ix + n] == '"')
        break;

      opponent_name[n] = line[ix + n];
    }

    if (n == opponent_name_max_len)
      return 1;

    opponent_name[n] = 0;
  }

  return 0;
}
