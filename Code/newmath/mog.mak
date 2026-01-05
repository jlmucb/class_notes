
#    Copyright 2014 John Manferdelli, All Rights Reserved.
#    Licensed under the Apache License, Version 2.0 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at
#        http://www.apache.org/licenses/LICENSE-2.0
#    or in the the file LICENSE-2.0.txt in the top level sourcedirectory
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#    See the License for the specific language governing permissions and
#    limitations under the License
#    Project: New Cloudproxy Crypto


#ifndef SRC_DIR
SRC_DIR=$(HOME)/src/github.com/jlmucb/class_notes/Code/newmath
#endif
#ifndef OBJ_DIR
#endif
#ifndef EXE_DIR
EXE_DIR=$(HOME)/cryptobin
#endif
#ifndef GOOGLE_INCLUDE
GOOGLE_INCLUDE=/usr/local/include/google
#endif
#ifndef LOCAL_LIB
LOCAL_LIB=/usr/local/lib
#endif
#ifndef TARGET_MACHINE_TYPE
TARGET_MACHINE_TYPE=x64
#endif

O= $(HOME)/cryptoobj

CFLAGS=$(INCLUDE) -O3 -g -Wall -std=c++17
CFLAGS1=$(INCLUDE) -O1 -g -Wall -std=c++17

ifdef MACSI
	TARGET_MACHINE_TYPE=arm64
	INCLUDE= -I/opt/homebrew/include
	CC=clang++
	LDFLAGS=-v -L/opt/homebrew/lib -lgtest -lprotobuf -lgflags 
	LINK=clang++
else
	INCLUDE= -I$(SRC_DIR)/include -I/usr/local/include -I$(SRC_DIR)/keys -I$(GOOGLE_INCLUDE) 
	ifdef YOSEMITE
		CC=clang++
		LINK=clang++
	LDFLAGS= $(LOCAL_LIB)/libprotobuf.a -L$(LOCAL_LIB) -lgflags -lprotobuf -lgtest
	else
		CC=g++
		LINK=g++
		export LD_LIBRARY_PATH=/usr/local/lib
		LDFLAGS= -lprotobuf -lgtest -lgflags
	endif
endif

dobj=	$(O)/mog.o 

all:	$(EXE_DIR)/mog.exe
clean:
	@echo "removing object files"

AR=ar
PROTO=protoc

$(EXE_DIR)/mog.exe: $(dobj) 
	@echo "linking executable files"
	$(LINK) -o $(EXE_DIR)/mog.exe $(dobj) $(LDFLAGS)
 
$(O)/mog.o: $(SRC_DIR)/mog.cc
	@echo "compiling mog.cc"
	$(CC) $(CFLAGS) -c -o $(O)/mog.o $(SRC_DIR)/mog.cc

