#
#    Copyright 2024 John Manferdelli, All Rights Reserved.
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


WORK_DIR= $(HOME)/src/github.com/jlmucb/class_notes/Code
#ifndef SRC_DIR
SRC_DIR=$(WORK_DIR)/perms
#endif
#ifndef OBJ_DIR
OBJ_DIR=$(HOME)/cryptoobj/perms
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
TARGET_MACHINE_TYPE= x64
#endif

O= $(HOME)/tmp
INCLUDE= -I$(SRC_DIR) -I/usr/local/include -I$(GOOGLE_INCLUDE) 

CFLAGS=$(INCLUDE) -O3 -g -Wall -std=c++11
CFLAGS1=$(INCLUDE) -O1 -g -Wall -std=c++11

ifdef YOSEMITE
	CC=clang++
	LINK=clang++
	LDFLAGS= $(LOCAL_LIB)/libprotobuf.a -L$(LOCAL_LIB) -lgtest -lgflags -lprotobuf
else
	CC=g++
	LINK=g++
	export LD_LIBRARY_PATH=/usr/local/lib
	LDFLAGS= -lprotobuf -lgtest -lgflags
endif

dobj=	$(O)/perm_test.o $(O)/perm_lib.o

all:	$(EXE_DIR)/perm_test.exe
clean:
	@echo "removing object files"

$(EXE_DIR)/perm_test.exe: $(dobj) 
	@echo "linking executable files"
	$(LINK) -o $(EXE_DIR)/perm_test.exe $(dobj) $(LDFLAGS)
 
$(O)/perm_lib.o: $(SRC_DIR)/perm_lib.cc
	@echo "compiling perm_lib.cc"
	$(CC) $(CFLAGS) -c -o $(O)/perm_lib.o $(SRC_DIR)/perm_lib.cc
 
$(O)/perm_test.o: $(SRC_DIR)/perm_test.cc
	@echo "compiling perm_test.cc"
	$(CC) $(CFLAGS) -c -o $(O)/perm_test.o $(SRC_DIR)/perm_test.cc

