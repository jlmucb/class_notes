class Complex
{
friend class Complex;
friend Complex operator+(const Complex& x, const Complex& y);
friend Complex operator-(const Complex& x, const Complex& y);
friend Complex operator*(const Complex& x, const Complex& y);
friend Complex operator/(const Complex& x, const Complex& y);
friend bool operator==(const Complex& x, const Complex& y);

protected:
	int		m_ia, m_ib;

public:
	Complex() { m_ia= 0; m_ib= 0;}
	Complex(int a, int b) { m_ia= a; m_ib= b;}

    Complex& operator=(const Complex& x);
	int Re();
	int Im();
	int Modulus();
};

Complex operator+(const Complex& x, const Complex& y);
Complex operator-(const Complex& x, const Complex& y);
Complex operator*(const Complex& x, const Complex& y);
Complex operator/(const Complex& x, const Complex& y);

bool operator==(const Complex& x, const Complex& y);


