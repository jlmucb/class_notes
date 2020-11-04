#include <stdio.h>
#include "bt.h"


/*
 *	(c) Copyright, 1991, John L. Manferdelli.  All Rights Reserved.
 */


/* ------------------------------------------------------------------------ */


#define NC 20
char *ab[]={
	"ksjd","RTY","uerty","32847","(*&","eiuwdh",
	"SBHW","WSGIA","OIU","M(*&^Y","a,mansd",
	"/.,/.,","JKHKUJH","qaqwe","LKJ","asdasd",
	"1",";","cvbn","uuu","LLKK"
	};

unsigned root={NULL};


nf(n)

node *n;

{
	int i,k;
	unsigned *u,*v;

	k= n->nc;
	printf("NODE: %x, parent: %x, type: %d, children: %d\n",n,n->p,n->ct,k);
	u= &(n->c1);
	v= &(n->m1);
	for(i=0;i<k;i++) {
		printf("\tc: %x, m: %s\n",*u,*v);
		u++; v++;
		}
}


kf(n)

char *n;

{
	printf("\tleaf: %s\n",n);
}


/* ------------------------------------------------------------------------ */


main(an,av)

int an;
char *av[];

{
	int i,j,k;
	extern int bt_nn;
	extern strcmp();

	for(i=0;i<NC;i++) {
		printf("\n---------\ninsert %d\n",i);
		bt_insert(&root,ab[i],strcmp);
		bt_traverse(root,nf,kf,strcmp);
		bt_pc(root);
		printf("\n---------\n");
		}
	printf("\n---------\nDONE\n\n");
	printf("%d nodes used\n",bt_nn);
	bt_traverse(root,NULL,kf,strcmp);
	exit(1);
}


/* ------------------------------------------------------------------------ */



