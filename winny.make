#
# Windows Makefile for Ascal 
#

#Build using Cygwin "make -f winny" 
CC  = g++
CXX = g++
#for linux
INCLUDES = Calculator.hpp setting.hpp Object.hpp AscalFrame.hpp AscalParameters.hpp ObjectKey.hpp -I/usr/include/readline 
ASINCLUDE = -IC:/cygwin64/usr/include/
#for mac os if linux version doesn't work
#INCLUDES = Calculator.hpp setting.hpp Object.hpp AscalFrame.hpp AscalParameters.hpp -I/usr/local/Cellar/boost/1.76.0/include -I/usr/local/opt/readline/include 
UTILINCLUDES = queue.hpp stack.hpp Vect2D.hpp unsortedlist.hpp
CFLAGS   = -O3 -Wall
CXXFLAGS = -O3 -std=c++17 -Wall -Dlibreadline $(ASINCLUDE)
#Windows build 
LDFLAGS = -L/bin/ -lreadline7 -LC:/Users/arhum/source/repos/Ascal-VS19 -lSDL2 -lIMAGESDL2
LDLIBS = -L/bin/ -lreadline7 -LC:/Users/arhum/source/repos/Ascal-VS19 -lSDL2 -lIMAGESDL2
#for debian if previous fails
#LDFLAGS = -lreadline -L/usr/lib/x86_64-linux-gnu 
#LDLIBS = -lreadline -L/usr/lib/x86_64-linux-gnu 
.PHONY: default
default: 
	make clean; make -j 7 ascal
ascal: AscalMain
	mv AscalMain ascal
AscalMain:  AscalMain.o Object.o AscalParameters.o string_view.o Ascal.o AscalExecutor.o ParsingUtil.o PRNG.o Keyword.o MemoryMap.o MemoryManager.o SubStr.o

# header dependency
AscalMain.o: AscalMain.cpp 

Object.o:

AscalParameters.o:

string_view.o:
Ascal.o: 
AscalExecutor.o:
ParsingUtil.o:
PRNG.o:
Keyword.o:
MemoryMap.o:
MemoryManager.o:
SubStr.o:
.PHONY: clean
clean:
	rm -f *.o *~  ascal
.PHONY: run
run:
	./run.sh
.PHONY: all
all: clean default

