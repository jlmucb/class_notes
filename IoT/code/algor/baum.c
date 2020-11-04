/*	3 May	*/
#define		GOOPr	for (r=part; r<size; r++)
#define		GOOPs	for (s=part; s<size; s++)
#include		<stdio.h>
#define	MOOPi	for(i=1;i<k;i++)
#define	LOOPi	for(i=0;i<k;i++)
#define	LOOPt	for(t=0;t<length;t++)
#define	LOOPs	for(s=0;s<size;s++)
#define	LOOPr	for(r=0;r<size;r++)
#define	ENTRY	rot[i][r][s]
#define	T	300
#define	R	3
#define	S	10
/*
 *	This is a general puropse rotor machine plaything
 *	27 March 1978   J. Reeds  and  D.  Andelman
 *
 */
float	xrand();
float	logprob[S];
float	log();
float mu[R][S];
int	mytime();
int	rand();
int	rin();
char	getlet();
float	prob[2*S],
	rot[R][S][S],
	tor[R][S][S],
	alpha[R][S],  beta[R][S],
	bb[S],
	dery[R][S][S], entropy, enttrue;
int	pt[T],
	ct[T],
	dt[T],
	rotation[R][T],
	sh[R][T],
	invsh[R][T],
	tperm[R][S];
int	length, size, sizem1, k, itno;
int	seed;
float	TM16;
char	*order[100];

main(){
	int command();
	int m,n,i;
	TM16=1.;
printf("Reeds.c  versiomn of  2 May  15:50");
	for(m=0;m<16;m++) TM16 = TM16/2;
	for (m=0; m<100; m++) order[m] = 0;
	order[0] = &"rotate";
	order[1] = &"pt";
	order[2] = &"ct";
	order[3] = &"init";
	order[4] = &"iter";
	order[5] = &"go";
	order[6] = &"wire";
	order[7] = &"wireall";
	order[8] = &"display";
	order[9] = &"param";
	order[10] = &"getprob";
	order[12] = &"displayall";
	order[11] = &"textlook";
	order[13] = &"regular";
	order[14] = &"chlen";
	order[15] = &"jiggle";
	order[16] = &"quit";
	order[17] = &"commands";
	order[18] = &"aitken";
	mydate();
	regular();
	TOP:
	m=command();
	if(m<0) printf("Unknown command\n");
 	if(m==0) rotmake();
	if(m==1) ptmake();
	if(m==2) ctmake();
	if(m==3) initial();
	if(m==4) iterate();
	if(m==5) {
		n=rin();
		for(i=0;i<n;i++) iterate();
	}
	if(m==6) wire(rin());
	if(m==7) LOOPi wire(i);
	if(m==8) {
		printf("Which rotor? ");
		n=rin();
		if (n<0 || n>=k) printf("hunh?\n");
		else display(n);
	}
	if(m==9) param();
	if (m==10) getprob();
	if (m==11) blurt(rin());
	if (m==12)  LOOPi display(i);
	if (m==13) regular();
	if (m==14) chlen();
	if (m==15) jiggle();
	if (m==16) goto OUT;
	if (m==17) {
		n=0;
		while(order[n] != 0) printf("%s\n", order[n++]);
	}
	if (m==18) {
		printf("Enter number of aitken steps: ");
		n = rin();
		for (i=0;i<n;i++) aitken();
	}
	goto TOP;
OUT:;
}
int command(){
	int samestr();
	char s[100];
 	int i; putchar(':');
	scanf("%s", s);
	for (i=0; order[i] != 0; i++)
		if (samestr(order[i],s))
			return(i);
  	return(-1);
}

int samestr(s,t) char *s,*t; {
	while(*t) if (*s++ != *t++) return(0);
 	if(*s) return(0);
	else return(1);
}



float dot(x,y) float *x, *y;
{
	float d;
	int i;
	d=0.;
	i=size;
	while(i--) d =+ *x++ * *y++;
	return(d);
}
m2(a,x,y,r) float (*a)[S], *x, *y; int r;{
	register float *row, *vect;
	float d;
	register int n;
	float *yy;
	int i, ii;
	ii = size - r;
	yy = &y[r];
	while (ii--) {
		d=0;
		row = a++;
		vect = x;
		n = size;
		while (n--) d += *row++ * *vect++;
		*yy++ = d;
	}
	ii = r;
	yy = y;
	while (ii--){
		d=0;
		row = a++;
		vect = x;
		n = size;
		while (n--) d += *row++ * *vect++;
		*yy++ = d;
	}
}


