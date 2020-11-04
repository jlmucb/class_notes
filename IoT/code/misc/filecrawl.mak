#
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
#    File: cryptolib.mak


ifndef SRC_DIR
SRC_DIR=$(HOME)/jlm/MySrc/misc
endif
ifndef OBJ_DIR
OBJ_DIR=$(HOME)/cryptoobj
endif
ifndef EXE_DIR
EXE_DIR=$(HOME)/cryptobin
endif
ifndef GOOGLE_INCLUDE
GOOGLE_INCLUDE=/usr/local/include/google
endif
ifndef LOCAL_LIB
LOCAL_LIB=/usr/local/lib
endif
ifndef TARGET_MACHINE_TYPE
TARGET_MACHINE_TYPE= x64
endif

O= $(OBJ_DIR)/cryptolib
INCLUDE= -I$(HOME)/crypto/include

CFLAGS=$(INCLUDE) -O3 -g -Wall -std=c++11
CFLAGS1=$(INCLUDE) -O1 -g -Wall -std=c++11

CRYPTOLIB= $(OBJ_DIR)/jlmcryptolib.a

YOSEMITE="yes"
ifdef YOSEMITE
	CC=clang++
	LINK=clang++
	LDFLAGS= 
else
	CC=g++
	LINK=g++
	export LD_LIBRARY_PATH=/usr/local/lib
	LDFLAGS= # -lprotobuf -lgtest -lgflags -lpthread
endif

dobj=	$(O)/filecrawl.o
clib= $(OBJ_DIR)/jlmcryptolib.a
all: $(EXE_DIR)/filecrawl.exe

$(EXE_DIR)/filecrawl.exe: $(dobj)
	echo "Linking filecrawl.exe"
	$(LINK) -o $(EXE_DIR)/filecrawl.exe $(dobj) $(CRYPTOLIB)

$(O)/filecrawl.o: $(SRC_DIR)/filecrawl.cc
	@echo "compiling filecrawl.cc"
	$(CC) $(CFLAGS) -c -o $(O)/filecrawl.o $(SRC_DIR)/filecrawl.cc


