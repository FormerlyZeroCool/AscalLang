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
#include <string>
#include <memory>
#include "Ascal.hpp"
#include <readline/readline.h>
#include <readline/history.h>

#define DEBUG 1
#define THROWERRORS 1

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
  //Beginning of section interpreting program parameters from command line
	  bool running = true;
  char* readLineBuffer;
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
  //beginning of main program loop, if parameters are given in the
  //command line by the user loop will not run
  //by default the loop runs
  std::string lastLine = "";
  while(std::cin && running)
  {
    //this can be replaced with std::cout<<">>"; getline(std::cin, arg);
    //if you then remove header files for readline this will compile without dependencies, or just use AscalMainNoDep.cpp
      if((readLineBuffer = readline(">>")) == nullptr)
        break;
      auto bufCleaner = std::make_unique<char*>(readLineBuffer);
      
      std::string line(readLineBuffer);
      if (line.size() > 0 && line != lastLine) {
        add_history(readLineBuffer);
        lastLine = line;
      }
      //get expression from line parsed from std in,
      //If a codeblock is unclosed it will continue reading from std in until it sees a closing brace to the codeblock }
      	if(ParsingUtil::firstChar(line, '{'))
        line = ParsingUtil::getExpr(line, 0, std::cin).data;
        try{
        	ascalRuntime.execExpression(line);
        }
        catch(std::string &exception)
        {
            std::cerr<<"Function call stack trace.\n";
            std::cerr<<exception<<std::endl;
            std::cerr<<"Failed to exec: "<<line<<std::endl;
        }

    }
  }
  catch(int exitCode){
      return exitCode;
  }
  return 0;
}
