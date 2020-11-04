#include "stdio.h"

int main(int an, char** av) {
  int cnt = 0;
  int cnt0 = 0;
  int cnt1 = 0;
  int cnt2 = 0;
  int cnt3 = 0;
  int cnt4 = 0;
  int p = 5;
  int t;

  for (int i = 0; i < p; i++) {
    for (int j = 0; j < p; j++) {
      for (int k = 0; k < p; k++) {
        for (int l = 0; l < p; l++) {
          t = (i*i +(p*p - j*j) + k*k + (p*p - l*l)) % p;
          if (t == 0)
            cnt0++;
          if (t == 1)
            cnt1++;
          if (t == 2)
            cnt2++;
          if (t == 3)
            cnt3++;
          if (t == 4)
            cnt4++;
          cnt++;
        }
      }
    }
  }

  printf("Number of solutions for 0 is %d\n", cnt0);
  printf("Number of solutions for 1 is %d\n", cnt1);
  printf("Number of solutions for 2 is %d\n", cnt2);
  printf("Number of solutions for 3 is %d\n", cnt3);
  printf("Number of solutions for 4 is %d\n", cnt4);
  printf("Total count is %d\n", cnt);
  return 0;
}
