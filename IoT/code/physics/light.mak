ORIGIN	= QCWIN
ORIGIN_VER	= 1.00

PROJ	=LIGHT
DEBUG	=1
PROGTYPE	=4
CALLER	=
ARGS	=
DLLS	=
CVPACK	=1
CC	=cl -qc
RC	=rc
CFLAGS_G_WEXE	=/AS /G2w /Zp /W3 /D_WINDOWS
CFLAGS_D_WEXE	=/Gi /Od /Zi
CFLAGS_R_WEXE	=/O /Os /DNDEBUG
CFLAGS_G_WDLL	=/AS /G2w /Zp /Aw /W3 /D_WINDOWS /D_WINDLL
CFLAGS_D_WDLL	=/Gi /Od /Zi
CFLAGS_R_WDLL	=/O /Os /DNDEBUG
CFLAGS_G_WTTY	=/AS /G2w /W3 /D_WINDOWS
CFLAGS_D_WTTY	=/Gi /Od /Zi
CFLAGS_R_WTTY	=/O /Os /DNDEBUG
CFLAGS_G_DEXE	=/AS /W2
CFLAGS_D_DEXE	=/Gi /Od /Zi
CFLAGS_R_DEXE	=/O /Ot /DNDEBUG
CFLAGS	=$(CFLAGS_G_DEXE) $(CFLAGS_D_DEXE)
LFLAGS_G_WEXE	=/ST:5120 /A:16
LFLAGS_D_WEXE	=/CO
LFLAGS_R_WEXE	=
LFLAGS_G_WDLL	=/ST:5120 /A:16
LFLAGS_D_WDLL	=/CO
LFLAGS_R_WDLL	=
LFLAGS_G_WTTY	=/ST:5120 /A:16
LFLAGS_D_WTTY	=/CO
LFLAGS_R_WTTY	=
LFLAGS_G_DEXE	=/NOI /ST:2048
LFLAGS_D_DEXE	=/CO
LFLAGS_R_DEXE	=
LFLAGS	=$(LFLAGS_G_DEXE) $(LFLAGS_D_DEXE)
RCFLAGS	=
RESFLAGS	=-t
RUNFLAGS	=
OBJS_EXT = 	
LIBS_EXT = 	

.rc.res: ; $(RC) $(RCFLAGS) -r $*.rc

all:	$(PROJ).EXE

UV.OBJ:	UV.C $(H)

$(PROJ).EXE:	UV.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
UV.OBJ +
$(OBJS_EXT)
$(PROJ).EXE

C:\QCWIN\LIB\+
/NOD slibcer oldnames 
$(DEFFILE);
<<
	link $(LFLAGS) @$(PROJ).CRF

run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)
