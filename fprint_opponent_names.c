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
"usage: fprint_opponent_names (-debug) (-terse) (-i_am_white) (-i_am_black) filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

#define MAX_OPPONENT_NAME_LEN 32
static char opponent_name[MAX_OPPONENT_NAME_LEN+1];
static int max_len;

int get_opponent_name(char *title,char *opponent_name,bool bDebug);

int main(int argc,char **argv)
{
  int curr_arg;
  bool bDebug;
  bool bTerse;
  bool bIAmWhite;
  bool bIAmBlack;
  int retval;
  FILE *fptr;
  int filename_len;
  struct game curr_game;

  if ((argc < 2) || (argc > 6)) {
    printf(usage);
    return 1;
  }

  bDebug = false;
  bTerse = false;
  bIAmWhite = false;
  bIAmBlack = false;

  max_len = 0;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = true;
    else if (!strcmp(argv[curr_arg],"-terse"))
      bTerse = true;
    else if (!strcmp(argv[curr_arg],"-i_am_white"))
      bIAmWhite = true;
    else if (!strcmp(argv[curr_arg],"-i_am_black"))
      bIAmBlack = true;
    else
      break;
  }

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  if (bIAmWhite and bIAmBlack) {
    printf("can't specify both -i_am_white and -i_am_black\n");
    return 3;
  }

  if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    return 4;
  }

  for ( ; ; ) {
    GetLine(fptr,filename,&filename_len,MAX_FILENAME_LEN);

    if (feof(fptr))
      break;

    bzero(&curr_game,sizeof (struct game));

    retval = read_game(filename,&curr_game);

    if (retval) {
      printf("read_game of %s failed: %d\n",filename,retval);
      printf("curr_move = %d\n",curr_game.curr_move);

      continue;
    }

    if (bIAmWhite && curr_game.orientation)
      continue;

    if (bIAmBlack && !curr_game.orientation)
      continue;

    retval = get_opponent_name(curr_game.title,opponent_name,bDebug);

    if (retval) {
      printf("get_opponent_name() failed on %s\n",filename);
      continue;
    }

    if (bTerse)
      printf("%s\n",opponent_name);
    else
      printf("%s %s\n",opponent_name,filename);
  }

  fclose(fptr);

  if (bDebug)
    printf("max_len = %d\n",max_len);

  return 0;
}

int get_opponent_name(char *title,char *opponent_name,bool bDebug)
{
  int n;
  int start;

  if (!strncmp(title,"neostreet vs ",13))
    start = 13;
  else
    start = 0;

  for (n = 0; n < MAX_OPPONENT_NAME_LEN; n++) {
    if (title[start + n] == ' ')
      break;

    opponent_name[n] = title[start + n];
  }

  opponent_name[n] = 0;

  if (bDebug) {
    for (n = 0; n < MAX_TITLE_LEN; n++) {
      if (title[start + n] == ' ')
        break;
    }

    if (n > max_len)
      max_len = n;
  }

  return 0;
}
