//============================================================================
// Name        : Ascal.cpp
// Author      : Andrew Rubinstein
// Version     : v2.02
// Copyright   :
// Description : An infix expression interpreter, and simple math programming language in C++
// A.S.Cal.
// Andrew's Simple Calculator Language
//============================================================================
//On Line 46 there is a Macro defining a debug print function LOG_DEBUG that only will compile
//when the DEBUG Macro is equal to 1


#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <unordered_map>
#include <map>
#include <chrono>
#include <ctime>
#include <cmath>
#include <algorithm>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <thread>
#include "Ascal.hpp"
#include "AscalFrame.hpp"
#include "Object.hpp"
#include "setting.hpp"
#include "stack.hpp"
#include "Vect2D.hpp"
#include "Calculator.hpp"
#include "SubStr.hpp"

#define DEBUG 1
#define THROWERRORS 1
static const size_t sizeofFrame = sizeof(AscalFrame<double>);
static size_t allocated = 0, deallocated = 0;;
static const std::string VERSION = "2.01";

static uint32_t frameCount = 1;
static uint32_t varCount = 0;

static std::streambuf* stream_buffer_cin = std::cin.rdbuf();
/////////////////////////////
//Program Global Memory Declaration
static std::unordered_map<std::string,Object > memory;
std::vector<Object> userDefinedFunctions;
std::vector<Object> systemDefinedFunctions;
linkedStack<AscalFrame<double> *> *currentStack = nullptr;
//Interpreter Settings HashMap for toggle flags, like show time, or operations
static std::map<std::string,setting<bool> > boolsettings;
//Interpreter hash map for system keywords
static std::unordered_map
<std::string,
std::string (*)(AscalFrame<double>*,bool)> inputMapper;
static std::map
<ObjectKey,
std::string (*)(AscalFrame<double>*,Object&)> objectActionMapper;

static std::map<char, std::string> operations;
static size_t rememberedFromMemoTableCount;
//End Program Global Memory Declaration
/////////////////////////////





#if defined(WIN32) || defined(_WIN32)
#define PATH_SEPARATOR '\\'
#include <windows.h>
void usleep(__int64 usec)
{
    HANDLE timer;
    LARGE_INTEGER ft;

    ft.QuadPart = -(10*usec); // Convert to 100 nanosecond interval, negative value indicates relative time

    timer = CreateWaitableTimer(NULL, TRUE, NULL);
    SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0);
    WaitForSingleObject(timer, INFINITE);
    CloseHandle(timer);
}
#else
void usleep(uint64_t usec){
	std::this_thread::sleep_for(std::chrono::microseconds(usec));
}
#define PATH_SEPARATOR '/'
#endif

#if DEBUG==1
#define LOG_DEBUG(x)  if(*boolsettings["d"]) {std::cout<<x<<std::endl;}
short stackDepth = 0;
short maxDepth = 0;
#else
#define LOG_DEBUG(x)
#endif


int main(int argc,char* argv[])
{
  /*
   * Initializing values in system hashmaps
   * */

      //initializes operations hashmap that maps operators like + to the appropriate function
      //initOperations<double>();
      //initializes inputMapper hashmap for user input
      //initParamMapper();
      //Load Standard Library of functions fo Ascal
      //loadInitialFunctions();
	  Ascal ascalRuntime(argv, argc, 0);
	  std::cout<<"runtime initialized succseffullly\n";
  //Beginning of section interpreting program parameters from command line
	  bool running = true;
  std::string arg = "";
    try{
  if(argc > 1)
  {
    //setting for main program loop makes use of overloaded operator = on setting class
    running = false;

    for(ascalRuntime.getCLParams().index = 1; ascalRuntime.getCLParams().index < argc; ascalRuntime.getCLParams().index++)
    {
        arg = argv[ascalRuntime.getCLParams().index];
        //Auto execute if param specificies a .asl file
        if(arg.size() > 4)
        {
            if(arg[arg.size()-4] == '.' && ParsingUtil::toLower(arg[arg.size()-3]) == 'a' &&
                    ParsingUtil::toLower(arg[arg.size()-2]) == 's' && ParsingUtil::toLower(arg[arg.size()-1]) == 'l')
            {
            	//Exec code in filepath specified in command line parameter
            	ascalRuntime.execExpression(std::string("run:"+arg));
                continue;
            }
        }
        ascalRuntime.execExpression(arg);

    }
  }
  //End of section interpreting program parameters from command line
  //beginning of main program loop, if only one parameter is given in the
  //command line by the user loop will not run, unless that param was the loop command
  //by default the loop runs

  while(std::cin && running)
  {
          //Interpreter prompt to let user know program is expecting a command/expression
      std::cout<<std::endl<<">>";
      getline(std::cin, arg);
      arg = ParsingUtil::getExpr(arg, 0, std::cin).data;
        try{
        	ascalRuntime.execExpression(arg);
        }
        catch(std::string &exception)
        {
            std::cerr<<"Function call stack trace.\n";
            std::cerr<<exception<<std::endl;
            std::cerr<<"Failed to exec: "<<arg<<std::endl;
        }
  }
  }
  catch(int exitCode){
      return exitCode;
  }
  return 0;
}
