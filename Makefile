#
# Makefile for Ascal/SDL2
#

CC  = g++
CXX = g++

INCLUDES = Calculator.hpp setting.hpp Object.hpp AscalFrame.hpp AscalParameters.hpp ObjectKey.hpp Ascal.hpp AscalExecutor.hpp CrossPlatform.hpp ParsingUtil.hpp PRNG.hpp Keyword.hpp
ASINCLUDE = -IC:/cygwin64/usr/include/
UTILINCLUDES = queue.hpp stack.hpp Vect2D.hpp unsortedlist.hpp
CFLAGS   = -o3 
CXXFLAGS = -o3 -std=c++17 $(ASINCLUDE)

LDFLAGS = -LC:/Users/RUMMY/source/repos/Ascal-VS19 -lSDL2 -lIMAGESDL2

default: AscalMain 
AscalMain:  AscalMain.o Object.o AscalParameters.o ObjectKey.o string_view.o Ascal.o AscalExecutor.o CrossPlatform.o ParsingUtil.o PRNG.o Keyword.o Graphics.o keywords/PlotGUIAction.o plot_gui/Input.o plot_gui/Camera.o
	g++ -o ascal AscalMain.o Object.o AscalParameters.o ObjectKey.o string_view.o Ascal.o AscalExecutor.o CrossPlatform.o ParsingUtil.o PRNG.o Keyword.o Graphics.o keywords/PlotGUIAction.o plot_gui/Input.o plot_gui/Camera.o $(LDFLAGS) $(ASINCLUDE)
	
# header dependency
Graphics.o: plot_gui/Graphics.h Keyword.hpp AscalExecutor.hpp plot_gui/Camera.h

keywords/PlotGUIAction.o: keywords/PlotGUIAction.hpp Keyword.hpp CrossPlatform.hpp plot_gui/Input.h plot_gui/Camera.h

plot_gui/Input.o: plot_gui/Input.h

plot_gui/Camera.o: plot_gui/Camera.h

AscalMain.o: AscalMain.cpp $(INCLUDES)  $(UTILINCLUDES)

Ascal.o: Ascal.cpp Ascal.hpp AscalExecutor.hpp Keyword.hpp keywords/PlotGUIAction.hpp

Object.o: Object.cpp Object.hpp

AscalExecutor.o: $(INCLUDES)  $(UTILINCLUDES)

AscalParameters.o: $(INCLUDES)  $(UTILINCLUDES)

ObjectKey.o: $(INCLUDES)  $(UTILINCLUDES)

string_view.o: $(INCLUDES)  $(UTILINCLUDES)

PRNG.o: $(INCLUDES)  $(UTILINCLUDES)

CrossPlatform.o: $(INCLUDES)  $(UTILINCLUDES)

Keyword.o: $(INCLUDES)  $(UTILINCLUDES)

ParsingUtil.o: $(INCLUDES) $(UTILINCLUDES)


.PHONY: clean
clean:
	rm -f *.o *~  ascal
.PHONY: run
run:
	./run.sh
.PHONY: all
all: clean default