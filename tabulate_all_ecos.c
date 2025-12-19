#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "chess.h"
#define MAKE_GLOBALS_HERE
#include "chess.glb"
#include "chess.fun"
#include "chess.mac"

#define MAX_FILENAME_LEN 256
static char filename[MAX_FILENAME_LEN];

struct stats {
  int wins;
  int draws;
  int losses;
  int white;
  int black;
  int total_games;
  double win_pct;
};

static struct stats *eco_stats;
static int *ixs;

static char usage[] =
"usage: tabulate_all_ecos (-i_am_white) (-i_am_black) (-terse) (-no_sort) (-debug)\n"
"  (-min_gamesmin_games) (-not_both_colors) ecos filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

int read_ecos(char *ecos_file,char **ecos_pt,int *num_ecos_pt);
int find_eco(char *eco,char *ecos,int num_ecos);

int elem_compare(const void *elem1,const void *elem2);

static int elem_compare_calls;

static char work_eco[4];

int main(int argc,char **argv)
{
  int m;
  int n;
  int curr_arg;
  bool bIAmWhite;
  bool bIAmBlack;
  bool bTerse;
  bool bNoSort;
  bool bDebug;
  int min_games;
  bool bNotBothColors;
  char *ecos;
  int bytes_to_malloc;
  int num_ecos;
  int eco_ix;
  int retval;
  FILE *fptr;
  int filename_len;
  struct game curr_game;
  char *cpt;

  if ((argc < 3) || (argc > 10)) {
    printf(usage);
    return 1;
  }

  bIAmWhite = false;
  bIAmBlack = false;
  bTerse = false;
  bNoSort = false;
  bDebug = false;
  min_games = 0;
  bNotBothColors = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-i_am_white"))
      bIAmWhite = true;
    else if (!strcmp(argv[curr_arg],"-i_am_black"))
      bIAmBlack = true;
    else if (!strcmp(argv[curr_arg],"-terse"))
      bTerse = true;
    else if (!strcmp(argv[curr_arg],"-no_sort"))
      bNoSort = true;
    else if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = true;
    else if (!strncmp(argv[curr_arg],"-min_games",10))
      sscanf(&argv[curr_arg][10],"%d",&min_games);
    else if (!strcmp(argv[curr_arg],"-not_both_colors"))
      bNotBothColors = true;
    else
      break;
  }

  if (bIAmWhite and bIAmBlack) {
    printf("can't specify both -i_am_white and -i_am_black\n");
    return 2;
  }

  if (argc - curr_arg != 2) {
    printf(usage);
    return 3;
  }

  retval = read_ecos(argv[curr_arg],&ecos,&num_ecos);

  if (retval) {
    printf("read_ecos() failed; retval = %d\n",retval);
    return 4;
  }

  bytes_to_malloc = num_ecos * sizeof(struct stats);

  if ((eco_stats = (struct stats *)malloc(bytes_to_malloc)) == NULL) {
    printf("malloc of %d bytes failedd\n",bytes_to_malloc);
    return 5;
  }

  bytes_to_malloc = num_ecos * sizeof(int);

  if ((ixs = (int *)malloc(bytes_to_malloc)) == NULL) {
    printf("malloc of %d bytes failedd\n",bytes_to_malloc);
    return 6;
  }

  if ((fptr = fopen(argv[curr_arg+1],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg+1]);
    free(ecos);
    return 7;
  }

  for (n = 0; n < num_ecos; n++) {
    eco_stats[n].wins = 0;
    eco_stats[n].draws = 0;
    eco_stats[n].losses = 0;
    eco_stats[n].white = 0;
    eco_stats[n].black = 0;
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

    eco_ix = find_eco(curr_game.eco,ecos,num_ecos);

    if (eco_ix == -1) {
      printf("find_eco() failed for eco %s in %s\n",curr_game.eco,filename);
      continue;
    }

    switch(curr_game.result) {
      case RESULT_WIN:
        eco_stats[eco_ix].wins++;

        break;
      case RESULT_DRAW:
        eco_stats[eco_ix].draws++;

        break;
      case RESULT_LOSS:
        eco_stats[eco_ix].losses++;

        break;
    }

    if (!curr_game.orientation)
      eco_stats[eco_ix].white++;
    else
      eco_stats[eco_ix].black++;
  }

  fclose(fptr);

  for (n = 0; n < num_ecos; n++) {
    eco_stats[n].total_games = eco_stats[n].wins + eco_stats[n].draws + eco_stats[n].losses;
    eco_stats[n].win_pct = chess_win_pct(eco_stats[n].wins,eco_stats[n].draws,eco_stats[n].losses);
    ixs[n] = n;
  }

  if (!bNoSort)
    qsort(ixs,num_ecos,sizeof (int),elem_compare);

  if (bDebug)
    printf("elem_compare() called %d times\n",elem_compare_calls);

  for (n = 0; n < num_ecos; n++) {
    if (!eco_stats[ixs[n]].total_games)
      continue;

    if (eco_stats[ixs[n]].total_games < min_games)
      continue;

    if (bNotBothColors) {
      if (eco_stats[ixs[n]].white && eco_stats[ixs[n]].black)
        continue;
    }

    cpt = &ecos[ixs[n] * 3];

    for (m = 0; m < 3; m++)
      work_eco[m] = cpt[m];

    if (!bTerse) {
      printf("%s\n",work_eco);

      printf("  %5d wins\n",eco_stats[ixs[n]].wins);
      printf("  %5d draws\n",eco_stats[ixs[n]].draws);
      printf("  %5d losses\n",eco_stats[ixs[n]].losses);
      printf("  %5d white\n",eco_stats[ixs[n]].white);
      printf("  %5d black\n",eco_stats[ixs[n]].black);
      printf("  %5d games\n",eco_stats[ixs[n]].total_games);
      printf(" %6.2lf%%\n",eco_stats[ixs[n]].win_pct);
    }
    else {
      printf("%5.2lf%% %s %d wins %d draws %d losses %d white %d black %d games\n",
        eco_stats[ixs[n]].win_pct,work_eco,
        eco_stats[ixs[n]].wins,eco_stats[ixs[n]].draws,eco_stats[ixs[n]].losses,
        eco_stats[ixs[n]].white,eco_stats[ixs[n]].black,eco_stats[ixs[n]].total_games);
    }
  }

  return 0;
}

