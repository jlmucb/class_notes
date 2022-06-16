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
#    File: my_openssl.mak


ifndef SRC_DIR
SRC_DIR=$(HOME)/src/github.com/jlmucb/class_notes/IoT/code
endif
ifndef OBJ_DIR
OBJ_DIR=$(HOME)/cryptoobj
endif
ifndef EXE_DIR
EXE_DIR=$(HOME)/cryptobin
endif
#ifndef GOOGLE_INCLUDE
#GOOGLE_INCLUDE=/usr/local/include/google
#endif
ifndef LOCAL_LIB
LOCAL_LIB=/usr/local/lib
endif
ifndef TARGET_MACHINE_TYPE
TARGET_MACHINE_TYPE= x64
endif


S= $(SRC_DIR)/nw
O= $(OBJ_DIR)/code/nw
INCLUDE= -I$(SRC_DIR)/code -I$(S) -I/usr/local/include -I/usr/local/opt/openssl@1.1/include/

CFLAGS=$(INCLUDE) -O3 -g -Wall -std=c++11 -Wno-unused-variable
# -D X64 -D_GLIBCXX_USE_CXX11_ABI=0
CFLAGS1=$(INCLUDE) -O1 -g -Wall -std=c++11 -Wno-unused-variable -D X64
CC=g++
LINK=g++ -std=c++11
PROTO=protoc
AR=ar
#export LD_LIBRARY_PATH=/usr/local/lib
LDFLAGS= -lprotobuf -lgtest -lgflags -lpthread -lcrypto -L/usr/local/opt//openssl@1.1/lib -lssl

dobj=	$(O)/test_my_openssl.o $(O)/my_ssl_lib.o # $(O)/ssl.pb.o


all:	test_my_openssl.exe
clean:
	@echo "removing object files"
	rm $(O)/*.o
	@echo "removing executable file"
	rm $(EXE_DIR)/test_my_openssl.exe

test_my_openssl.exe: $(dobj) 
	@echo "linking executable files"
	$(LINK) -o $(EXE_DIR)/test_my_openssl.exe $(dobj) $(LDFLAGS)

$(O)/test_my_openssl.o: $(S)/test_my_openssl.cc $(S)/ssl.pb.cc
	@echo "compiling test_my_openssl.cc"
	$(CC) $(CFLAGS) -c $(I) -o $(O)/test_my_openssl.o $(S)/test_my_openssl.cc

$(S)/ssl.pb.cc: $(S)/ssl.proto
	$(PROTO) -I=$(S) --cpp_out=$(S) $(S)/ssl.proto

$(O)/ssl.pb.o: $(S)/ssl.pb.cc $(S)/ssl.pb.h
	@echo "compiling ssl.pb.cc"
	$(CC) $(CFLAGS) -c $(I) -o $(O)/ssl.pb.o $(S)/ssl.pb.cc

$(O)/my_ssl_lib.o: $(S)/my_ssl_lib.cc
	@echo "compiling my_ssl_lib.cc"
	$(CC) $(CFLAGS) -c $(I) -o $(O)/my_ssl_lib.o $(S)/my_ssl_lib.cc

