#include <stdio.h>
#include <dos.h>
#include <io.h>


main(argn,argv)

int argn;
char *argv[];

{
  int in, out;
  char cr,buf[256];
  int nb, i,j,k;

  nb= 0;
  if((in=open(argv[1],0))<=0) {
    printf("cannot open %s\n",argv[1]);
    exit(1);
    }
  if((out=creat(argv[2],0666))<0) {
    printf("Cannot creat: %s\n",argv[2]);
    exit(1);
    }
  cr= '\r';
  printf("UNIX to DOS, input: %s, output: %s\n",argv[1],argv[2]);

  for(;;) {
    if(nb<=0) {
      if((nb=read(in,buf,256))<=0)
        break;
      i= 0;
      }
    if(buf[i]=='\n')
      write(out,&cr,1);
    write(out,&buf[i++],1);
    nb--;
    }
  printf("done\n");
  close(in);
  close(out);
  exit();
}
