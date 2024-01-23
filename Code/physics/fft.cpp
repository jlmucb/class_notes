#include "stdio.h"
#include "math.h"
#include "complex.h"


// --------------------------------------------------------------------------------------------


void PrintComplex(Complex& z)
{
	int i= z.Re();
	int j= z.Im();

	printf("(%d + %d i)", i,j);
}

void PrintOpResult(Complex& zo, char* op, Complex& zi1, Complex& zi2)
{
	PrintComplex(zo); printf("= ");
	PrintComplex(zi1); printf(" %s ", op);
	PrintComplex(zi2); printf("\n");
}

class X {
	public:
	void f() {printf("X::f\n");};
	virtual void g() {printf("X::g\n");};
	};

class Y : public X {
public:
	void f() {printf("Y::f\n");};
	void g() {printf("Y::g\n");};
	};



int main(int an, char** av)

{
	int i;
	Complex zi(1,0);
	Complex zj(0,1);
	Complex zk(1,2);
	Complex zl(2,1);
	Complex zm(1,1);
	Complex zo1;
	Complex zo2;
	Complex zo3;

	zo1= zi+zj;
	zo2= zi-zj;
	zo3= zk*zl;

	PrintOpResult(zo1, "+", zi, zj);
	PrintOpResult(zo2, "-", zi, zj);
	PrintOpResult(zo3, "*", zk, zl);

	if(zi == zm) 
		printf("zi==zm\n");
	else
		printf("zi!=zm\n");

	class X xc;
	class Y yc;
	class X *p;

	p= &yc;
	p->f();
	p->g();
	return(1);
}


// --------------------------------------------------------------------------------------------