rotmake()
{
	int i, r, s, t;
	LOOPt {
		LOOPi{
			s=rand()+t;
			if (s<0) s = -s;
			s = ( s) % 1493;
			rotation[i][t] = s % size;
		}
		MOOPi sh[i-1][t]=
			(size+rotation[i][t]-rotation[i-1][t])%size;
		sh[k-1][t]=size-rotation[k-1][t];
		LOOPi
			invsh[i][t]=size-sh[i][t];
	}	
}

ptmake()
{
	float qrob[27], d;
	int i, r, s, t;
	qrob[0]= 0;
	enttrue=0;
	LOOPr
		qrob[r+1] = qrob[r]+ prob[r];
	qrob[size] = 2;
	qrob[0] = -1;
	for (t=0; t<T; t++) {
		d=xrand();
		LOOPr {
			if (qrob[r] < d && d<= qrob[r+1]) {
					if (t<length) enttrue +=
						logprob[r];
				pt[t]=r;
				break;
			}
		}
	}
	enttrue = enttrue / length;
	printf("Sample entropy = %10.5f\n", enttrue);
}

ctmake()
{
	int i, r,  t;
	for (t=0; t<T; t++) {
		LOOPr beta[k-1][r]=0;
		beta[k-1][(pt[t]+sh[k-1][t])%size] = 1;
		for (i=k-1;i>0;i--)
			m2(rot[i],beta[i],beta[i-1],sh[i-1][t]);
		m2(rot[0],beta[0],bb,0);
		LOOPr {
			if(bb[r]>.5) {
				dt[t]=r;
				ct[t] = (r+rotation[0][t]) % size;
				goto OUT;
			}
		}
		printf("gap at %d\n", t);
		LOOPr printf("bb[%2d] = %4.2f\n", r, bb[r]);
OUT:;
	}
}

initial()
{
	int i, r, s;
	LOOPi
		LOOPr
			LOOPs
				ENTRY=1;
	renorm();
	itno=0;
	LOOPi LOOPr mu[i][r] = 0;
}

blurt(n) int n;
{
	int i, t;
	if(n>length) n=length;
	for (t=0;t<n;t++){
		printf("%3d   %2d   ",t,pt[t]);
		LOOPi
			printf("%2d ", rotation[k-1-i][t]);
		printf("  (%2d)   %2d\n", dt[t], ct[t]);
	}
}

int rand()
{
	seed=3125*seed;
	return(seed);
}

float xrand()
{
	return(.5+rand()*TM16);
}
int mytime(){
	struct{
		int t1;
		int t2;
		int t3[2];
		int t4[2];
	}
	buffer;

	times(&buffer);
	return(buffer.t1);
}

mydate(){
	int t[2];
	char *ctime();
	time(t);
	printf("%s\n", ctime(t));
}
iterate(){
	register  float *betai, *deryir;
	float *probsh;
	int rr, ss;
	int *sh0, *sh1, *sh2, *shk1;
	int *ish1, *ishk1;
	float *beta0, *beta1, *beta2, *betak1;
	float *alpha0, *alpha1, *alpha2, *rotdtt;
	int dtt;
	float d, dot();
	float *bp;
	float  x;
	int time;
	int i, r, s,t;
	itno++;
	time =mytime();
	alpha0 = alpha[0];
	alpha1 = alpha[1];
	alpha2 = alpha[2];
	LOOPr alpha0[r] = 0;
	shk1 = sh[k-1];
	ish1 = invsh[1];
	sh0 = sh[0];
	sh1 = sh[1];
	betak1 = beta[k-1];
	entropy = 0;
	beta0 = beta[0];
	beta1 = beta[1];
	beta2 = beta[2];
	r = k * S * S;
	deryir = &dery[0][0][0];
	while(r--) *deryir++ = 0.0;
	LOOPt {
		dtt = dt[t];
		rotdtt = rot[0][dtt];
		probsh = & prob[ size - *shk1++ ];
		bp = betak1;
		rr = size;
		while(rr--) *bp++ = *probsh++;
		alpha0[dtt] = 1;
		r = *sh0;
		s = size - r;
		ss = size;
		while (s) alpha1[--s] = rotdtt[--ss];
		s = size;
		ss = r;
		while (ss) alpha1[--s] = rotdtt[--ss];
		if (k>2) m2(tor[1], alpha1, alpha2, *ish1++);
		if (k>2) m2(rot[2], beta2, beta1, *sh1++);
		if (k>1) m2(rot[1], beta1,  beta0, *sh0++);
		d = dot(rotdtt, beta0);
		entropy =- log(d);
		LOOPr{
				x = alpha[0][r] / d;
				deryir = dery[0][r];
				betai = beta[0];
				s = size;
				while(s--) *deryir++ += x * *betai++;
		}
		MOOPi
			for (r=1;r<size;r++) {
				x = alpha[i][r] / d;
				deryir = dery[i][r];
				betai = beta[i];
				s = sizem1;
				while(s--) *++deryir += x * *++betai;
		}
		alpha0[dtt] = 0;
	}
	entropy = entropy / length;
	LOOPi {
		LOOPr
			LOOPs dery[i][r][s] *= ENTRY;
		dstok(dery[i], rot[i], i, mu[i]);
	}
	renorm();
	time = mytime() - time;
	printf("Took %d/60 seconds\n", time);
	printf("itno, ent, true ent %3d  %10.5f  %10.5f\n",
				itno, entropy, enttrue);
}
 

