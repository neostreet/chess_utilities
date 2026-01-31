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
"usage: felos (-terse) (-after) (-ge_eloval) (-le_eloval) filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bTerse;
  bool bAfter;
  bool bGeElo;
  bool bLeElo;
  int ge_elo_val;
  int le_elo_val;
  int retval;
  FILE *fptr;
  int filename_len;
  int elo_after;
  bool bPrint;
  struct game curr_game;

  if ((argc < 2) || (argc > 6)) {
    printf(usage);
    return 1;
  }

  bTerse = false;
  bAfter = false;
  bGeElo = false;
  bLeElo = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-terse"))
      bTerse = true;
    else if (!strcmp(argv[curr_arg],"-after"))
      bAfter = true;
    else if (!strncmp(argv[curr_arg],"-ge_elo",7)) {
      sscanf(&argv[curr_arg][7],"%d",&ge_elo_val);
      bGeElo = true;
    }
    else if (!strncmp(argv[curr_arg],"-le_elo",7)) {
      sscanf(&argv[curr_arg][7],"%d",&le_elo_val);
      bLeElo = true;
    }
    else
      break;
  }

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  if (bGeElo && bLeElo) {
    printf("can't specify both -ge_elo and -le_elo\n");
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

    retval = read_binary_game(filename,&curr_game);

    if (retval) {
      printf("read_binary_game of %s failed: %d\n",filename,retval);
      printf("curr_move = %d\n",curr_game.curr_move);

      continue;
    }

    elo_after = curr_game.my_elo_before + curr_game.my_elo_delta;

    bPrint = true;

    if (bGeElo) {
      if (elo_after < ge_elo_val)
        bPrint = false;
    }
    else if (bLeElo) {
      if (elo_after > le_elo_val)
        bPrint = false;
    }

    if (bPrint) {
      if (!bTerse) {
        printf("%d %d %d %d %s %s\n",
          curr_game.my_elo_before,curr_game.my_elo_delta,
          curr_game.opponent_elo_before,curr_game.opponent_elo_delta,
          filename,curr_game.date);
      }
      else {
        if (!bAfter)
          printf("%d %s %s\n",curr_game.my_elo_before,filename,curr_game.date);
        else
          printf("%d %s %s\n",elo_after,filename,curr_game.date);
      }
    }
  }

  fclose(fptr);

  return 0;
}
