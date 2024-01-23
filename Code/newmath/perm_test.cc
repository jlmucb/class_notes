#include "perm.h"

int main(int an, char** av) {

  perm p1(5);
  p1.make_identity();
  p1.print();
  printf("\n");

  perm p2(5);
  for (int i = 0; i < p2.n_; i++) {
    p2.s_[i] = (i + 1) % p2.n_;
  }
  p2.print();
  printf("\n");

  perm p3(5);
  if (inverse(p2, &p3)) {
    printf("inverse: ");
    p3.print();
    printf("\n");
  } else {
    printf("Can't compute inverse\n");
  }

  perm p4(5);
  p4.make_identity();
  p4.s_[0] = 1;
  p4.s_[1] = 0;
  p4.print();
  printf("\n");

  perm p5(5);
  if (compose(p2, p3, &p5)) {
    printf("composed: ");
    p5.print();
    printf("\n");
  } else {
    printf("Can't compose (1)\n");
  }

  perm p6(5);
  if (compose(p2, p4, &p6)) {
    printf("composed: ");
    p6.print();
    printf("\n");
  } else {
    printf("Can't compose (1)\n");
  }

  for (int m = 0; m < 10; m++) {
    int m_fact = factorial(m);
    printf("%d! = %d\n", m, m_fact);
  }

  int m = 1;
  int m_fact = factorial(m);
  perm  perms1[m_fact];
  for (int i = 0; i < m_fact; i++) {
    perms1[i].n_ = m;
    perms1[i].s_ = new short int [m];
  }
  if (!gen_perms(m, perms1, perms1)) {
    printf("Can't generate %d perms\n", m);
    return 1;
  }
  printf("\n factorials on %d letters\n", m);
  for (int i = 0; i < m_fact; i++) {
    perms1[i].print();
    printf("\n");
  }

  m = 2;
  m_fact = factorial(m);
  perm  perms2[m_fact];
  for (int i = 0; i < m_fact; i++) {
    perms2[i].n_ = m;
    perms2[i].s_ = new short int [m];
  }
  if (!gen_perms(m, perms1, perms2)) {
    printf("Can't generate %d perms\n", m);
    return 1;
  }
  printf("\n factorials on %d letters\n", m);
  for (int i = 0; i < m_fact; i++) {
    perms2[i].print();
    printf("\n");
  }

  m = 3;
  m_fact = factorial(m);
  perm  perms3[m_fact];
  for (int i = 0; i < m_fact; i++) {
    perms3[i].n_ = m;
    perms3[i].s_ = new short int [m];
  }
  if (!gen_perms(m, perms2, perms3)) {
    printf("Can't generate %d perms\n", m);
    return 1;
  }
  printf("\n factorials on %d letters\n", m);
  for (int i = 0; i < m_fact; i++) {
    perms3[i].print();
    printf("\n");
  }

  m = 4;
  m_fact = factorial(m);
  perm  perms4[m_fact];
  for (int i = 0; i < m_fact; i++) {
    perms4[i].n_ = m;
    perms4[i].s_ = new short int [m];
  }
  if (!gen_perms(m, perms3, perms4)) {
    printf("Can't generate %d perms\n", m);
    return 1;
  }
  printf("\n factorials on %d letters\n", m);
  for (int i = 0; i < m_fact; i++) {
    printf("%d: ", i);
    perms4[i].print();
    printf("\n");
  }


  return 0;
}
