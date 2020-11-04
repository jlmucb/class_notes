HEXDUMP_HOME=	\jlm
BIN=		$(HEXDUMP_HOME)\bin
SRC=		$(HEXDUMP_HOME)\util

CC=		cl
OPTSW=		-O
INCLDIR=	"\Program Files\Devstudio\Vc\include"
CFLAGS=		/nologo /c /DEBUG $(OPTSW) -I$(INCLDIR) -I$(INCLDIR)/sys \
/D "_DEBUG" /D "DEBUG" /D "_CONSOLE" /D "_MBCS" /Zi
LINK=		link
LINKFLAGS=	/nologo /DEBUG /debug /subsystem:console  
LIB=		c:\"Program Files"\DevStudio\Vc\lib;c:\"Program Files"\DevStudio\sharedide\bin
LIBDIR=		\"Program Files"\DevStudio\Vc\lib;\"Program Files"\DevStudio\sharedide\bin
LIBEXT=		libcd.lib

srcs= hexdump.c


all: hexdump.exe 

hexdump.exe: $(BIN)\hexdump.obj 
	@echo "hexdump"
	$(LINK) $(LINKFLAGS) /OUT:$(BIN)/hexdump.exe $(pobjs) /pdb:$(BIN)\hexdump.pdb $(LIBEXT)


$(BIN)/hexdump.obj: hexdump.c
	$(CC) $(CFLAGS) -c hexdump.c
	mv hexdump.obj $@

