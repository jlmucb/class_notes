#include <stdio.h>
#include "bt.h"


/*
 *  (c) Copyright, 1991, John L. Manferdelli.  All Rights Reserved.
 */


/* ------------------------------------------------------------------------ */


#define NC 32
char *ab[]={
  "ksjd","RTY","uerty","32847","(*&","eiuwdh",
  "SBHW","WSGIA","OIU","M(*&^Y","a,mansd",
  "/.,/.,","JKHKUJH","qaqwe","LKJ","asdasd",
  "1",";","cvbn","uuu","LLKK"
  "ksjd","RTY","uerty","32847","(*&","eiuwdh",
  "SBHW","WSGIA","OIU","M(*&^Y","a,mansd","$%"
  };



/* ------------------------------------------------------------------------ */


main(an,av)

int an;
char *av[];

{
  int i,j,k;
  extern int bubble(),merge();
  extern strcmp();

  printf("Before sorting:\n");
  for(i=0;i<NC;i++)
    printf("\t%s\n",ab[i]);
  bubble(NC,(unsigned)(&ab[0]),strcmp);
  printf("After sorting:\n");
  for(i=0;i<NC;i++)
    printf("\t%s\n",ab[i]);
  exit(1);
}


/* ------------------------------------------------------------------------ */



