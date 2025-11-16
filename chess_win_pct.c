#include <stdio.h>

double chess_win_pct(int wins,int draws,int losses);

int main(int argc,char **argv)
{
  int wins;
  int draws;
  int losses;
  double win_pct;

  if (argc != 4) {
    printf("usage: chess_win_pct wins draws losses\n");
    return 1;
  }

  sscanf(argv[1],"%d",&wins);
  sscanf(argv[2],"%d",&draws);
  sscanf(argv[3],"%d",&losses);

  win_pct = chess_win_pct(wins,draws,losses);

  printf("%6.2lf\n",win_pct);

  return 0;
}

double chess_win_pct(int wins,int draws,int losses)
{
  int games;
  double win_pct;

  games = wins + draws + losses;

  if (!games)
    win_pct = (double)0;
  else
    win_pct = ((double)wins + ((double)draws / (double) 2)) / (double)games * (double)100;

  return win_pct;
}