int read_ecos(char *ecos_file,char **ecos_pt,int *num_ecos_pt)
{
  struct stat statbuf;
  int bytes_to_read;
  int bytes_read;
  char *ecos;
  int fhndl;

  if (stat(ecos_file,&statbuf) == -1)
    return 1;

  bytes_to_read = (int)statbuf.st_size;

  if (bytes_to_read % 3)
    return 2;

  *num_ecos_pt = bytes_to_read / 3;

  if ((ecos = (char *)malloc(bytes_to_read)) == NULL)
    return 3;

  if ((fhndl = open(ecos_file,O_BINARY | O_RDONLY,0)) == -1) {
    free(ecos);
    return 4;
  }

  bytes_read = read(fhndl,ecos,bytes_to_read);

  if (bytes_read != bytes_to_read) {
    free(ecos);
    close(fhndl);
    return 5;
  }

  close(fhndl);

  *ecos_pt = ecos;

  return 0;
}

int find_eco(char *eco,char *ecos,int num_ecos)
{
  int m;
  int n;
  int offset;

  for (n = 0, offset = 0; n < num_ecos; n++, offset += 3) {

    for (m = 0; m < 3; m++) {
      if (eco[m] != ecos[offset + m])
        break;
    }

    if (m == 3)
      return n;
  }

  return -1;
}

int elem_compare(const void *elem1,const void *elem2)
{
  int ix1;
  int ix2;

  elem_compare_calls++;

  ix1 = *(int *)elem1;
  ix2 = *(int *)elem2;

  if (eco_stats[ix2].win_pct > eco_stats[ix1].win_pct)
    return 1;
  else if (eco_stats[ix2].win_pct < eco_stats[ix1].win_pct)
    return -1;
  else
    return 0;
}