renorm(){
	float d;
	int i, r, s, t;
	MOOPi
		LOOPr
			rot[i][r][0]=rot[i][0][r]=0;
	MOOPi  rot[i][0][0] = 1;
	LOOPi
		LOOPs {
			LOOPr if(ENTRY <0) ENTRY =0;
			d = 0;
			LOOPr d =+ ENTRY;
			LOOPr  ENTRY /= d;
	    		LOOPr    tor[i][s][r]=ENTRY;
	}
}
getprob(){
	int r, i;
	float d, unicity;
	putchar('\n');
	printf("enter %d theoretical relative plain text frequencies\n",
								size);
	LOOPr {
		printf("letter %d ",r);
		prob[r]=rin();
	}
	d=0;
	LOOPr  d =+ prob[r];
	LOOPr {
		prob[r] =/ d;
		prob[size+r] = prob[r];
		printf("%d  %8.4f\n", r, prob[r]);
	}
	d=0;
	LOOPr {
		logprob[r] = 0;
		if(prob[r]==0) continue;
		logprob[r] = -log(prob[r]);
		d += prob[r]*logprob[r];
	}
	printf("theoretical entropy = %10.5f\n",d);
	unicity = 0;
	for (i=1;i<size;i++) unicity =+ log( (double) i);
	unicity = (unicity*k + log(1.*size))/(log(1.*size)-d);
	printf("Unicity distance is %10.1f\n", unicity);
}

param(){
	int i;
	printf("type in length <= %d:  ", T);
	length=rin();
	printf("type in number of rotors <= %d:  ",R);
	k=rin();
	printf("type in alphabet size <= %d:  ",S);
	size=rin();
	sizem1 = size - 1;
	printf("type in the seed:  ");
	seed=rin();
}



display(i) int i;
{
	float score;
	int r, s, t;
	printf("rotor %d   after %d iterations and true rotor  (*) \n",i,itno);
	printf("numbers are 100 times p's \n");
 	LOOPr{
		printf("(%2d)...", r);
		LOOPs {
			printf(" %3.0f", 100* ENTRY);
			if (r==tperm[i][s]) putchar('*');
			else putchar(' ');
		}
		putchar('\n');
	}
 	score=0;
	LOOPs {
		r=tperm[i][s];
		score =+ ENTRY;
  	}
	if (i)
		score=100*(score-1)/(size-1);
	else
		score=100*score/size;
	printf("score= %5.2f\n",score);
}


