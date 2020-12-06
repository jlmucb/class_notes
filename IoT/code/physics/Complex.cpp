#include "complex.h"

#ifndef NULL
#define NULL 0
#endif


// --------------------------------------------------------------------------------------------



Complex& Complex::operator=(const Complex& x)
{
	m_ia= x.m_ia;
	m_ib= x.m_ib;

	return(*this);
}


int Complex::Re()
{
	return(m_ia);
}


int Complex::Im()
{
	return(m_ib);
}


int Complex::Modulus()
{
	return(m_ia*m_ia + m_ib*m_ib);
}

Complex operator+(const Complex& x, const Complex& y)
{
	Complex z(x.m_ia+y.m_ia, x.m_ib+y.m_ib);
	return z;
}


Complex operator-(const Complex& x, const Complex& y)
{
	Complex z(x.m_ia-y.m_ia, x.m_ib-y.m_ib);
	return z;
}


Complex operator*(const Complex& x, const Complex& y)
{
	Complex z(x.m_ia*y.m_ia-x.m_ib*y.m_ib, x.m_ia*y.m_ib+x.m_ib*y.m_ia);
	return z;
}


Complex operator/(const Complex& x, const Complex& y)
{
	Complex z(x.m_ia*y.m_ia-x.m_ib*y.m_ib, x.m_ia*y.m_ib+x.m_ib*y.m_ia);
	return z;
}


bool operator==(const Complex& x, const Complex& y)
{
	return x.m_ia==y.m_ia && x.m_ib==y.m_ib;
}


// --------------------------------------------------------------------------------------------

