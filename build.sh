echo "Building Ascal Interpreter with g++ -o"
g++ -c  AscalMain.cpp AscalFrame.h AscalParameters.h AscalParameters.cpp Object.h Object.cpp queue.h setting.h stack.h unsortedlist.h 
rm AscalFrame.h.gch AscalParameters.h.gch Object.h.gch queue.h.gch stack.h.gch unsortedlist.h.gch setting.h.gch
g++ -o ascal AscalMain.o Object.o AscalParameters.o   
rm AscalMain.o Object.o AscalParameters.o
rm AscalFrame.h.gch AscalParameters.h.gch Object.h.gch queue.h.gch stack.h.gch unsortedlist.h.gch setting.h.gch
clear
echo "Starting Ascal interpreter"
echo "Running, awaiting input, try typing o;fibr(3)"
echo "That will calculate the third fibonacci number in the sequence 2, and show all the calculations"
echo "and branching paths it took to calculate the result."
./ascal
