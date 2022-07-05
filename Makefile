
#
# Makefile for lab 6, part 1
#

CC  = g++
CXX = g++
#for linux
INCLUDES = Calculator.hpp setting.hpp Object.hpp AscalFrame.hpp AscalParameters.hpp ObjectKey.hpp -I/usr/include/readline
#for mac os
#INCLUDES = Calculator.hpp setting.hpp Object.hpp AscalFrame.hpp AscalParameters.hpp 
UTILINCLUDES = queue.hpp stack.hpp Vect2D.hpp unsortedlist.hpp
CFLAGS   = -o3 -Wall
CXXFLAGS = -o3 -std=c++17 -Wall
#for linux distros
LDFLAGS = -lreadline -L/usr/lib/x86_64-linux-gnu
LDLIBS = -lreadline -L/usr/lib/x86_64-linux-gnu
#for mac os
#LDFLAGS = -L/usr/local/opt/readline/lib -lreadline
#LDLIBS = -L/usr/local/opt/readline/lib -lreadline
.PHONY: default
default: AscalMain
AscalMain:  AscalMain.o Object.o AscalParameters.o string_view.o Ascal.o AscalExecutor.o ParsingUtil.o PRNG.o Keyword.o MemoryMap.o MemoryManager.o SubStr.o

# header dependency
AscalMain.o: AscalMain.cpp $(INCLUDES)  $(UTILINCLUDES) 

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

