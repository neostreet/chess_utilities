#include <stdio.h>
#include <string.h>

char pieces[] = "rnbqkbnr";
#define NUM_FILES 8
int ix_array[NUM_FILES];

#define NUM_BASELINES 960
char baselines[NUM_BASELINES][NUM_FILES+1];

int validate_candidate_baseline(char *candidate);

int main(int argc,char **argv)
{
  int m;
  int n;
  int o;
  int p;
  int q;
  int r;
  int s;
  int t;
  int u;
  int num_baselines;
  char candidate[NUM_FILES+1];

  num_baselines = 0;

  for (m = 0; m < NUM_FILES; m++) {
    ix_array[0] = m;

    for (n = 0; n < NUM_FILES; n++) {
      if (n == m)
        continue;

      ix_array[1] = n;

      for (o = 0; o < NUM_FILES; o++) {
        if ((o == m) || (o == n))
          continue;

        ix_array[2] = o;

        for (p = 0; p < NUM_FILES; p++) {
          if ((p == m) || (p == n) || (p == o))
            continue;

          ix_array[3] = p;

          for (q = 0; q < NUM_FILES; q++) {
            if ((q == m) || (q == n) || (q == o) || (q == p))
              continue;

            ix_array[4] = q;

            for (r = 0; r < NUM_FILES; r++) {
              if ((r == m) || (r == n) || (r == o) || (r == p) || (r == q))
                continue;

              ix_array[5] = r;

              for (s = 0; s < NUM_FILES; s++) {
                if ((s == m) || (s == n) || (s == o) || (s == p) || (s == q) || (s == r))
                  continue;

                ix_array[6] = s;

                for (t = 0; t < NUM_FILES; t++) {
                  if ((t == m) || (t == n) || (t == o) || (t == p) || (t == q) || (t == r) || (t == s))
                    continue;

                  ix_array[7] = t;

                  for (u = 0; u < NUM_FILES; u++)
                    candidate[u] = pieces[ix_array[u]];

                  candidate[u] = 0;

                  for (u = 0; u < num_baselines; u++) {
                    if (!strcmp(candidate,baselines[u]))
                      break;
                  }

                  if (u == num_baselines) {
                    if (validate_candidate_baseline(candidate)) {
                      if (num_baselines == NUM_BASELINES) {
                        printf("too many unique baselines\n");
                        return 1;
                      }

                      strcpy(baselines[num_baselines++],candidate);
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }

  if (num_baselines != NUM_BASELINES) {
    printf("num_baselines < %d\n",NUM_BASELINES);
    return 2;
  }

  for (n = 0; n < NUM_BASELINES; n++)
    printf("%s\n",baselines[n]);

  return 0;
}

int validate_candidate_baseline(char *candidate)
{
  int n;
  int rook_ix1;
  int rook_ix2;
  int king_ix;
  int bishop_ix1;
  int bishop_ix2;

  rook_ix1 = -1;
  bishop_ix1 = -1;

  for (n = 0; n < NUM_FILES; n++) {
    if (candidate[n] == 'r') {
      if (rook_ix1 == -1)
        rook_ix1 = n;
      else
        rook_ix2 = n;
    }
    else if (candidate[n] == 'b') {
      if (bishop_ix1 == -1)
        bishop_ix1 = n;
      else
        bishop_ix2 = n;
    }
    else if (candidate[n] == 'k')
      king_ix = n;
  }

  if ((king_ix < rook_ix1) || (king_ix > rook_ix2))
    return 0;

  if (!((bishop_ix1 + bishop_ix2) % 2))
    return 0;

  return 1;
}
