S=          .
E=          ~/jlmcrypt

DEBUG_CFLAGS     := -Wall -Wno-format -g -DDEBUG
CFLAGS   := -Wall -Wno-unknown-pragmas -Wno-format -O3
LDFLAGSXML      := ${RELEASE_LDFLAGS}

CC=         g++
LINK=       g++

tobjs=  $(B)/sha1.o $(B)/Jerrold.o

all: $(E)/Jerrold.exe

$(E)/Jerrold.exe: $(tobjs)
	@echo "Jerrold"
	$(LINK) -o $(E)/Jerrold.exe $(tobjs)

$(B)/Jerrold.o: $(S)/Jerrold.cpp 
	$(CC) $(CFLAGS) -I$(S) -c -o $(B)/Jerrold.o $(S)/Jerrold.cpp

$(B)/sha1.o: $(S)/sha1.cpp $(S)/sha1.h
	$(CC) $(CFLAGS) -I$(S) -c -o $(B)/sha1.o $(S)/sha1.cpp

