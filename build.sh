echo "Building Ascal Interpreter with g++ -o"
g++ -c  AscalMain.cpp AscalFrame.hpp AscalParameters.hpp AscalParameters.cpp Object.hpp Object.cpp queue.hpp setting.hpp stack.hpp unsortedlist.hpp 
rm AscalFrame.hpp.gch AscalParameters.hpp.gch Object.hpp.gch queue.hpp.gch stack.hpp.gch unsortedlist.hpp.gch setting.hpp.gch
g++ -o ascal AscalMain.o Object.o AscalParameters.o   
rm AscalMain.o Object.o AscalParameters.o
rm AscalFrame.hpp.gch AscalParameters.hpp.gch Object.hpp.gch queue.hpp.gch stack.hpp.gch unsortedlist.hpp.gch setting.hpp.gch
echo clear
echo "Starting Ascal interpreter"
echo "Running, awaiting input, try typing o;fibr(3)"
echo "That will calculate the third fibonacci number in the sequence 2, and show all the calculations"
echo "and branching paths it took to calculate the result."
./ascal
