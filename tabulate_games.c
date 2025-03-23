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
"usage: tabulate_games (-binary_format) filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

static int get_result_from_title(char *title);

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bBinaryFormat;
  int retval;
  FILE *fptr;
  int filename_len;
  struct game curr_game;
  int result;
  int tabulation[3];
  int total_games;

  if ((argc < 2) || (argc > 3)) {
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

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    return 3;
  }

  for (n = 0; n < 3; n++)
    tabulation[n] = 0;

  for ( ; ; ) {
    GetLine(fptr,filename,&filename_len,MAX_FILENAME_LEN);

    if (feof(fptr))
      break;

    bzero(&curr_game,sizeof (struct game));

    retval = read_binary_game(filename,&curr_game);

    if (!bBinaryFormat) {
      retval = read_game(filename,&curr_game,err_msg);

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

    result = get_result_from_title(curr_game.title);

    if ((result < 0) || (result > 2)) {
      printf("can't determine result of %s\n",filename);
      return 4;
    }

    tabulation[result]++;
  }

  fclose(fptr);

  total_games = 0;

  for (n = 0; n < 3; n++)
    total_games += tabulation[n];

  printf("%5d wins\n",tabulation[0]);
  printf("%5d draws\n",tabulation[1]);
  printf("%5d losses\n",tabulation[2]);
  printf("\n%5d games\n",total_games);

  return 0;
}

static int get_result_from_title(char *title)
{
  int title_len;

  title_len = strlen(title);

  switch (title[title_len - 1]) {
    case '0':
      if (!strncmp(title,"neostreet",9))
        return 0;
      else
        return 2;

      break;
    case '1':
      if (!strncmp(title,"neostreet",9))
        return 2;
      else
        return 0;

      break;
    case '2':
      return 1;

      break;
  }

  return -1;
}
