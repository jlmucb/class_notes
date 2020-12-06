#include <stdio.h>


/*
 * (c) Copyright, 1990, John L. Manferdelli. All Rights Reserved.
 */

#ifdef X11
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#endif

#define XM 700
#define YM 700
#define XLM 100
#define YLM 50
#define SCALE 750
#define ITER 64
#define ITERSCALE 8

#define cadd(r,x,y) {r[0]= x[0]+y[0];r[1]=x[1]+y[1];}
#define cmult(r,x,y) {r[0]= x[0]*y[0]-x[1]*y[1];r[1]=x[1]*y[0]+x[0]*y[1];}


/* ----------------------------------------------------------------------- */


#ifdef X11
Display *display;
int screen;
Window win;
GC gc;
Pixmap pix;
XFontStruct *font;
long unsigned valuemask={0};
XSetWindowAttributes att;
XSizeHints size_hints;
XGCValues gcvalues;



InitGraphics()

{
  if((display= XOpenDisplay(NULL))==NULL) {
    printf("Cannot open X Server\n");
    return(0);
    }
  screen= DefaultScreen(display);
  /* set window attributes */
  win= XCreateSimpleWindow(display,RootWindow(display,screen),1,1,XM,YM,
         10,BlackPixel(display,screen),WhitePixel(display,screen));
  if(win==NULL) {
    printf("Cannot open X Server\n");
    return(0);
    }
  size_hints.flags= PSize;
  size_hints.width= XM;
  size_hints.height= YM;
  size_hints.min_width= 75;
  size_hints.min_height= 75;
  XSetStandardProperties(display,win,"JLM Graphics","JLM",None,NULL,0,
        &size_hints);
  XSelectInput(display,win,ExposureMask|KeyPressMask|ButtonPressMask);
  font= XLoadQueryFont(display,"9x15");
  gc= XCreateGC(display,win,valuemask,&gcvalues);
  XSetFont(display,gc,font->fid);
  XSetForeground(display,gc,BlackPixel(display,screen));
  /* XSetLineAttributes */
  XMapWindow(display,win);
  /* XNextEvent(display,report) XDrawString */
  return(1);
}


ClearGraphics()

{
  XClearArea(display,win,0,0,XM,YM,True);
}


Drawbits(map,x,y,xm,ym)

char map[];
int x,y,xm,ym;

{
  int i,j,k;

  XClearArea(display,win,0,0,xm,ym,True);
  for(i=0;i<y;i++) {
    for(j=0;j<x;j++) {
      k= i*xm+j;
      if(map[k]>5)
        XDrawPoint(display,win,gc,j,i);
      }
    }
}


CloseGraphics()

{
  XDestroyWindow(display,win);
  XCloseDisplay(display);
}
#endif


/* ----------------------------------------------------------------------- */


char map[XM*YM];    /* "bit map" */
int gr={0};


main(argn,argv)

int argn;
char *argv[];

{
  int i,j,k;
  double a[2],b[2],z[2],c[2],x,y;
  char in[10];
  int onscreen={0};

#ifdef X11
  XEvent report;
  if(InitGraphics()<=0) {
    printf("Can't use bitmap graphics\n");
    }
  else
    gr= 1;
#endif
  printf("Computing set, %dx%d grid %d iterations graphics ",XM,YM,ITER);
  if(gr>0)
    printf("on\n");
  else
    printf("off\n");

  for(i=0;i<YM;i++) {
    y= i-YLM;
    y/= (double) SCALE;
    for(j=0;j<XM;j++) {
      x= j-XLM;
      x/= (double) SCALE;
      z[0]= 0.0;
      z[1]= 0.0;
      c[0]= x;
      c[1]= y;
#ifdef DEBUG1
      printf("z: %f+%fi, c: %f+%fi, ",z[0],z[1],c[0],c[1]);
#endif
      for(k=0;k<ITER;k++) {
        cmult(a,z,z);
        cadd(z,a,c);
#ifdef DEBUG7
        printf("new z: %f+%fi\n",z[0],z[1]);
#endif
        if((z[0]*z[0]+z[1]*z[1])>=4.0)
          break;
        }
#ifdef DEBUG1
      printf("lasted %d iterations\n",k);
#endif
      map[i*XM+j]= k/ITERSCALE;
      }
    }
  for(i=0;i<YM;i++) {
    for(j=0;j<80;j++) {
      k= map[i*XM+j];
      printf("%x",k);
      }
    printf("\n");
    }
#ifdef X11
  for(;;) {
    XNextEvent(display,&report);
    if(report.type==Expose) {
      printf("got Expose event\n");
      if(onscreen==0) {
        onscreen= 1;
        ClearGraphics();
        Drawbits(map,XM,YM,XM,YM);
        }
      }
    else if(report.type==KeyPress)
      break;
    else if(report.type==ButtonPress)
      onscreen= 0;
    }
    
#endif
  read(0,in,10);
  CloseGraphics();
  exit();
}


/* ----------------------------------------------------------------------- */


