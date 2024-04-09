#include <stdio.h>
#include <string.h>

#define MAX_FILENAME_LEN 256
static char filename[MAX_FILENAME_LEN];

#define MAX_LINE_LEN 8192
static char line[MAX_LINE_LEN];
static char date[MAX_LINE_LEN];
static char time[MAX_LINE_LEN];

static char usage[] =
"usage: fchess_checks (-verbose) (-date_time) (-none) (-terse) (-white) (-black)\n"
"  (-me) (-opponent) (-mate) player_name filename\n";
static char couldnt_open[] = "couldn't open %s\n";

static char white[] = "White";
#define WHITE_LEN (sizeof (white) - 1)
static char black[] = "Black";
#define BLACK_LEN (sizeof (black) - 1)
static char utcdate[] = "UTCDate";
#define UTCDATE_LEN (sizeof (utcdate) - 1)
static char utctime[] = "UTCTime";
#define UTCTIME_LEN (sizeof (utctime) - 1)

static void GetLine(FILE *fptr,char *line,int *line_len,int maxllen);
static int Contains(bool bCaseSens,char *line,int line_len,
  char *string,int string_len,int *index);
static void get_date(char *date,char *line);
static void get_time(char *time,char *line);
int count_checks(char *line,int line_len,int *white_checks_pt,int *black_checks_pt,bool bMate);

