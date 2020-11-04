#include "stdio.h"


class List {
public:
	List*	m_pNext;
	int 	m_iC;

	List(int i) {m_pNext= NULL; m_iC= i;}
};

void StringReverse(char* pS)
{
	char c, d;
	char *p, *q;

	p= pS;
	q= p;
	while(*q!='\0')
		q++;
	q--;

	while(q>p) {
		c= *p;
		d= *q;
		*q= c;
		*p= d;
		p++;
		q--;
	}
}


List* ReverseList(List* pL)
{
	List* pLast= NULL;
	List* pNext;

	while(pL!=NULL) {
		pNext= pL->m_pNext;
		pL->m_pNext= pLast;
		pLast= pL;
		pL= pNext;
	}
	return(pLast);
}


void printList(List* pL)
{
	while(pL!=NULL) {
		printf("%d ", pL->m_iC);
		pL= pL->m_pNext;
	}
	printf("\n");
}


void MakeList(List** ppL)
{
	int i= 0;
	List* pL= new List(i++);
	*ppL= pL;

	while (i<10) {
		pL->m_pNext= new List(i++);
		pL= pL->m_pNext;
	}
}


void Fill(char* p, char rgsz[])
{
	int i;

	for(i=0;i<20;i++) {
		rgsz[i]= *p++;
		if(rgsz[i]==0)
			break;
	}
}


int main(int an, char** av) 
{
	char* g_szString1= "abcdefgh";
	char* g_szString2= "abcdefghi";
	char  rgString[20];
	int i;

	Fill(g_szString1, rgString);
	printf("String: %s,", rgString);
	StringReverse(rgString);
	printf("%s\n", rgString);

	Fill(g_szString2, rgString);
	printf("String: %s,", rgString);
	StringReverse(rgString);
	printf("%s\n", rgString);

	List* pL;
	MakeList(&pL);
	printList(pL);

	List* pLR= ReverseList(pL);
	printList(pLR);
	return(0);
}


// ------------------------------------------------------------------------
