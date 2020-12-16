echo "Building Ascal Interpreter with g++ -O2"
g++ -c -O2 -std=c++17 AscalMain.cpp AscalFrame.hpp AscalParameters.cpp Object.cpp  ObjectKey.cpp 
g++ -o ascal AscalMain.o Object.o AscalParameters.o ObjectKey.o
rm AscalMain.o Object.o AscalParameters.o ObjectKey.o
clear
echo "Starting Ascal interpreter"
echo "Running, awaiting input, try typing o;fibr(3)"
echo "That will calculate the third fibonacci number in the sequence 2, and show all the calculations"
echo "and branching paths it took to calculate the result."
./ascal
