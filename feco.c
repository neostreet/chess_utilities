#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "chess.h"
#define MAKE_GLOBALS_HERE
#include "chess.glb"
#include "chess.fun"
#include "chess.mac"
#include "str_list.h"

#define MAX_FILENAME_LEN 256
static char filename[MAX_FILENAME_LEN];

static char usage[] =
"usage: feco (-terse) (-i_am_white) (-i_am_black) (-aggregate) filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

struct eco_and_count {
  char *eco;
  int count;
};

static struct eco_and_count *contig_ecos;
static int *ixs;

int elem_compare(const void *elem1,const void *elem2);

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bTerse;
  bool bIAmWhite;
  bool bIAmBlack;
  bool bAggregate;
  int retval;
  FILE *fptr;
  int filename_len;
  struct game curr_game;
  bool bPrintedFilename;
  struct info_list ecos;
  struct info_list_elem *work_elem;
  int ix;
  int bytes_to_malloc;

  if ((argc < 2) || (argc > 6)) {
    printf(usage);
    return 1;
  }

  bTerse = false;
  bIAmWhite = false;
  bIAmBlack = false;
  bAggregate = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-terse"))
      bTerse = true;
    else if (!strcmp(argv[curr_arg],"-i_am_white"))
      bIAmWhite = true;
    else if (!strcmp(argv[curr_arg],"-i_am_black"))
      bIAmBlack = true;
    else if (!strcmp(argv[curr_arg],"-aggregate"))
      bAggregate = true;
    else
      break;
  }

  if (bIAmWhite and bIAmBlack) {
    printf("can't specify both -i_am_white and -i_am_black\n");
    return 2;
  }

  if (argc - curr_arg != 1) {
    printf(usage);
    return 3;
  }

  if ((fptr = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    return 4;
  }

  if (bAggregate)
    ecos.num_elems = 0;

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

    if (bAggregate) {
      if (member_of_info_list(&ecos,curr_game.eco,&ix)) {
        if (get_info_list_elem(&ecos,ix,&work_elem)) {
          work_elem->int1++;
        }
      }
      else
        add_info_list_elem(&ecos,curr_game.eco,1,0,0,0,true);
    }
    else if (!bTerse)
      printf("%s %s\n",curr_game.eco,filename);
    else
      printf("%s\n",curr_game.eco);
  }

  fclose(fptr);

  if (bAggregate) {
    bytes_to_malloc = ecos.num_elems * sizeof(struct eco_and_count);

    if ((contig_ecos = (struct eco_and_count *)malloc(bytes_to_malloc)) == NULL) {
      printf("malloc of %d bytes failed\n",bytes_to_malloc);
      return 5;
    }

    work_elem = ecos.first_elem;

    for (n = 0; n < ecos.num_elems; n++) {
      contig_ecos[n].eco = work_elem->str;
      contig_ecos[n].count = work_elem->int1;
      work_elem = work_elem->next_elem;
    }

    bytes_to_malloc = ecos.num_elems * sizeof(int);

    if ((ixs = (int *)malloc(bytes_to_malloc)) == NULL) {
      printf("malloc of %d bytes failed\n",bytes_to_malloc);
      return 6;
    }

    for (n = 0; n < ecos.num_elems; n++)
      ixs[n] = n;

    qsort(ixs,ecos.num_elems,sizeof (int),elem_compare);

    for (n = 0; n < ecos.num_elems; n++)
      printf("%4d %s\n",contig_ecos[ixs[n]].count,contig_ecos[ixs[n]].eco);

    free_info_list(&ecos);
    free(contig_ecos);
    free(ixs);
  }

  return 0;
}

int elem_compare(const void *elem1,const void *elem2)
{
  int ix1;
  int ix2;

  ix1 = *(int *)elem1;
  ix2 = *(int *)elem2;

  if (contig_ecos[ix2].count > contig_ecos[ix1].count)
    return 1;
  else if (contig_ecos[ix2].count < contig_ecos[ix1].count)
    return -1;
  else
    return 0;
}
