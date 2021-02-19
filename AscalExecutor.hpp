/*
 * Ascal.hpp
 *
 *  Created on: Jan 11, 2021
 *      Author: andrewrubinstein
 */

#ifndef ASCALEXECUTOR_HPP_
#define ASCALEXECUTOR_HPP_

#define DEBUG 1
#define THROWERRORS 1
#if DEBUG==1
#define LOG_DEBUG(x)  if(*boolsettings["d"]) {std::cout<<x<<std::endl;}
#else
#define LOG_DEBUG(x)
#endif

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
#include "AscalFrame.hpp"
#include "Object.hpp"
#include "setting.hpp"
#include "stack.hpp"
#include "Calculator.hpp"
#include "SubStr.hpp"
#include "ParsingUtil.hpp"
class Keyword;
struct CommandLineParams{
	char ** argv;
	int argc;
	int index;
};
class AscalExecutor {
private:
const size_t sizeofFrame = sizeof(AscalFrame<double>);
size_t allocated = 0, deallocated = 0;;
const std::string VERSION = "2.01";

uint32_t frameCount = 1;
uint32_t varCount = 0;

linkedStack<AscalFrame<double> *> *currentStack = nullptr;
//Interpreter hash map for system keywords
std::unordered_map
<std::string,
Keyword*> inputMapper;
std::map
<ObjectKey,
std::string (*)(AscalFrame<double>*,Object&)> objectActionMapper;

/////////////////////////////
//stacks!
//these stacks handle respecting priority, the can handle as many priority levels as you like
stack<double> savedOperands;
stack<char> savedOperators;
stack<double> processOperands;
stack<char> processOperators;
AscalFrame<double>* cachedRtnObject = nullptr;
public:
size_t rememberedFromMemoTableCount;
std::unordered_map<uint64_t,double> memoPad;
struct ColorRGBA {
	uint8_t r,g,b,a = 0;
	ColorRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a) : r(r), g(g), b(b), a(a) {}
	ColorRGBA(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b), a(0) {}
};
std::map<std::string, ColorRGBA> colorMap;
std::unordered_map<uint64_t,double> memoPad;
/////////////////////////////
//Program Global Memory Declaration
std::unordered_map<std::string,Object > memory;
CommandLineParams commandLineParams;
std::streambuf* stream_buffer_cin;
std::istream ascal_cin;
//Interpreter Settings HashMap for toggle flags, like show time, or operations
std::map<std::string,setting<bool> > boolsettings;
//list of previous expressions for u command in interpretParam fn
stack<std::string> lastExp;
//list of previous undone expressions for r command in interpretParam fn
stack<std::string> undoneExp;
std::vector<Object> userDefinedFunctions;
std::vector<Object> systemDefinedFunctions;
AscalFrame<double>* getCachedRtnObject()
{
	return this->cachedRtnObject;
}
void setCachedRtnObject(AscalFrame<double> *frame)
{
	this->cachedRtnObject = frame;
}
	//called in constructor
	void loadInitialFunctions();
	//End of called in constructor
	void addKeyWord(Keyword *key);

	double callOnFrame(AscalFrame<double>* callingFrame,const std::string &subExp);
	double callOnFrame(AscalFrame<double>* callingFrame,const std::string &&subExp);
	double callOnFrameFormatted(AscalFrame<double>* callingFrame,std::string subExp);
	double calculateExpression(AscalFrame<double>* frame);
	double processStack(stack<double> &operands,stack<char> &operators);
	void createFrame(linkedStack<AscalFrame<double>* > &executionStack, AscalFrame<double>* currentFrame, Object *obj, int i,uint64_t hash);
	void clearStackOnError(bool printStack, std::string &error, linkedStack<AscalFrame<double>* > &executionStack, AscalFrame<double>* currentFrame, AscalFrame<double>* frame);

	Object* resolveNextExprSafe(AscalFrame<double>* frame, SubStr &varName);
	Object* resolveNextObjectExpression(AscalFrame<double>* frame, SubStr &varName, Object *obj = nullptr);
	Object* resolveNextObjectExpressionPartial(AscalFrame<double>* frame, SubStr &varName, Object *obj = nullptr);
	Object& getObject(AscalFrame<double>* frame, std::string &functionName);
	Object* getObjectNoError(AscalFrame<double>* frame, std::string &functionName);

	template <typename t>
	void cleanOnError(bool timeInstruction, t start, t end);
	double calcWithOptions(AscalFrame<double>* frame);
	/////////////////////////////
	//UI handling functions
	/////////////////////////////
	double interpretParam(AscalFrame<double>* frame,bool);
	//End UI handling functions
	/////////////////////////////
	/////////////////////////////
	//helper functions
	/////////////////////////////
	void loadFn(Object function);
	template <typename t>
	void loadUserDefinedFn(Object &function, t &mem);
	void updateBoolSetting(AscalFrame<double>* frame);
	CommandLineParams& getCLParams()
	{
		return commandLineParams;
	}
	AscalExecutor(char** argv, int argc, int index, std::streambuf* streambuf): rememberedFromMemoTableCount(0), stream_buffer_cin(streambuf), ascal_cin(streambuf)
	{
		ascal_cin.rdbuf(streambuf);
		loadInitialFunctions();
		commandLineParams.argc = argc;
		commandLineParams.argv = argv;
		commandLineParams.index = 1;
		{
		setting<bool> set(
	            /*name*/
	                "Show Operations that the interpreter uses while executing code",
	            /*command line command*/
	                "o",
	            /*variable*/
	                false);

	        boolsettings[set.getCommand()] = set;

	        set = setting<bool> (
	                /*name*/
	                    "Auto print results of calculations",
	                /*command line command*/
	                    "p",
	                /*variable*/
	                    true);

	        boolsettings[set.getCommand()] = set;

	        set = setting<bool> (
	                /*name*/
	                    "Use scientific notation for output of numbers larger than 999,999",
	                /*command line command*/
	                    "sci",
	                /*variable*/
	                    true);

	        boolsettings[set.getCommand()] = set;
	        set = setting<bool>(
	            /*name*/
	                "Debug Ascal Mode",
	            /*command line command*/
	                "d",
	            /*variable*/
	                false);
	        boolsettings[set.getCommand()] = set;
	        set = setting<bool>(
	            /*name*/
	                "Keep Interpreter listening for input to stdin",
	            /*command line command*/
	                "l",
	            /*variable*/
	                true);
	        boolsettings[set.getCommand()] = set;
	        set = setting<bool>(
	            /*name*/
	                "Auto Memoize all function calls to improve multiple recursive function performance,\nwill cause erroneous calculations if not using pure mathematical functions.",
	            /*command line command*/
	                "memoize",
	            /*variable*/
	                false);
	        boolsettings[set.getCommand()] = set;
	        set = setting<bool>(
	            /*name*/
	                "Print time taken to run calculation",
	            /*command line command*/
	                "t",
	            /*variable*/
	                false);
	        boolsettings[set.getCommand()] = set;
	        }//bracket to remove set variable from program memory
	          /*
	           * End of initialization values in settings hashmap
	           * */
	}
	static std::string printMemory(std::map<std::string,Object*> &memory,std::string delimiter,bool justKey = true,
	        std::string secondDelimiter = "\n")
	{
	    std::string s;
	    if(justKey)
	        for(auto &[key,value]:memory)
	            s+=key+delimiter;
	    else
	        for(auto &[key,value]:memory)
	            s+=key+delimiter+value->instructionsToString()+secondDelimiter;
	    return s.substr(0,s.size()-secondDelimiter.size());
	}
	static std::string printMemory(std::unordered_map<std::string,Object> &memory,std::string delimiter,bool justKey = true,
	        std::string secondDelimiter = "\n")
	{
	    std::string s;
	    if(justKey)
	        for(auto &[key,value]:memory)
	            s+=key+delimiter;
	    else
	        for(auto &[key,value]:memory){
	            std::string instr = value.instructionsToString();
	            s+=key+delimiter+instr+secondDelimiter;
	        }
	    return s.substr(0,s.size()-secondDelimiter.size());
	}
	~AscalExecutor();
};

#endif /* ASCAL_HPP_ */
