
/*
 *	Time formats
 *		0. Internal date: days since Jan 1, EPOCH
 *		1. Julian date (Double) days since Jan 1, 4713BC
 *		2. Year, month, day, hr
 *		3. Year, month, day, hr, min, sec
 *
 */

typedef long int Int;
typedef char Char;
typedef double Double;
typedef int Short;
typedef long Long;

#define PI 3.141592653589793
#define REARTH 6378
#define KMTOMI 0.621369949

/* HA of aries jan 0, 1986 */
#define TAEPOCH 2192.0
#define PARII  6.2602			/* 6.6245  1/0/86 */
/* #define PARII 6.624319 */

#define EPOCH 1980
#define W0 1.00273790934	/* angular speed of earth (per 24hrs) */

#define AUKM 1.496e8
#define AUM 1.496e11

#define TMInt 0
#define TMJUL 1
#define TM2 3

struct T2 {
	Int year;	/* year */
	Int mno;	/* month number */
	Int day;	/* day of month */
	Int hr;		/* hour of day */
	Int min;	/* minutes of hour */
	Double sec;	/* seconds in hour */
	};

extern int scmp1(Char *,Char *);
extern Int month12(Char *);
extern char *month21(Int);
extern Double intojul(Double);
extern printdate(struct T2 *);
extern Double jultoint(Double);
extern intot2(Double,struct T2 *,Int);
extern Double t2toint(struct T2 *,Int);
extern Double gmttocivil(Double, Double);
extern Double civiltogmt(Double,Double);
extern Double gmttolocal(Double,Double);
extern Double normalize(Double);
extern tomins(Double,Int *,Double *);
extern int sptriang(Double,Double,Double,Double *),
		   tospherical(Double,Double,Double,Double *,Double *),
		   eqtohor(Double,Double,Double,Double,
				   Double,Double,Double *,Double *),
		   degtora(Double,Char *), degtominsec(Double,Char *),
		   rotate(Double,Double,Double,
	   			  Double *,Double *,Double *,Char *,Double);
extern Double angles(Double,Char *,Char *);
extern Int fetchpos(char **,Double *,Double *,char **);




#ifndef NULL
#define NULL 0
#endif


