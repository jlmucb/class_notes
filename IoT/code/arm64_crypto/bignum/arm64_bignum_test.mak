#
#    Copyright 2019 John Manferdelli, All Rights Reserved.
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
#    File: arm64_test.mak

SRC_DIR=$(HOME)/jlm/MySrc/arm64_crypto
ifndef SRC_DIR
SRC_DIR=$(HOME)/jlm/MySrc/arm64_crypto
endif
ifndef OBJ_DIR
OBJ_DIR=$(HOME)/cryptoobj/arm64
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
TARGET_MACHINE_TYPE= arm64
endif

S= $(SRC_DIR)/bignum
O= $(OBJ_DIR)/bignum
INCLUDE= -I$(S) -I$(SRC_DIR)/include -I/usr/local/include -I$(GOOGLE_INCLUDE)

CFLAGS=$(INCLUDE) -O3 -g -Wall -std=c++11
CFLAGS1=$(INCLUDE) -O3 -g -Wall -std=c++11

# readelf: readelf –a running_arm_entropy.exe

# 32 bit arm
# Compiler: arm-linux-gnueabi-g++ --static
# Compiler: arm-linux-gnueabi-gcc --static
# objdump: arm-linux-gnu-objdump --disassemble-all test1_arm.o
# Simulator: qemu-arm running_arm_entropy.exe --in find_key1_arm.o

# 64 bit arm
# compile: aarch64-linux-gnu-g++ -static
# compile: aarch64-linux-gnu-gcc -static
# objdump: aarch64-linux-gnu-objdump --disassemble-all test1_arm.o
# to run: qemu-aarch64-static arm_test.exe


SIMTARGET=1
ifdef SIMTARGET
	CC=aarch64-linux-gnu-g++ -static
	LINK=aarch64-linux-gnu-g++ -static
	AR=ar
	LDFLAGS= # $(LOCAL_LIB)/libprotobuf.a -L$(LOCAL_LIB) -lgtest -lgflags -lprotobuf -lpthread
else
	CC=g++
	LINK=g++
	AR=ar
	export LD_LIBRARY_PATH=/usr/local/lib
	LDFLAGS= -lprotobuf -lgtest -lgflags -lpthread
endif

dobj=	$(O)/arm64_bignum_test.o $(O)/bignum.cc.o $(O)/arith.o $(O)/algs.o $(O)/util.o $(O)/smallprimes.o

all:	arm64_bignum_test.exe
clean:
	@echo "removing object files"
	rm $(O)/*.o
	@echo "removing executable file"
	rm $(EXE_DIR)/arm64_bignum_test.exe

arm64_bignum_test.exe: $(dobj) 
	@echo "linking executable files"
	$(LINK) -o $(EXE_DIR)/arm64_bignum_test.exe $(dobj) $(LDFLAGS)

$(O)/arm64_bignum_test.o: $(S)/arm64_bignum_test.cc
	@echo "compiling arm64_bignum_test.cc"
	$(CC) $(CFLAGS) -c -o $(O)/arm64_bignum_test.o $(S)/arm64_bignum_test.cc

$(O)/arith.o: $(S)/arith.cc
	@echo "compiling arith.cc"
	$(CC) $(CFLAGS) -c -o $(O)/arith.o $(S)/arith.cc

$(O)/bignum.cc.o: $(S)/bignum.cc
	@echo "compiling bignum.cc"
	$(CC) $(CFLAGS) -c -o $(O)/bignum.cc.o $(S)/bignum.cc

$(O)/algs.o: $(S)/algs.cc
	@echo "compiling algs.cc"
	$(CC) $(CFLAGS) -c -o $(O)/algs.o $(S)/algs.cc

$(O)/util.o: $(SRC_DIR)/support/util.cc
	@echo "compiling util.cc"
	$(CC) $(CFLAGS) -c -o $(O)/util.o $(SRC_DIR)/support/util.cc

$(O)/smallprimes.o: $(S)/smallprimes.cc
	@echo "compiling smallprimes.cc"
	$(CC) $(CFLAGS) -c -o $(O)/smallprimes.o $(S)/smallprimes.cc
