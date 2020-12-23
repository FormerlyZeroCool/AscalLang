
#
# Makefile for lab 6, part 1
#

CC  = gcc
CXX = g++

INCLUDES = 
CFLAGS   = -o2 -Wall $(INCLUDES)
CXXFLAGS = -o2 -std=c++17 -Wall $(INCLUDES)

LDFLAGS = 
LDLIBS = 

.PHONY: default
default: AscalMain
AscalMain:  AscalMain.o Object.o AscalParameters.o ObjectKey.o
	g++ -o ascal AscalMain.o Object.o AscalParameters.o ObjectKey.o
# header dependency
AscalMain.o: AscalMain.cpp  Calculator.hpp queue.hpp stack.hpp Vect2D.hpp unsortedlist.hpp setting.hpp Object.hpp AscalFrame.hpp AscalParameters.hpp ObjectKey.hpp

Object.o: Object.cpp Object.hpp
AscalParameters.o: 
ObjectKey.o:
.PHONY: clean
clean:
	rm -f *.o *~  ascal

.PHONY: all
all: clean default