wire(i) int i;{
	int m;
	char score[S];
	int  r, s;
	int bot;
TOP:
 	LOOPr
		LOOPs {
			ENTRY=0;
	}
	bot = 0;
	LOOPr  score[r] = -1;
	printf("wirings for rotor %d \n\n",i);
	if (i) {
		tperm[i][0] = 0;
		score[0]=0;
		bot=1;
	}
	LOOPs {
		if (s < bot)
			printf("wire  0 goes to: 0\n");
		if (s>=bot) {
			printf("wire %2d goes to: ",s);
			m=rin();
			tperm[i][s] = m;
			if(score[m]>=0){
				printf("not a permutation, out you go\n");
				goto TOP;
			}
			score[m] = s;
		}
		r = tperm[i][s];
		ENTRY = 1;
	}
}
regular(){
	int i;
	char isity;
	param();
	getprob();
	LOOPi wire(i);
	rotmake();
	ptmake();
	ctmake();
	printf("Type  y   to see wired rotors: ");
	scanf("%c \n", &isity);
	if (isity=='y') LOOPi display(i);
	blurt(10);
	initial();
	printf("Type  y   to see flat rotors: ");
	scanf("%c \n", &isity);
	if (isity=='y') LOOPi display(i);
	printf("All set to iterate\n");
}
jiggle(){
	int i, r, s, q;
	printf("Enter percent size of jiggle: ");
	q = rin();
	if (q<0) q = 0;
	LOOPi
		LOOPr
			LOOPs
				ENTRY += .01 * q * xrand() ;
	renorm();
}
chlen(){
	int t;
	printf("Input new length <= %d: ", T);
	length = rin();
	if (length < 0) length = 0;
	if ( length > T) length = T;
	enttrue = 0;
	LOOPt  enttrue += logprob[pt[t]];
	enttrue /= length;
	printf("New length is %d and new entropy is %10.5f\n",
				length, enttrue);
}
aitken(){
	float history[2][R][S][S], abs(), d1, d2, d;
	int i, r, s;
	LOOPi
		LOOPr
			LOOPs
				history[0][i][r][s] = ENTRY;
	iterate();
	LOOPi
		LOOPr
			LOOPs
				history[1][i][r][s] = ENTRY;
	iterate();
	LOOPi
		LOOPr
			LOOPs {
				d1 = history[1][i][r][s] -
					history[0][i][r][s];
				d2 = ENTRY - history[1][i][r][s];
				if (abs(d1) <= abs(d2)) d = d2;
				else {
					float ratio;
					ratio = d1/d2;
					if (ratio < 1.1) ratio = 1.1;
					d = d2/(ratio - 1);
				}
				ENTRY += d;
	}
	renorm();
}
float dsf(a,x,y,t, part) float a[S][S], x[S][S], y[S][S], t;
int part;
{
	float f, temp;
	int r, s;
	f = 0;
	GOOPr
		GOOPs
			if ((temp=x[r][s] - t * y[r][s]) > 0)
				f -= a[r][s] * y[r][s] / temp;
	return(f);
}
dstok(a,x, part, mu) float a[S][S], x[S][S], mu[S];
int part;
{
	float theta[S][S], y[S][S], lambda[S],
		thi[S], thj[S], t, tmax, temp, dsf();
	int ii;
	float sum;
	int i, j, k, r, s;
	if (part) part = 1;
	sum = a[1][1];
	GOOPr GOOPs if (a[r][s] > sum) sum = a[r][s];
	sum = sum * 1e-5;
	GOOPr GOOPs a[r][s] += sum;
   for (ii=0; ii < 8; ii++) {
	GOOPr thi[r] = thj[r] = 0;
	GOOPr
		GOOPs {
			theta[r][s] = x[r][s]*x[r][s]/a[r][s];
			thi[r] += theta[r][s];
			thj[s] += theta[r][s];
	}
	for (k=0; k<4; k++) {
		GOOPr {
			lambda[r] = 1;
			GOOPs lambda[r] -= theta[r][s] * mu[s];
			lambda[r] /= thi[r];
		}
		GOOPs {
			mu[s] = 1;
			GOOPr mu[s] -= lambda[r] * theta[r][s];
			mu[s] /= thj[s];
		}
	}
	GOOPr
		GOOPs
			y[r][s] = theta[r][s] * (lambda[r] + mu[s] )
				- x[r][s];
	t = 0;
	if (part) LOOPr y[r][0] = y[0][r] = 0;
	GOOPr GOOPs t += y[r][s];
	t /= ((size - part)*(size - part));
	GOOPr GOOPs  y[r][s] -= t;
	GOOPr {
		t = 0;
		GOOPs t += y[r][s];
		t /= (size-part);
		GOOPs y[r][s] -= t;
	}
	GOOPs {
		t = 0;
		GOOPr  t += y[r][s];
		t /= (size-part);
		GOOPr y[r][s] -= t;
	}
	t = temp = 0;
	GOOPr GOOPs {
		t += y[r][s] * y[r][s];
		temp += x[r][s] * x[r][s];
	}
	if ( t < 1e-8 * temp) continue;
	tmax = t = 0;
	GOOPr GOOPs {
		t += a[r][s] * a[r][s];
		temp = a[r][s] - x[r][s] * (lambda[r] + mu[s] );
		tmax += temp * temp;
	}
	if (tmax < 1e-8 * t) continue;
	tmax = 1e30;
	GOOPr
		GOOPs {
			if (y[r][s] <= 0) continue;
			t = x[r][s] / y[r][s];
			if (tmax > t) tmax = t;
	}
	if (dsf(a,x,y,0,part) <1e-9) continue;
	t = 1;
	while (dsf(a,x,y,t, part) > 1e-8 && t < tmax) t *= 1.5;
	if (t > tmax) t = .99 * tmax;
	while (dsf(a,x,y,t, part) < -1e-8) t *= .9;
	t /= .9;
	GOOPr
		GOOPs x[r][s] -= t * y[r][s];
    }
}



