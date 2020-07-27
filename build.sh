 g++ -c  AscalMain.cpp AscalFrame.h AscalParameters.h AscalParameters.cpp Object.h Object.cpp queue.h setting.h stack.h unsortedlist.h 
rm AscalFrame.h.gch AscalParameters.h.gch Object.h.gch queue.h.gch stack.h.gch unsortedlist.h.gch setting.h.gch
g++ -o a.out AscalMain.o Object.o AscalParameters.o   
rm AscalMain.o Object.o AscalParameters.o
clear
echo "Starting Ascal interpreter"
echo "Running, awaiting input, try typing o;fibr(9)"
echo "That will calculate the ninth fibonacci number, in the sequence, and show all the calculations,"
echo "and branching paths it took to calculate the result."
./a.out


