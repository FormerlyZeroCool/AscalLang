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
#include "HashMap.hpp"
#include "MemoryMap.hpp"

template <typename t>
class AscalFrame;
class Keyword;
struct StackDataRecord;
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


/////////////////////////////
size_t rememberedFromMemoTableCount;
//end stack frame shred memory
std::unordered_map<uint64_t,double> memoPad;
AscalFrame<double>* cachedRtnObject = nullptr;
public:
ParsedStatementList paramsBuffer;
//Interpreter hash map for system keywords
FlatMap<string_view, Keyword*> inputMapper;
Keyword* getKeywordFromPtrToOpcode(uint8_t *start)
{
	Keyword* opcode;
    memcpy(&opcode, start, sizeof(Keyword*));
    //std::cout<<"op code parsed from bin: "<<opCode<<"\n";
    return opcode;
}
static inline const uint8_t KEYWORD_IDENTIFIER = 128, DOUBLE = 129, OPERATOR = 130, VARIABLE = 131, DELIMITER = 132, ARRAY_OBJ = 133;

ObjectPool<AscalFrame<double> > framePool;
stack<AscalFrame<double>* > frameStack;
/////////////////////////////
//Program Global Memory Declaration
struct Operand {
	private:
	union rec{ double number; Object* obj; constexpr rec(double d):number(d){} constexpr rec(Object* obj): obj(obj) {}} data;
	public:
	constexpr Operand(double num): data(num) {}
	constexpr Operand(Object* object): data(object) { }
	constexpr Operand(): data(0.0) {}
	constexpr double& number() noexcept
	{
		return data.number;
	}
	constexpr Object*& object() noexcept
	{
		return data.obj;
	}
};
stack<Operand> operands;
stack<StackDataRecord> dataStack;
MemoryManager memMan;
FlatMap<string_view, Object*> memory;
CommandLineParams commandLineParams;
std::streambuf* stream_buffer_cin;
std::istream ascal_cin;
//Interpreter Settings FlatMap for toggle flags, like show time, or operations
FlatMap<std::string,setting<bool> > boolsettings;
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

	double callOnFrameFormatted(AscalFrame<double>* callingFrame,std::string subExp);
	double calculateExpression(AscalFrame<double>* frame);
    void createFrame(StackSegment<AscalFrame<double>* > &executionStack, AscalFrame<double>* currentFrame, Object *obj, ParsedStatementList &params, int i,uint64_t hash);
	void clearStackOnError(bool printStack, std::string &error, stack<AscalFrame<double>* > &executionStack, AscalFrame<double>* currentFrame, AscalFrame<double>* frame);

	Object* resolveNextExprSafe(AscalFrame<double>* frame, SubStrSV varName);
    expressionResolution resolveNextObjectExpression(AscalFrame<double>* frame, SubStrSV &varName, Object *obj = nullptr);
	Object* resolveNextObjectExpressionPartial(AscalFrame<double>* frame, SubStrSV &varName, Object *obj = nullptr);
	Object& getObject(AscalFrame<double>* frame, string_view functionName);
	Object* getObjectNoError(AscalFrame<double>* frame, string_view functionName);

	template <typename STRING>
	constexpr void log(const STRING& var)
	{
		#ifdef debug
		if(*this->boolsettings["o"])
		std::cout<<var<<"\n";
		#endif
	}
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
	void makeArray(Object &obj)
	{
        Object pushMethod(this->memMan, string_view("push"), string_view("arrPush(this,numberzxa)"));
        pushMethod.compileInstructions(*this);
        Object lengthMethod(this->memMan, string_view("length"), string_view("arrLen(this)"));
        pushMethod.compileInstructions(*this);
        Object popMethod(this->memMan, string_view("pop"), string_view("arrErase(this,arrLen(this)-1)"));
        popMethod.compileInstructions(*this);
        Object eraseMethod(this->memMan, string_view("erase"), string_view("arrErase(this,index)"));
        popMethod.compileInstructions(*this);
        obj.loadChild(pushMethod, *this);
        obj.loadChild(lengthMethod, *this);
        obj.loadChild(popMethod, *this);
        obj.loadChild(eraseMethod, *this);
	}
	void makeString(Object &obj)
	{
		this->makeArray(obj);
		//substr
        //Object pushMethod(this->memMan, string_view("push"), string_view("arrPush(this,numberzxa)"));
        //pushMethod.compileInstructions();
        //obj.loadChild(pushMethod, *this);
	}
	Object& loadFn(Object function);
	Object& loadUserDefinedFn(Object &&function, FlatMap<string_view, Object*> &mem);
	Object& loadUserDefinedFn(Object &function, FlatMap<string_view, Object*> &mem);
	Object& loadUserDefinedFn(Object &function, MemoryMap &mem);
	Object& loadUserDefinedFn(Object &function, Map<string_view, Object*> &mem);
	void updateBoolSetting(std::string &&name);
	CommandLineParams& getCLParams()
	{
		return commandLineParams;
	}
    AscalExecutor(char** argv, int argc, int index, std::streambuf* streambuf);
	template <typename string1, typename string2>
	static std::string printMemory(Map<string1,Object*> &memory,string2 delimiter,bool justKey = true,
	        std::string secondDelimiter = "\n")
	{
	    std::string s;
		/*
	    if(justKey)
	        for(auto &[key,value]:memory)
	            s+=key+delimiter;
	    else
	        for(auto &[key,value]:memory)
	            s+=key+delimiter+value->instructionsToFormattedString()+secondDelimiter;*/
	    return s.substr(0,s.size()-secondDelimiter.size());
	}
	template <typename string1, typename string2>
	static std::string printMemory(FlatMap<string1,Object> &memory,string2 delimiter,bool justKey = true,
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
