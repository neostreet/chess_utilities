#include <stdio.h>
#include <string.h>

#define MAX_FILENAME_LEN 256
static char filename[MAX_FILENAME_LEN];

#define MAX_LINE_LEN 1024
static char line[MAX_LINE_LEN];
static char date[MAX_LINE_LEN];
static char time[MAX_LINE_LEN];
static char date_time[MAX_LINE_LEN];

static char usage[] =
"usage: fchess_elo (-terse) (-verbose) (-rating_diff) (-after) (-before_and_after) (-before_and_after_diff) (-opponent)\n"
"  (-opponent_name) (-date) (-time) (-boundaryboundary) (-ge_eloelo) (-is_ge_eloelo) (-lt_eloelo) (-filename) player_name filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static char white[] = "[White \"";
#define WHITE_LEN (sizeof (white) - 1)
static char black[] = "[Black \"";
#define BLACK_LEN (sizeof (black) - 1)
static char white_elo[] = "WhiteElo \"";
#define WHITE_ELO_LEN (sizeof (white_elo) - 1)
static char black_elo[] = "BlackElo \"";
#define BLACK_ELO_LEN (sizeof (black_elo) - 1)
static char white_rating_diff[] = "WhiteRatingDiff \"";
#define WHITE_RATING_DIFF_LEN (sizeof (white_rating_diff) - 1)
static char black_rating_diff[] = "BlackRatingDiff \"";
#define BLACK_RATING_DIFF_LEN (sizeof (black_rating_diff) - 1)
#define MAX_OPPONENT_NAME_LEN 50
static char opponent_name[MAX_OPPONENT_NAME_LEN + 1];
static char utcdate[] = "UTCDate";
#define UTCDATE_LEN (sizeof (utcdate) - 1)
static char utctime[] = "UTCTime";
#define UTCTIME_LEN (sizeof (utctime) - 1)

static bool bDate;
static bool bTime;
static bool bFilename;

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
static int Contains(bool bCaseSens,char *line,int line_len,
  char *string,int string_len,int *index);
static void get_date(char *date,char *line);
static void get_time(char *time,char *line);

#define RATING_DIFF_BLOCK \
          if (boundary != -1) { \
            if ((elo < boundary) && (elo + rating_diff >= boundary)) \
              ; \
            else \
              break; \
          } \
          if ((ge_elo != -1) && (elo + rating_diff < ge_elo)) \
            break; \
          else if ((lt_elo != -1) && (elo + rating_diff >= lt_elo)) \
            break; \
          if (bTerse) { \
            if (bRatingDiff) \
              printf("%d\n",rating_diff); \
            else if (bAfter) { \
              if (is_ge_elo == -1) \
                printf("%d\n",elo+rating_diff); \
              else \
                printf("%d %d\n",(elo + rating_diff >= is_ge_elo),elo+rating_diff); \
            } \
            else { \
              if (is_ge_elo == -1) \
                printf("%d %d\n",elo,elo+rating_diff); \
              else \
                printf("%d %d %d\n",(elo + rating_diff >= is_ge_elo),elo,elo+rating_diff); \
            } \
          } \
          else if (bVerbose) { \
            if (bRatingDiff) { \
              if (!bDate && !bTime) {\
                if (!bFilename) \
                  printf("%d\n",rating_diff); \
                else \
                  printf("%d %s\n",rating_diff,filename); \
              } \
              else { \
                if (!bFilename) \
                  printf("%d %s\n",rating_diff,date_time); \
                else \
                  printf("%d %s %s\n",rating_diff,filename,date_time); \
              } \
            } \
            else if (bAfter) { \
              if (!bDate && !bTime) { \
                if (is_ge_elo == -1) \
                  printf("%d (%d) %s\n",elo+rating_diff,rating_diff,filename); \
                else \
                  printf("%d %d (%d) %s\n",(elo + rating_diff >= is_ge_elo),elo+rating_diff,rating_diff,filename); \
              } \
              else { \
                if (is_ge_elo == -1) \
                  printf("%d (%d) %s %s\n",elo+rating_diff,rating_diff,filename,date_time); \
                else \
                  printf("%d %d (%d) %s %s\n",(elo + rating_diff >= is_ge_elo),elo+rating_diff,rating_diff,filename,date_time); \
              } \
            } \
            else { \
              if (!bDate && !bTime) { \
                if (is_ge_elo == -1) \
                  printf("%d %d (%d) %s\n",elo,elo+rating_diff,rating_diff,filename); \
                else \
                  printf("%d %d %d (%d) %s\n",(elo + rating_diff >= is_ge_elo),elo,elo+rating_diff,rating_diff,filename); \
              } \
              else { \
                if (is_ge_elo == -1) { \
                  if (!bFilename) \
                    printf("%d %d (%d) %s\n",elo,elo+rating_diff,rating_diff,date_time); \
                  else \
                    printf("%d %d (%d) %s %s\n",elo,elo+rating_diff,rating_diff,filename,date_time); \
                } \
                else \
                  printf("%d %d %d (%d) %s %s\n",(elo + rating_diff >= is_ge_elo),elo,elo+rating_diff,rating_diff,filename,date_time); \
              } \
            } \
          } \
          else { \
            if (bRatingDiff) { \
              if (!bDate && !bTime) \
                printf("%d %s\n",rating_diff,filename); \
              else \
                printf("%d %s %s\n",rating_diff,filename,date_time); \
            } \
            else if (bAfter) { \
              if (!bDate && !bTime) { \
                if (is_ge_elo == -1) \
                  printf("%d %s\n",elo+rating_diff,filename); \
                else \
                  printf("%d %d %s\n",(elo + rating_diff >= is_ge_elo),elo+rating_diff,filename); \
              } \
              else { \
                if (is_ge_elo == -1) \
                  printf("%d %s %s\n",elo+rating_diff,filename,date_time); \
                else \
                  printf("%d %d %s %s\n",(elo + rating_diff >= is_ge_elo),elo+rating_diff,filename,date_time); \
              } \
            } \
            else { \
              if (!bDate && !bTime) { \
                if (is_ge_elo == -1) \
                  printf("%d %d %s\n",elo,elo+rating_diff,filename); \
                else \
                  printf("%d %d %d %s\n",(elo + rating_diff >= is_ge_elo),elo,elo+rating_diff,filename); \
              } \
              else { \
                if (is_ge_elo == -1) { \
                  if (!bFilename) \
                    printf("%d %d %s\n",elo,elo+rating_diff,date_time); \
                  else \
                    printf("%d %d %s %s\n",elo,elo+rating_diff,filename,date_time); \
                } \
                else \
                  printf("%d %d %d %s %s\n",(elo + rating_diff >= is_ge_elo),elo,elo+rating_diff,filename,date_time); \
              } \
            } \
          }

static int dbg_line_no;
static int dbg;

int main(int argc,char **argv)
{
  int m;
  int n;
  int curr_arg;
  bool bTerse;
  bool bVerbose;
  bool bRatingDiff;
  bool bAfter;
  bool bBeforeAndAfter;
  bool bBeforeAndAfterDiff;
  bool bOpponent;
  bool bOpponentName;
  int boundary;
  int ge_elo;
  int is_ge_elo;
  int lt_elo;
  int player_name_ix;
  int player_name_len;
  FILE *fptr0;
  int filename_len;
  FILE *fptr;
  int line_len;
  int line_no;
  bool bPlayerIsWhite;
  int ix;
  int elo;
  int rating_diff;

  if ((argc < 3) || (argc > 18)) {
    printf(usage);
    return 1;
  }

  bTerse = false;
  bVerbose = false;
  bRatingDiff = false;
  bAfter = false;
  bBeforeAndAfter = false;
  bBeforeAndAfterDiff = false;
  bOpponent = false;
  bOpponentName = false;
  bDate = false;
  bTime = false;
  boundary = -1;
  ge_elo = -1;
  is_ge_elo = -1;
  lt_elo = -1;
  bFilename = false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-terse"))
      bTerse = true;
    else if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else if (!strcmp(argv[curr_arg],"-rating_diff"))
      bRatingDiff = true;
    else if (!strcmp(argv[curr_arg],"-after"))
      bAfter = true;
    else if (!strcmp(argv[curr_arg],"-before_and_after"))
      bBeforeAndAfter = true;
    else if (!strcmp(argv[curr_arg],"-before_and_after_diff"))
      bBeforeAndAfterDiff = true;
    else if (!strcmp(argv[curr_arg],"-opponent"))
      bOpponent = true;
    else if (!strcmp(argv[curr_arg],"-opponent_name"))
      bOpponentName = true;
    else if (!strcmp(argv[curr_arg],"-date"))
      bDate = true;
    else if (!strcmp(argv[curr_arg],"-time"))
      bTime = true;
    else if (!strncmp(argv[curr_arg],"-boundary",9))
      sscanf(&argv[curr_arg][9],"%d",&boundary);
    else if (!strncmp(argv[curr_arg],"-ge_elo",7))
      sscanf(&argv[curr_arg][7],"%d",&ge_elo);
    else if (!strncmp(argv[curr_arg],"-is_ge_elo",10))
      sscanf(&argv[curr_arg][10],"%d",&is_ge_elo);
    else if (!strncmp(argv[curr_arg],"-lt_elo",7))
      sscanf(&argv[curr_arg][7],"%d",&lt_elo);
    else if (!strcmp(argv[curr_arg],"-filename"))
      bFilename = true;
    else
      break;
  }

  if (argc - curr_arg != 2) {
    printf(usage);
    return 2;
  }

  if (bTerse && bVerbose) {
    printf("can't specify both -terse and -verbose\n");
    return 3;
  }

  if (bRatingDiff && bAfter) {
    printf("can't specify both -rating_diff and -after\n");
    return 4;
  }

  if (bRatingDiff && bBeforeAndAfter) {
    printf("can't specify both -rating_diff and -before_and_after\n");
    return 5;
  }

  if (bAfter && bBeforeAndAfter) {
    printf("can't specify both -after and -before_and_after\n");
    return 6;
  }

  player_name_ix = curr_arg;
  player_name_len = strlen(argv[player_name_ix]);

  if ((fptr0 = fopen(argv[curr_arg + 1],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg + 1]);
    return 5;
  }

  for ( ; ; ) {
    GetLine(fptr0,filename,&filename_len,MAX_FILENAME_LEN);

    if (feof(fptr0))
      break;

    if ((fptr = fopen(filename,"r")) == NULL) {
      printf(couldnt_open,filename);
      continue;
    }

    line_no = 0;

    for ( ; ; ) {
      GetLine(fptr,line,&line_len,MAX_LINE_LEN);

      if (feof(fptr))
        break;

      line_no++;

      if (line_no == dbg_line_no)
        dbg = 1;

      if (Contains(true,
        line,line_len,
        utcdate,UTCDATE_LEN,
        &ix)) {

        if (bDate)
          get_date(date,line);
      }
      else if (Contains(true,
        line,line_len,
        utctime,UTCTIME_LEN,
        &ix)) {

        if (bTime)
          get_time(time,line);
      }
      else if (Contains(true,
        line,line_len,
        white_elo,WHITE_ELO_LEN,
        &ix)) {

        // build date_time from date and time
        if (bDate) {
          strcpy(date_time,date);

          if (bTime) {
            strcat(date_time," ");
            strcat(date_time,time);
          }
        }
        else if (bTime)
          strcpy(date_time,time);

        if ((bPlayerIsWhite && !bOpponent) ||
          (!bPlayerIsWhite && bOpponent)) {
          sscanf(&line[ix + WHITE_ELO_LEN],"%d",&elo);

          if ((ge_elo != -1) && (elo < ge_elo))
            continue;

          if (!bRatingDiff && !bAfter && !bBeforeAndAfter) {
            if (bTerse) {
              if (is_ge_elo == -1)
                printf("%d\n",elo);
              else
                printf("%d %d\n",(elo >= is_ge_elo),elo);
            }
            else {
              if (!bDate && !bTime) {
                if (!bOpponentName) {
                  if (is_ge_elo == -1)
                    printf("%d %s\n",elo,filename);
                  else
                    printf("%d %d %s\n",(elo >= is_ge_elo),elo,filename);
                }
                else {
                  if (is_ge_elo == -1)
                    printf("%d %s %s\n",elo,opponent_name,filename);
                  else
                    printf("%d %d %s %s\n",(elo >= is_ge_elo),elo,opponent_name,filename);
                }
              }
              else {
                if (!bOpponentName) {
                  if (is_ge_elo == -1)
                    printf("%d %s %s\n",elo,filename,date_time);
                  else
                    printf("%d %d %s %s\n",(elo >= is_ge_elo),elo,filename,date_time);
                }
                else {
                  if (is_ge_elo == -1)
                    printf("%d %s %s %s\n",elo,opponent_name,filename,date_time);
                  else
                    printf("%d %d %s %s %s\n",(elo >= is_ge_elo),elo,opponent_name,filename,date_time);
                }
              }
            }

            break;
          }
        }
      }
      else if (Contains(true,
        line,line_len,
        black_elo,BLACK_ELO_LEN,
        &ix)) {

        // build date_time from date and time
        if (bDate) {
          strcpy(date_time,date);

          if (bTime) {
            strcat(date_time," ");
            strcat(date_time,time);
          }
        }
        else if (bTime)
          strcpy(date_time,time);

        if ((!bPlayerIsWhite && !bOpponent) ||
          (bPlayerIsWhite && bOpponent)) {
          sscanf(&line[ix + BLACK_ELO_LEN],"%d",&elo);

          if ((ge_elo != -1) && (elo < ge_elo))
            continue;

          if (!bRatingDiff && !bAfter && !bBeforeAndAfter) {
            if (bTerse) {
              if (is_ge_elo == -1)
                printf("%d\n",elo);
              else
                printf("%d %d\n",(elo >= is_ge_elo),elo);
            }
            else {
              if (!bDate && !bTime) {
                if (!bOpponentName) {
                  if (is_ge_elo == -1)
                    printf("%d %s\n",elo,filename);
                  else
                    printf("%d %d %s\n",(elo >= is_ge_elo),elo,filename);
                }
                else {
                  if (is_ge_elo == -1)
                    printf("%d %s %s\n",elo,opponent_name,filename);
                  else
                    printf("%d %d %s %s\n",(elo >= is_ge_elo),elo,opponent_name,filename);
                }
              }
              else {
                if (!bOpponentName) {
                  if (is_ge_elo == -1)
                    printf("%d %s %s\n",elo,filename,date_time);
                  else
                    printf("%d %d %s %s\n",(elo >= is_ge_elo),elo,filename,date_time);
                }
                else {
                  if (is_ge_elo == -1)
                    printf("%d %s %s %s\n",elo,opponent_name,filename,date_time);
                  else
                    printf("%d %d %s %s %s\n",(elo >= is_ge_elo),elo,opponent_name,filename,date_time);
                }
              }
            }

            break;
          }
        }
      }
      else if ((bRatingDiff || bAfter || bBeforeAndAfter) && Contains(true,
        line,line_len,
        white_rating_diff,WHITE_RATING_DIFF_LEN,
        &ix)) {

        if (bPlayerIsWhite) {
          sscanf(&line[ix + WHITE_RATING_DIFF_LEN],"%d",&rating_diff);

          RATING_DIFF_BLOCK

          break;
        }
      }
      else if ((bRatingDiff || bAfter || bBeforeAndAfter) && Contains(true,
        line,line_len,
        black_rating_diff,BLACK_RATING_DIFF_LEN,
        &ix)) {

        if (!bPlayerIsWhite) {
          sscanf(&line[ix + BLACK_RATING_DIFF_LEN],"%d",&rating_diff);

          RATING_DIFF_BLOCK

          break;
        }
      }
      else if (Contains(true,
        line,line_len,
        argv[player_name_ix],player_name_len,
        &ix)) {

        if (Contains(true,
          line,line_len,
          white,WHITE_LEN,
          &ix)) {

          bPlayerIsWhite = true;
        }
        else if (Contains(true,
          line,line_len,
          black,BLACK_LEN,
          &ix)) {

          bPlayerIsWhite = false;
        }
        else {
          printf("%s: couldn't determine whether %s is white or black\n",
            filename,argv[player_name_ix]);
          return 8;
        }
      }
      else if (Contains(true,
        line,line_len,
        white,WHITE_LEN,
        &ix) ||
        Contains(true,
        line,line_len,
        black,BLACK_LEN,
        &ix)) {

        for (m = 0, n = ix + WHITE_LEN; n < line_len; n++) {
          if (line[n] == '"')
            break;

          if (m == MAX_OPPONENT_NAME_LEN)
            break;

          opponent_name[m++] = line[n];
        }

        opponent_name[m] = 0;
      }
    }

    fclose(fptr);
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

static void get_date(char *date,char *line)
{
  strncpy(date,&line[10],10);
  date[4] = '-';
  date[7] = '-';
}

static void get_time(char *time,char *line)
{
  strncpy(time,&line[10],8);
}
