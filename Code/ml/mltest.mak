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


SRC_DIR=$(HOME)/jlm/MySrc
OBJ_DIR=$(HOME)/cryptoobj
EXE_DIR=$(HOME)/cryptobin

ifndef GOOGLE_INCLUDE
GOOGLE_INCLUDE=/usr/local/include/google
endif
ifndef LOCAL_LIB
LOCAL_LIB=/usr/local/lib
endif
ifndef TARGET_MACHINE_TYPE
TARGET_MACHINE_TYPE= x64
endif

S= $(SRC_DIR)/ml
O= $(OBJ_DIR)/ml
INCLUDE= -I$(SRC_DIR)/include -I$(S) -I/usr/local/include -I$(GOOGLE_INCLUDE)

CFLAGS=$(INCLUDE) -O3 -g -Wall -std=c++11
CFLAGS1=$(INCLUDE) -O3 -g -Wall -std=c++11

YOSEMITE=true
ifdef YOSEMITE
	CC=clang++
	LINK=clang++
	AR=ar
	LDFLAGS= $(LOCAL_LIB)/libprotobuf.a -L$(LOCAL_LIB) -lgtest -lgflags -lprotobuf -lpthread
else
	CC=g++
	LINK=g++
	AR=ar
	export LD_LIBRARY_PATH=/usr/local/lib
	LDFLAGS= -lprotobuf -lgtest -lgflags -lpthread
endif

dobj=	$(O)/mlcommon.o $(O)/em.o $(O)/newton.o $(O)/matrix.o $(O)/svm.o $(O)/k_means.o \
        $(O)/naive_bayes.o $(O)/least_squares.o $(O)/gradient.o $(O)/gaussian_elimination.o \
        $(O)/pca.o $(O)/simple_statistics.o $(O)/logistic_regression.o \
        $(O)/adaboost.o $(O)/mltest.o 

all:	mltest.exe
clean:
	@echo "removing object files"
	rm $(O)/*.o
	@echo "removing executable file"
	rm $(EXE_DIR)/mltest.exe

mltest.exe: $(dobj) 
	@echo "linking executable files"
	$(LINK) -o $(EXE_DIR)/mltest.exe $(dobj) $(LDFLAGS)


$(O)/mltest.o: $(S)/mltest.cc
	@echo "compiling mltest.cc"
	$(CC) $(CFLAGS) -c -o $(O)/mltest.o $(S)/mltest.cc

$(O)/k_means.o: $(S)/k_means.cc
	@echo "compiling k_means.cc"
	$(CC) $(CFLAGS) -c -o $(O)/k_means.o $(S)/k_means.cc

$(O)/gradient.o: $(S)/gradient.cc
	@echo "compiling gradient.cc"
	$(CC) $(CFLAGS) -c -o $(O)/gradient.o $(S)/gradient.cc

$(O)/svm.o: $(S)/svm.cc
	@echo "compiling svm.cc"
	$(CC) $(CFLAGS) -c -o $(O)/svm.o $(S)/svm.cc

$(O)/matrix.o: $(S)/matrix.cc
	@echo "compiling matrix.cc"
	$(CC) $(CFLAGS) -c -o $(O)/matrix.o $(S)/matrix.cc

$(O)/newton.o: $(S)/newton.cc
	@echo "compiling newton.cc"
	$(CC) $(CFLAGS) -c -o $(O)/newton.o $(S)/newton.cc

$(O)/em.o: $(S)/em.cc
	@echo "compiling em.cc"
	$(CC) $(CFLAGS) -c -o $(O)/em.o $(S)/em.cc

$(O)/naive_bayes.o: $(S)/naive_bayes.cc
	@echo "compiling naive_bayes.cc"
	$(CC) $(CFLAGS) -c -o $(O)/naive_bayes.o $(S)/naive_bayes.cc

$(O)/least_squares.o: $(S)/least_squares.cc
	@echo "compiling least_squares.cc"
	$(CC) $(CFLAGS) -c -o $(O)/least_squares.o $(S)/least_squares.cc

$(O)/gaussian_elimination.o: $(S)/gaussian_elimination.cc
	@echo "compiling gaussian_elimination.cc"
	$(CC) $(CFLAGS) -c -o $(O)/gaussian_elimination.o $(S)/gaussian_elimination.cc

$(O)/simple_statistics.o: $(S)/simple_statistics.cc
	@echo "compiling simple_statistics.cc"
	$(CC) $(CFLAGS) -c -o $(O)/simple_statistics.o $(S)/simple_statistics.cc

$(O)/pca.o: $(S)/pca.cc
	@echo "compiling pca.cc"
	$(CC) $(CFLAGS) -c -o $(O)/pca.o $(S)/pca.cc

$(O)/logistic_regression.o: $(S)/logistic_regression.cc
	@echo "compiling logistic_regression.cc"
	$(CC) $(CFLAGS) -c -o $(O)/logistic_regression.o $(S)/logistic_regression.cc

$(O)/adaboost.o: $(S)/adaboost.cc
	@echo "compiling adaboost.cc"
	$(CC) $(CFLAGS) -c -o $(O)/adaboost.o $(S)/adaboost.cc

$(O)/mlcommon.o: $(S)/mlcommon.cc
	@echo "compiling mlcommon.cc"
	$(CC) $(CFLAGS) -c -o $(O)/mlcommon.o $(S)/mlcommon.cc
