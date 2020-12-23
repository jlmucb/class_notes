/*                         
 *  (c) Copyright, 1991, John L. Manferdelli.  All Rights Reserved.
 */

#define NODE 0
#define LEAF 1

#ifndef NULL
#define NULL 0
#endif

#define NFP 200

struct node {
  struct node *p;      // parent
  int ct,nc;      // child type, number of childre
  unsigned c1,c2,c3;    // children
  unsigned m1,m2,m3;    // min of children
  };

typedef struct node node;
#define Btp(x) ((node *) x)

