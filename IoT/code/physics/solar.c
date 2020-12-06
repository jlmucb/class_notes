#include <stdio.h>
#include <math.h>


/*
 *  determine the chemical composition of the star according
 *  to the star's polytrope index, en.
 */


double lnr[1001],lnpr[1001],lnmr[1001],massr[1001];
double lnrho[1001],dlrdlm[1001],dlpdlm[1001],b[1001];
double b11[1001],b21[1001],c12[1001],c22[1001];
double alpha1[1001],alpha2[1001],beta1[1001],beta2[1001];
double dellnp[1001],dellnr[1001],large[1001];
double ge[1001],mr[1001],r[1001],rho[1001],pr[1001];
double en,xi,radsun,g,massun,cntrpr,cntrdn,fourpi,k,z,z1,z8,lnk;
double dmassr,dlnmr;


main(argn, argv)

int argn;
char *argv[];

{
  int i;

  printf("enter the polytrope index of the star: ");
  sscanf(argv[1],"%lf",&en);
  printf("polytrope index of the star: %G\n",en);
  if(en==1.0)
    xi=0.63662;
  if(en==1.5)
    xi=0.42422;
  if(en==2.0)
    xi=0.36475;
  if(en==2.5)
    xi=0.35150;

  /* initial conditions for stellar model */
  radsun= 6.95997e+10;
  g= 6.672041e-08;
  massun= 1.9892e+33;
  cntrpr= 3.4e+17;
  cntrdn= 160.0;

  fourpi= 1.0/(4.0*3.1415926536);
  k= xi*g*pow(massun,((en-1.0)/en))*pow(radsun,((3.0-en)/en));
  printf("k= %G\n",k);

  /* solar model calculations */
  dmassr= massun/1000.0;
  massr[0]= dmassr;
  z8= log(cntrpr/k);
  lnr[0]= .333333*(log(3.0*fourpi*massr[0])-((1.0/en)*z8));
  lnpr[0]= log(cntrpr);
  lnk= log(k);
  lnmr[0]= log(massr[0]);
  lnrho[0]= log(cntrdn);
  dlrdlm[0]= fourpi*exp(lnmr[0]-(3.0*lnr[0])-((1.0/en)*z8));
  dlpdlm[0]= -g*fourpi*exp((2.0*lnmr[0])-(4.0*lnr[0])-lnpr[0]);

  for(i=0;i<1000;i++) {
    lnmr[i+1]= log(massr[i]+dmassr);
    dlnmr= lnmr[i+1]-lnmr[i];
    lnr[i+1]= lnr[i]+(dlrdlm[i]*dlnmr);
    lnrho[i+1]= (1.0/en)*(lnpr[i]-lnk);
    z= exp(lnmr[i+1]-(3.0*lnr[i+1])-lnrho[i+1]);
    dlrdlm[i+1]= fourpi*z;
    lnr[i+1]= lnr[i]+(0.5*dlnmr*(dlrdlm[i+1]+dlrdlm[i]));
    lnpr[i+1]= lnpr[i]+(dlpdlm[i]*dlnmr);
    z1= exp((2.0*lnmr[i+1])-(4.0*lnr[i+1])-lnpr[i+1]);
    dlpdlm[i+1]= -g*fourpi*z1;
    lnpr[i+1]= lnpr[i]+(0.5*dlnmr*(dlpdlm[i]+dlpdlm[i+1]));
    massr[i+1]= massr[i]+dmassr;
    if(massr[i+1]>massun)
      break;
    }

  printf("\n");
  printf("           mass         radius        density       pressure\n");
  for(i=0;i<1000;i++) {
    mr[i]= exp(lnmr[i]);
    r[i]= exp(lnr[i]);
    rho[i]= exp(lnrho[i]);
    pr[i]= exp(lnpr[i]);
    printf(" %14G %14G %14G %14G\n",mr[i],r[i],rho[i],pr[i]);
    }

  printf("done\n");
  exit();
}

