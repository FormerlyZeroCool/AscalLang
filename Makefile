
#
# Makefile for lab 6, part 1
#

CC  = gcc
CXX = g++

INCLUDES = Calculator.hpp setting.hpp Object.hpp AscalFrame.hpp AscalParameters.hpp ObjectKey.hpp
UTILINCLUDES = queue.hpp stack.hpp Vect2D.hpp unsortedlist.hpp
CFLAGS   = -o2 -Wall
CXXFLAGS = -o2 -std=c++17 -Wall

LDFLAGS = 
LDLIBS = 

.PHONY: default
default: AscalMain
AscalMain:  AscalMain.o Object.o AscalParameters.o ObjectKey.o
	g++ -o ascal AscalMain.o Object.o AscalParameters.o ObjectKey.o
# header dependency
AscalMain.o: AscalMain.cpp $(INCLUDES)  $(UTILINCLUDES) 

Object.o: Object.cpp Object.hpp
AscalParameters.o: 
ObjectKey.o:
.PHONY: clean
clean:
	rm -f *.o *~  ascal
.PHONY: run
run:
	./run.sh
.PHONY: all
all: clean default