int main(int argc,char **argv)
{
  int n;
  int curr_arg;
  bool bVerbose;
  bool bDateTime;
  bool bNone;
  bool bTerse;
  bool bWhite;
  bool bBlack;
  bool bMe;
  bool bOpponent;
  bool bMate;
  bool bNotNone;
  int player_name_ix;
  int player_name_len;
  FILE *fptr0;
  int filename_len;
  FILE *fptr;
  int line_len;
  int line_no;
  bool bIAmWhite;
  int ix;
  int white_checks;
  int black_checks;
  int my_checks;
  int opponent_checks;
  int total_checks;
  int total_white_checks;
  int total_black_checks;
  int total_my_checks;
  int total_opponent_checks;
  int grand_total_checks;
  int num_games;

  if ((argc < 3) || (argc > 12)) {
    printf(usage);
    return 1;
  }

  bVerbose = false;
  bDateTime = false;
  bNone = false;
  bTerse = false;
  bWhite = false;
  bBlack = false;
  bMe = false;
  bOpponent = false;
  bMate = false;
  bNotNone= false;

  for (curr_arg = 1; curr_arg < argc; curr_arg++) {
    if (!strcmp(argv[curr_arg],"-verbose"))
      bVerbose = true;
    else if (!strcmp(argv[curr_arg],"-date_time"))
      bDateTime = true;
    else if (!strcmp(argv[curr_arg],"-none"))
      bNone = true;
    else if (!strcmp(argv[curr_arg],"-terse"))
      bTerse = true;
    else if (!strcmp(argv[curr_arg],"-white"))
      bWhite = true;
    else if (!strcmp(argv[curr_arg],"-black"))
      bBlack = true;
    else if (!strcmp(argv[curr_arg],"-me"))
      bMe = true;
    else if (!strcmp(argv[curr_arg],"-opponent"))
      bOpponent = true;
    else if (!strcmp(argv[curr_arg],"-mate")) {
      bMate = true;
      bNotNone = true;
    }
    else
      break;
  }

  if (argc - curr_arg != 2) {
    printf(usage);
    return 2;
  }

  if ((bWhite || bBlack) && (bMe || bOpponent)) {
    printf("can't specify -white and/or -black and also -me and/or -opponent\n");
    return 3;
  }

  if (bNone && bNotNone) {
    printf("can't specify both -none and -not_none\n");
    return 4;
  }

  player_name_ix = curr_arg++;
  player_name_len = strlen(argv[player_name_ix]);

  if ((fptr0 = fopen(argv[curr_arg],"r")) == NULL) {
    printf(couldnt_open,argv[curr_arg]);
    return 5;
  }

  total_white_checks = 0;
  total_black_checks = 0;
  total_my_checks = 0;
  total_opponent_checks = 0;
  grand_total_checks = 0;
  num_games = 0;

  for ( ; ; ) {
    GetLine(fptr0,filename,&filename_len,MAX_FILENAME_LEN);

    if (feof(fptr0))
      break;

    if ((fptr = fopen(filename,"r")) == NULL) {
      printf(couldnt_open,filename);
      continue;
    }

    num_games++;

    for ( ; ; ) {
      GetLine(fptr,line,&line_len,MAX_LINE_LEN);

      if (feof(fptr))
        break;

      if (Contains(true,
        line,line_len,
        argv[player_name_ix],player_name_len,
        &ix)) {

        if (Contains(true,
          line,line_len,
          white,WHITE_LEN,
          &ix)) {
          bIAmWhite = true;
        }
        else if (Contains(true,
          line,line_len,
          black,BLACK_LEN,
          &ix)) {
          bIAmWhite = false;
        }
        else {
          printf("%s: couldn't determine whether %s is white or black\n",
            filename,argv[player_name_ix]);
          return 6;
        }
      }
      else if (Contains(true,
        line,line_len,
        utcdate,UTCDATE_LEN,
        &ix)) {

        if (bDateTime)
          get_date(date,line);
      }
      else if (Contains(true,
        line,line_len,
        utctime,UTCTIME_LEN,
        &ix)) {

        if (bDateTime)
          get_time(time,line);
      }
      else if (!strncmp(line,"1. ",3)) {
        total_checks = count_checks(line,line_len,&white_checks,&black_checks,bMate);

        if (bIAmWhite) {
          my_checks = white_checks;
          opponent_checks = black_checks;
        }
        else {
          my_checks = black_checks;
          opponent_checks = white_checks;
        }

        total_white_checks += white_checks;
        total_black_checks += black_checks;
        total_my_checks += my_checks;
        total_opponent_checks += opponent_checks;
        grand_total_checks += total_checks;

        if ((!bNone && !bNotNone) || (bNone && !total_checks) || (bNotNone && total_checks)) {
          if (!bDateTime) {
            if (bNone && bTerse)
              printf("%s\n",filename);
            else {
              if (bWhite || bBlack) {
                if (bWhite) {
                  if (bBlack) {
                    printf("%3d white, %3d black, %3d total %s\n",white_checks,black_checks,total_checks,filename);
                  }
                  else {
                    printf("%3d white, %3d total %s\n",white_checks,total_checks,filename);
                  }
                } else if (bBlack) {
                  printf("%3d black, %3d total %s\n",black_checks,total_checks,filename);
                }
              }

              if (bVerbose || bMe || bOpponent) {
                if (bVerbose || bMe) {
                  if (bVerbose || bOpponent) {
                    if (bTerse)
                      printf("%d %d %d\n",my_checks,opponent_checks,total_checks);
                    else
                      printf("%3d me, %3d opponent, %3d total %s\n",my_checks,opponent_checks,total_checks,filename);
                  }
                  else {
                    if (bTerse)
                      printf("%d %d\n",my_checks,total_checks);
                    else
                      printf("%3d me, %3d total %s\n",my_checks,total_checks,filename);
                  }
                }
                else {
                  if (bVerbose || bOpponent) {
                    if (bTerse)
                      printf("%d %d\n",opponent_checks,total_checks);
                    else
                      printf("%3d opponent, %3d total %s\n",opponent_checks,total_checks,filename);
                  }
                }
              }
            }
          }
          else {
            if (bNone && bTerse)
              printf("%s %s %s\n",filename,date,time);
            else {
              if (bWhite || bBlack) {
                if (bWhite) {
                  if (bBlack) {
                    printf("%3d white, %3d black, %3d total %s %s %s\n",white_checks,black_checks,total_checks,filename,date,time);
                  }
                  else {
                    printf("%3d white, %3d total %s %s %s\n",white_checks,total_checks,filename,date,time);
                  }
                } else if (bBlack) {
                    printf("%3d black, %3d total %s %s %s\n",black_checks,total_checks,filename,date,time);
                }
              }

              if (bMe || bOpponent) {
                if (bMe) {
                  if (bOpponent) {
                    printf("%3d me, %3d opponent, %3d total %s %s %s\n",my_checks,opponent_checks,total_checks,filename,date,time);
                  }
                  else {
                    printf("%3d me, %3d total %s %s %s\n",my_checks,total_checks,filename,date,time);
                  }
                } else if (bOpponent) {
                    printf("%3d opponent, %3d total %s %s %s\n",opponent_checks,total_checks,filename,date,time);
                }
              }

              printf("%3d %s %s %s\n",total_checks,filename,date,time);
            }
          }
        }

        break;
      }
    }

    fclose(fptr);
  }

  fclose(fptr0);

  putchar(0x0a);
  printf("%5d white %s\n",total_white_checks,(bMate ? "mates" : "checks"));
  printf("%5d black %s\n",total_black_checks,(bMate ? "mates" : "checks"));
  printf("%5d my %s\n",total_my_checks,(bMate ? "mates" : "checks"));
  printf("%5d opponent %s\n",total_opponent_checks,(bMate ? "mates" : "checks"));
  printf("%5d total %s\n",grand_total_checks,(bMate ? "mates" : "checks"));
  putchar(0x0a);
  printf("%5d games\n",num_games);

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

int count_checks(char *line,int line_len,int *white_checks_pt,int *black_checks_pt,bool bMate)
{
  int n;
  int space_count;
  int white_checks;
  int black_checks;

  space_count = 0;
  white_checks = 0;
  black_checks = 0;

  if (!bMate) {
    for (n = 0; n < line_len; n++) {
      if (line[n] == '.')
        space_count = 0;
      else if (line[n] == ' ')
        space_count++;

      if ((line[n] == '+') || (line[n] == '#')) {
        if (space_count == 1)
          white_checks++;
        else
          black_checks++;
      }
    }
  }
  else {
    if (line[line_len - 5] == '#') {
      for (n = line_len - 6; n >= 0; n--) {
        if (line[n] == ' ')
          space_count++;
        else if (line[n] == '.') {
          if (space_count == 1)
            white_checks++;
          else
            black_checks++;

          break;
        }
      }
    }
  }

  *white_checks_pt = white_checks;
  *black_checks_pt = black_checks;

  return white_checks + black_checks;
}
