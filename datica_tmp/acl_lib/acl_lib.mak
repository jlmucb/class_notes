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
#    File: acl_lib.mak


ifndef SRC_DIR
SRC_DIR=$(HOME)/src/github.com/jlmucb/class_notes
endif
ifndef OBJ_DIR
OBJ_DIR=$(HOME)/cryptoobj
endif
ifndef EXE_DIR
EXE_DIR=$(HOME)/cryptobin
endif
ifndef LIB_DIR
LIB_DIR=$(SRC_DIR)
endif
#ifndef GOOGLE_INCLUDE
#GOOGLE_INCLUDE=/usr/local/include/g
#endif
ifndef LOCAL_LIB
LOCAL_LIB=/usr/local/lib
endif
ifndef TARGET_MACHINE_TYPE
TARGET_MACHINE_TYPE= x64
endif


S= $(SRC_DIR)/datica_tmp/acl_lib
O= $(OBJ_DIR)/acl_lib
INCLUDE= -I$(SRC_DIR)/include -I$(S) -I/usr/local/include

CFLAGS=$(INCLUDE) -O3 -g -Wall -std=c++11 -Wno-unused-variable -D X64
CFLAGS1=$(INCLUDE) -O1 -g -Wall -std=c++11 -Wno-unused-variable -D X64
CC=g++
LINK=g++
PROTO=protoc
AR=ar
#export LD_LIBRARY_PATH=/usr/local/lib
LDFLAGS= -lprotobuf -lgtest -lgflags -lpthread

# build the library later
tobj=   $(O)/acl.pb.o $(O)/acl.o $(O)/test_acl.o


all:	test_acl.exe
clean:
	@echo "removing object files"
	rm $(O)/*.o
	@echo "removing executable file"
	rm $(EXE_DIR)/test_acl.exe
	$(LINK) -o $(EXE_DIR)/test_acl.exe $(tobj) $(LDFLAGS)


test_acl.exe: $(tobj)
	@echo "linking executable files"
	$(LINK) -o $(EXE_DIR)/test_acl.exe $(tobj) $(LDFLAGS)

$(S)/acl.pb.cc $(S)/acl.pb.h: $(S)/acl.proto
	$(PROTO) -I=$(S) --cpp_out=$(S) $(S)/acl.proto

$(O)/test_acl.o: $(S)/test_acl.cc $(S)/acl.pb.h
	@echo "compiling test_acl.cc"
	$(CC) $(CFLAGS) -c $(I) -o $(O)/test_acl.o $(S)/test_acl.cc

$(O)/acl.pb.o: $(S)/acl.pb.cc $(S)/acl.pb.h
	@echo "compiling acl.pb.cc"
	$(CC) $(CFLAGS) -c $(I) -o $(O)/acl.pb.o $(S)/acl.pb.cc

$(O)/acl.o: $(S)/acl.cc $(S)/acl.pb.h
	@echo "compiling acl.cc"
	$(CC) $(CFLAGS) -c $(I) -o $(O)/acl.o $(S)/acl.cc
