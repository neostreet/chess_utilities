#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef WIN32
#include <io.h>
#else
#include <unistd.h>
#ifdef FREEBSD
#define O_BINARY 0
#endif
#endif

#include "chess.h"
#define MAKE_GLOBALS_HERE
#include "chess.glb"
#include "chess.fun"
#include "chess.mac"

struct game_info {
  char eco[MAX_ECO_LEN+1];
  int num_moves;
  int orientation;
  int result;
  int processed;
};

static char usage[] =
"usage: find_doppelgames (-verbose) (-debug) filename\n";

char couldnt_get_status[] = "couldn't get status of %s\n";
char couldnt_open[] = "couldn't open %s\n";

int read_list_file(char *filename,char **buf_ptr,int *num_lines_ptr,int **offsets_ptr);
bool doppelgames(struct game *gamept1,struct game *gamept2);

int main(int argc,char **argv)
{
  int m;
  int n;
  int curr_arg;
  bool bVerbose;
  bool bDebug;
  char *buf;
  int num_lines;
  int *offsets;
  int retval;
  int malloc_size;
  struct game_info *info;
  struct game game1;
  struct game game2;
  bool bReadGame1;
  bool bPrinted;

  if ((argc < 2) || (argc > 4)) {
    printf(usage);
    return 1;
  }

  bVerbose = false;
  bDebug = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else if (!strcmp(argv[curr_arg],"-debug"))
      bDebug = true;
    else
      break;
  }

  if (argc - curr_arg != 1) {
    printf(usage);
    return 2;
  }

  retval = read_list_file(argv[curr_arg],&buf,&num_lines,&offsets);

  if (retval) {
    printf("read_list_file() failed: %d\n",retval);
    return 3;
  }

  malloc_size = sizeof (struct game_info) * num_lines;

  if ((info = (struct game_info *)malloc(malloc_size)) == NULL) {
    printf("malloc of %d bytes failed\n",malloc_size);
    return 4;
  }

  for (n = 0; n < num_lines; n++) {
    bzero(&game1,sizeof (struct game));

    retval = read_game(&buf[offsets[n]],&game1);

    if (retval) {
      printf("read_game of %s failed: %d\n",&buf[offsets[n]],retval);

      return 5;
    }

    strcpy(info[n].eco,game1.eco);
    info[n].num_moves = game1.num_moves;
    info[n].orientation = game1.orientation;
    info[n].result = game1.result;
    info[n].processed = 0;
  }

  for (n = 0; n < num_lines - 1; n++) {
    if (!info[n].processed) {
      bReadGame1 = false;
      bPrinted = false;

      for (m = n + 1; m < num_lines; m++) {
        if (!info[m].processed) {
          if (strcmp(info[n].eco,info[m].eco))
            continue;

          if (info[n].num_moves != info[m].num_moves)
            continue;

          if (info[n].orientation != info[m].orientation)
            continue;

          if (info[n].result != info[m].result)
            continue;

          if (!bReadGame1) {
            bzero(&game1,sizeof (struct game));

            retval = read_game(&buf[offsets[n]],&game1);

            if (retval) {
              printf("read_game of %s failed: %d\n",&buf[offsets[n]],retval);

              return 6;
            }

            bReadGame1 = true;
          }

          bzero(&game2,sizeof (struct game));

          retval = read_game(&buf[offsets[m]],&game2);

          if (retval) {
            printf("read_game of %s failed: %d\n",&buf[offsets[m]],retval);

            return 7;
          }

          if (!doppelgames(&game1,&game2))
            continue;

          info[m].processed = true;

          if (!bPrinted) {
            printf("%s %s %d %d %d\n",&buf[offsets[n]],info[n].eco,info[n].num_moves,info[n].orientation,info[n].result);
            bPrinted = true;
          }

          printf("  %s %s %d %d %d\n",&buf[offsets[m]],info[m].eco,info[m].num_moves,info[m].orientation,info[m].result);
        }
      }
    }
  }

  free(info);
  free(offsets);
  free(buf);

  return 0;
}

int read_list_file(char *filename,char **buf_ptr,int *num_lines_ptr,int **offsets_ptr)
{
  int m;
  int n;
  struct stat statbuf;
  int fhndl;
  int bytes_to_io;
  char *buf;
  int bytes_read;
  int num_lines;
  int malloc_size;
  int *offsets;

  if (stat(filename,&statbuf) == -1) {
    printf(couldnt_get_status,filename);
    return 1;
  }

  bytes_to_io = (int)statbuf.st_size;

  if ((buf = (char *)malloc(bytes_to_io)) == NULL) {
    printf("malloc of %d bytes failed\n",bytes_to_io);
    return 2;
  }

  if ((fhndl = open(filename,O_BINARY | O_RDONLY,0)) == -1) {
    printf(couldnt_open,filename);
    free(buf);
    return 3;
  }

  bytes_read = read(fhndl,buf,bytes_to_io);

  if (bytes_read != bytes_to_io) {
    printf("read of %d bytes failed\n",bytes_to_io);
    free(buf);
    close(fhndl);
    return 4;
  }

  num_lines = 0;

  for (n = 0; n < bytes_to_io; n++) {
    if (buf[n] == 0x0a) {
      num_lines++;
      buf[n] = 0;
    }
  }

  malloc_size = sizeof (int) * num_lines;

  if ((offsets = (int *)malloc(malloc_size)) == NULL) {
    printf("malloc of %d bytes failed\n",malloc_size);
    return 5;
  }

  m = 0;
  offsets[0] = 0;

  for (n = 1; n < num_lines; n++) {
    for ( ; m < bytes_to_io; m++) {
      if (!buf[m]) {
        m++;
        break;
      }
    }

    offsets[n] = m;
  }

  close(fhndl);

  *buf_ptr = buf;
  *num_lines_ptr = num_lines;
  *offsets_ptr = offsets;

  return 0;
}

bool doppelgames(struct game *gamept1,struct game *gamept2)
{
  int n;

  for (n = 0; n < gamept1->num_moves; n++) {
    if (gamept1->moves[n].from != gamept2->moves[n].from)
      return false;

    if (gamept1->moves[n].to != gamept2->moves[n].to)
      return false;

    if (gamept1->moves[n].special_move_info != gamept2->moves[n].special_move_info)
      return false;
  }

  return true;
}
