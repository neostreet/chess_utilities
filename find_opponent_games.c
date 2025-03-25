#include <stdio.h>
#include <string.h>

#include "chess.h"
#define MAKE_GLOBALS_HERE
#include "chess.glb"
#include "chess.fun"
#include "chess.mac"

#define MAX_FILENAME_LEN 256
static char filename[MAX_FILENAME_LEN];

static char usage[] =
"usage: find_opponent_games (-binary_format) opponent filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

static int Contains(bool bCaseSens,char *line,int line_len,
  char *string,int string_len,int *index);

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  int opponent_name_len;
  bool bBinaryFormat;
  int retval;
  FILE *fptr;
  int filename_len;
  struct game curr_game;
  int title_len;
  int ix;

  if ((argc < 3) || (argc > 4)) {
    printf(usage);
    return 1;
  }

  bBinaryFormat = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-binary_format"))
      bBinaryFormat = true;
    else
      break;
  }

  if (argc - curr_arg != 2) {
    printf(usage);
    return 2;
  }

  opponent_name_len = strlen(argv[curr_arg]);

  if ((fptr = fopen(argv[curr_arg+1],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg+1]);
    return 3;
  }

  for ( ; ; ) {
    GetLine(fptr,filename,&filename_len,MAX_FILENAME_LEN);

    if (feof(fptr))
      break;

    bzero(&curr_game,sizeof (struct game));

    if (!bBinaryFormat) {
      retval = read_game(filename,&curr_game);

      if (retval) {
        printf("read_game of %s failed: %d\n",filename,retval);
        printf("curr_move = %d\n",curr_game.curr_move);

        continue;
      }
    }
    else {
      retval = read_binary_game(filename,&curr_game);

      if (retval) {
        printf("read_binary_game of %s failed: %d\n",filename,retval);
        printf("curr_move = %d\n",curr_game.curr_move);

        continue;
      }
    }

    title_len = strlen(curr_game.title);

    if (Contains(true,
      curr_game.title,title_len,
      argv[curr_arg],opponent_name_len,
      &ix)) {

      printf("%s\n",filename);
    }
  }

  fclose(fptr);

  return 0;
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
