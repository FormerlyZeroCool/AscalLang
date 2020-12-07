
echo off 
echo Building Ascal Interpreter with g++ -O2
echo g++ -O2 -std=c++17 AscalMain.cpp  AscalParameters.cpp Object.cpp queue.hpp setting.hpp stack.hpp unsortedlist.hpp
g++ -o ascal -O2  AscalMain.cpp  AscalParameters.cpp Object.cpp queue.hpp setting.hpp stack.hpp unsortedlist.hpp ObjectKey.cpp

echo Starting Ascal interpreter
echo Running, awaiting input, try typing o;fibr(3)
echo That will calculate the third fibonacci number in the sequence 2, and show all the calculations
echo and branching paths it took to calculate the result.
ascal.exe
