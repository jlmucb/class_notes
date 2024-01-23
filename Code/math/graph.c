#include <stdio.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>


/*
 * (c) Copyright, 1990, John L. Manferdelli. All Rights Reserved.
 */


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


int xpsize={0};
int ypsize={0};


InitGraphics(i,j)

{

  xpsize= i;
  ypsize= j;

  if((display= XOpenDisplay(NULL))==NULL) {
    printf("Cannot open X Server\n");
    return(0);
    }
  screen= DefaultScreen(display);
  /* set window attributes */
  win= XCreateSimpleWindow(display,RootWindow(display,screen),1,1,xpsize,ypsize,
         10,BlackPixel(display,screen),WhitePixel(display,screen));
  if(win==NULL) {
    printf("Cannot open X Server\n");
    return(0);
    }
  size_hints.flags= PSize;
  size_hints.width= xpsize;
  size_hints.height= ypsize;
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
  XClearArea(display,win,0,0,xpsize,ypsize,True);
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
      if(map[k]>0)
        XDrawPoint(display,win,gc,j,i);
      }
    }
}


CloseGraphics()

{
  XDestroyWindow(display,win);
  XCloseDisplay(display);
}



scale(xsf,ysf,x,y,ixw,iyw,ixc,iyc,ix,iy)

double x,y,xsf,ysf;
int ixw,iyw,ixc,iyc,*ix,*iy;

{
  int i,j;

#ifdef DEBUG1
  printf("scale(%f,%f,x:%f,y:%f,xw:%d,yw:%d,xc:%d,yc:%d)\n",xsf,ysf,x,y,ixw,iyw,ixc,iyc);
#endif
  /* ixw dots = xsf */
  i= x/xsf;
  j= y/ysf;
#ifdef DEBUG1
  printf("scaled:(%d,%d), ", i,j);
#endif
  i= ixw-ixc+i;
  j= iyw-iyc-j;
#ifdef DEBUG1
  printf("translated:(%d,%d)\n",i,j);
#endif
  if((i<0)|(i>=ixw))
    return(0);
  if((j<0)|(j>=ixw))
    return(0);
  *ix= i;
  *iy= j;
  return(1);
}



int onscreen= {0};


displaybits(map,x,y,xm,ym)

char map[];
int x,y,xm,ym;

{
  XEvent report;
  char in[10];

  for(;;) {
    XNextEvent(display,&report);
    if(report.type==Expose) {
      printf("got Expose event\n");
      if(onscreen==0) {
        onscreen= 1;
        ClearGraphics();
        Drawbits(map,x,y,xm,ym);
        }
      }
    else if(report.type==KeyPress)
      break;
    else if(report.type==ButtonPress)
      onscreen= 0;
    }
    
  read(0,in,10);
  return;
}


/* ----------------------------------------------------------------------- */
