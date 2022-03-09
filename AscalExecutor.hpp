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
#include "Object.hpp"
#include "setting.hpp"
#include "stack.hpp"
#include "Calculator.hpp"
#include "SubStr.hpp"
#include "ParsingUtil.hpp"
#include "string_view.hpp"
#include "MemoryMap.hpp"


template <typename t>
class AscalFrame;
template <typename t>
class FunctionFrame;
template <typename t>
class ParamFrame;
template <typename t>
class ParamFrameFunctionPointer;
class Keyword;
struct expressionResolution {
    Object *data = nullptr;
    Object *parent = nullptr;
    uint_fast32_t listIndex = -1;
    bool error = true;
};
struct CommandLineParams{
	char ** argv;
	int argc;
	int index;
};
class AscalExecutor {
private:
const size_t sizeofFrame = 0;
const std::string VERSION = "2.01";

uint32_t frameCount = 1;
uint32_t varCount = 0;

StackSegment<AscalFrame<double> *> *currentStack = nullptr;
//Interpreter hash map for system keywords
std::unordered_map<string_view, Keyword*> inputMapper;


/////////////////////////////
size_t rememberedFromMemoTableCount;
//stacks!
//these stacks handle respecting priority, the can handle as many priority levels as you like
stack<double> savedOperands;
stack<char> savedOperators;
stack<double> processOperands;
stack<char> processOperators;
//private stack frame shred memory
stack<char> instructionStack;
//end stack frame shred memory
std::unordered_map<uint64_t,double> memoPad;
AscalFrame<double>* cachedRtnObject = nullptr;
    boost::object_pool<ParamFrame<double> > pFramePool;
    boost::object_pool<ParamFrameFunctionPointer<double> > fpFramePool;
    boost::object_pool<FunctionFrame<double> > fFramePool;

    ParsedStatementList paramsBuffer;
public:
/////////////////////////////
//Program Global Memory Declaration
stack<double> operands;
stack<char> operators;
stack<char> instructionsStack;
stack<Object* > paramsStack;
MemoryManager memMan;
MemoryMap memory;
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
void deleteFrame(AscalFrame<double> *frame);
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
	double callOnFrame(AscalFrame<double>* callingFrame,const string_view subExp);
	double callOnFrameFormatted(AscalFrame<double>* callingFrame,std::string subExp);
	double calculateExpression(AscalFrame<double>* frame);
	double processStack(stack<double> &operands,stack<char> &operators);
    void createFrame(StackSegment<AscalFrame<double>* > &executionStack, AscalFrame<double>* currentFrame, Object *obj, ParsedStatementList &params, int i,uint64_t hash);
	void clearStackOnError(bool printStack, std::string &error, StackSegment<AscalFrame<double>* > &executionStack, AscalFrame<double>* currentFrame, AscalFrame<double>* frame);

	Object* resolveNextExprSafe(AscalFrame<double>* frame, SubStrSV varName);
    expressionResolution resolveNextObjectExpression(AscalFrame<double>* frame, SubStrSV &varName, Object *obj = nullptr);
	Object* resolveNextObjectExpressionPartial(AscalFrame<double>* frame, SubStrSV &varName, Object *obj = nullptr);
	Object& getObject(AscalFrame<double>* frame, string_view functionName);
	Object* getObjectNoError(AscalFrame<double>* frame, string_view functionName);

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
	Object& loadUserDefinedFn(Object &function, MemoryMap &mem);
	Object& loadUserDefinedFn(Object &function, std::map<string_view, Object*> &mem);
	void updateBoolSetting(AscalFrame<double>* frame);
	CommandLineParams& getCLParams()
	{
		return commandLineParams;
	}
    AscalExecutor(char** argv, int argc, int index, std::streambuf* streambuf);
	template <typename string1, typename string2>
	static std::string printMemory(std::map<string1,Object*> &memory,string2 delimiter,bool justKey = true,
	        std::string secondDelimiter = "\n")
	{
	    std::string s;
	    if(justKey)
	        for(auto &[key,value]:memory)
	            s+=key+delimiter;
	    else
	        for(auto &[key,value]:memory)
	            s+=key+delimiter+value->instructionsToFormattedString()+secondDelimiter;
	    return s.substr(0,s.size()-secondDelimiter.size());
	}
	template <typename string1, typename string2>
	static std::string printMemory(std::unordered_map<string1,Object> &memory,string2 delimiter,bool justKey = true,
	        std::string secondDelimiter = "\n")
	{
	    std::string s;
	    if(justKey)
	        for(auto &[key,value]:memory)
	            s+=key+delimiter;
	    else
	        for(auto &[key,value]:memory){
	            std::string instr = value.instructionsToFormattedString();
	            s+=key+delimiter+instr+secondDelimiter;
	        }
	    return s.substr(0,s.size()-secondDelimiter.size());
	}
	~AscalExecutor();
};

#endif /* ASCAL_HPP_ */
