
Object.o: Object.hpp Object.cppz

AscalFrame.o: AscalFrame.hpp
	g++ -c -Wall AscalFrame.hpp

AscalParams.o: AscalParameters.hpp AscalParameters.cpp
	g++ -c -Wall AscalParameters.hpp AscalParameters.cpp

