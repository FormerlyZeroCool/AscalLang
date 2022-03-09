echo "Building Ascal Interpreter with g++ -O2"
g++ -c -O3 -Wall -march=native -std=c++17 -I/usr/local/include/ AscalMain.cpp AscalParameters.cpp Object.cpp   string_view.cpp Ascal.cpp AscalExecutor.cpp CrossPlatform.cpp ParsingUtil.cpp PRNG.cpp Keyword.cpp MemoryMap.cpp MemoryManager.cpp SubStr.cpp
g++ -o ascal -O3 AscalMain.o Object.o AscalParameters.o string_view.o Ascal.o AscalExecutor.o CrossPlatform.o ParsingUtil.o PRNG.o Keyword.o MemoryMap.o MemoryManager.o SubStr.o
rm AscalMain.o Object.o AscalParameters.o string_view.o Ascal.o AscalExecutor.o CrossPlatform.o ParsingUtil.o PRNG.o Keyword.o MemoryMap.o MemoryManager.o SubStr.o

echo "Starting Ascal interpreter"
echo "Running, awaiting input, try typing o;fibr(3)"
echo "That will calculate the third fibonacci number in the sequence 2, and show all the calculations"
echo "and branching paths it took to calculate the result."
#./ascal
