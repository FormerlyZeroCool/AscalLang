//============================================================================
// Name        : Ascal.cpp
// Author      : Andrew Rubinstein
// Version     : v1
// Copyright   :
// Description : An infix expression interpreter, and simple math programming language in C++
// A.S.Cal.
// Andrew's Simple Calculator Language
//============================================================================
//On Line 46 there is a Macro defining a debug print function LOG_DEBUG that only will compile
//when the DEBUG Macro is equal to 1


#include <unordered_map>
#include <map>
#include <chrono>
#include <ctime>
#include <string>
#include <iostream>
#include <fstream>

#include "stack.h"
#include "setting.h"
#include "Object.h"
#include "Vect2D.h"

std::streambuf* stream_buffer_cin = std::cin.rdbuf();
/////////////////////////////
//Program Global Memory Declaration
static std::unordered_map<std::string,Object> memory;
std::vector<Object> userDefinedFunctions;
std::vector<Object> systemDefinedFunctions;
//Interpreter Settings HashMap for toggle flags, like show time, or operations
static std::map<std::string,setting<bool> > boolsettings;
//Interpreter hash map for system keywords
//template <class t>
static std::unordered_map
<std::string,
std::string (*)(
		const std::string&,std::unordered_map<std::string,Object>&,AscalParameters &,
			std::map<std::string,Object>&,bool)> inputMapper;

template <class t>
static std::map<char,t (*)(t&,t&)> operations;
//End Program Global Memory Declaration
/////////////////////////////



#define DEBUG 1
#define THROWERRORS 1

#if DEBUG==1
#define LOG_DEBUG(x)  if(*boolsettings["d"]) {std::cout<<x<<std::endl;}
#else
#define LOG_DEBUG(x)
#endif
//Parallel reimann sums
//thread count = number of partitions/20 + 1
//dynamic array as long as thread count passed to each thread, along with thread index number to save result of calculation
//when all threads are finished running sum up result in array, maybe use simd if

const std::string MAX = std::to_string(std::numeric_limits<double>::max());
//////////////////////////////////////////////////////////
//Start Ascal System Defined  Keyword functionality Executed by lookup in inputMapper unordered_map

void printVar(const std::string &expr,bool saveLast);
void printHelpMessage(const std::string &expr);
void updateBoolSetting(const std::string &expr);

std::string importAction(const std::string & expr,std::unordered_map<std::string,Object>& localMemory,
		AscalParameters &params, std::map<std::string,Object> &paramMemory,bool s);
std::string setAction(const std::string& expr,std::unordered_map<std::string,Object>& localMemory,
		AscalParameters &params, std::map<std::string,Object> &paramMemory,bool saveLast);
std::string whileAction(const std::string &expr,std::unordered_map<std::string,Object>& localMemory,
		AscalParameters &params,std::map<std::string,Object> &paramMemory,bool saveLast);
std::string plotAction(const std::string &expr,std::unordered_map<std::string,Object>& localMemory,
		AscalParameters &params,std::map<std::string,Object> &paramMemory,bool saveLast);
std::string quitAction(const std::string & expr,std::unordered_map<std::string,Object>& localMemory,
		AscalParameters &params, std::map<std::string,Object> &paramMemory,bool s);
std::string whenAction(const std::string &expr,std::unordered_map<std::string,Object>& localMemory,
		AscalParameters &params, std::map<std::string,Object> &paramMemory,bool saveLast);
std::string clocNewVar(const std::string &exp,std::unordered_map<std::string,Object>& localMemory,
		AscalParameters &params, std::map<std::string,Object> &paramMemory,bool saveLast);
std::string locNewVar(const std::string &exp,std::unordered_map<std::string,Object>& localMemory,
		AscalParameters &params, std::map<std::string,Object> &paramMemory,bool saveLast);
std::string printCommand(const std::string &expr,std::unordered_map<std::string,Object>& localMemory,
		AscalParameters &params, std::map<std::string,Object> &paramMemory,bool saveLast);
std::string printCalculation(const std::string &expr,std::unordered_map<std::string,Object>& localMemory,
		AscalParameters &params, std::map<std::string,Object> &paramMemory,bool saveLast);
std::string constNewVar(const std::string &expr,std::unordered_map<std::string,Object>& localMemory,
		AscalParameters &params, std::map<std::string,Object> &paramMemory,bool saveLast);
std::string letNewVar(const std::string &expr,std::unordered_map<std::string,Object>& localMemory,
		AscalParameters &params, std::map<std::string,Object> &paramMemory,bool saveLast);
std::string deleteObject(const std::string& expr,std::unordered_map<std::string,Object>& localMemory,
		AscalParameters &params, std::map<std::string,Object> &paramMemory,bool saveLast);
std::string undoAction(const std::string & expr,std::unordered_map<std::string,Object>& localMemory,
		AscalParameters &params, std::map<std::string,Object> &paramMemory,bool s);
std::string redoAction(const std::string & expr,std::unordered_map<std::string,Object>& localMemory,
		AscalParameters &params, std::map<std::string,Object> &paramMemory,bool s);
std::string showOpBoolSetting(const std::string & expr,std::unordered_map<std::string,Object>& localMemory,
		AscalParameters &params, std::map<std::string,Object> &paramMemory,bool s);
std::string loopBoolSetting(const std::string & expr,std::unordered_map<std::string,Object>& localMemory,
		AscalParameters &params, std::map<std::string,Object> &paramMemory,bool s);
std::string printBoolSetting(const std::string & expr,std::unordered_map<std::string,Object>& localMemory,
		AscalParameters &params, std::map<std::string,Object> &paramMemory,bool s);
std::string debugBoolSetting(const std::string & expr,std::unordered_map<std::string,Object>& localMemory,
		AscalParameters &params, std::map<std::string,Object> &paramMemory,bool s);
std::string timeToRunBoolSetting(const std::string & expr,std::unordered_map<std::string,Object>& localMemory,
		AscalParameters &params, std::map<std::string,Object> &paramMemory,bool s);
//End Ascal System Defined  Keyword functionality
//////////////////////////////////////////////////////////

/////////////////////////////
//Start char/string interpreters
int getPriority(char ator);
bool isOperator(char s);
bool isNonParentheticalOperator(char s);
bool isNumeric(char c);
bool cmpstr(const std::string &s1,const std::string &s2);
bool hasChar(const std::string &dat,const char &c);
//End char/string interpreters
/////////////////////////////


/////////////////////////////
//Logic
/////////////////////////////

////////////////////////////
//Start Parsing For Loading Ascal System Memory
struct SubStr{
	std::string data;
	int start,end;
	SubStr(std::string data,int start,int end):data(data),start(start),end(end){}
};
SubStr getVarName(const std::string &s,int index);
int getClosingIndex(char opening,const std::string &data);
SubStr getExpr(const std::string &data,int startingIndex);
SubStr getNewVarName(const std::string &data);
//End Parsing For Loading Ascal System Memory
////////////////////////////
//Start Operations mapper
template <class t>
void initOperations();
//End Operations mapper
////////////////////////////
//regular logic
template <class t>
t calculateExpression(std::string exp,AscalParameters &,std::map<std::string,Object>&,std::unordered_map<std::string,Object> &localMemory);
template <class t>
t calc(char op,t and1,t and2);
template <class t>
t processStack(stack<t> &operands,stack<char> &operators);
template <class t>
t pow(t a,t b)
{
	t result = 1;
	for(int i = 0;i<b;i++)
	{
		result *= a;
	}
	return result;
}
template <class t>
t root(t b,t p)
{
	t result = 100;
	if(b>0)
	{
		t base = b,one = 1;
		t newPow = p-one;
		t den;
		t num;
		t delta =  1,prev;
		int count = 0;
		while(delta > 0.000001 && count < 1000)
		{
			num = pow(result,p) - base;
			den = pow(result,newPow)*p;
			prev = result;
			result = (result*den - num)/den;
			delta = result - prev;
			if(delta < 0)
				delta *= -1;

			count++;
		}
	}
	else
		result = -10000000000000000000000000000000000000000.0786;
	//std::cout<<"to do radical: "<<count<<std::endl;
	return result;
}
//regular logic
////////////////////////////
//string interpreters/Logic
template <class t>
t getNextInt(std::string data,int &index);
double getNextDouble(const std::string &data,int &index);
//string interpreters/Logic
/////////////////////////////
/////////////////////////////
//UI handling functions
/////////////////////////////
double interpretParam(std::string &expr,std::unordered_map<std::string,Object> &localMemory,std::map<std::string,Object>& paramMemory,bool);
double calcWithOptions(std::string,std::unordered_map<std::string,Object> &localMemory,AscalParameters &params, std::map<std::string,Object> &paramMemory);
//End UI handling functions
/////////////////////////////

//list of previous expressions for u command in interpretParam fn
static stack<std::string> lastExp;
//list of previous undone expressions for r command in interpretParam fn
static stack<std::string> undoneExp;
/////////////////////////////

void printLoadedMemMessage(Object function)
{
	std::cout<<"Loaded Function: "<<function.id<<"\nexpression: "<<function.instructionsToString()<<std::endl<<std::endl;
}
bool containsOperator(std::string s)
{
	bool contains = false;
	int i = 0;
	while(!contains && s[i])
	{
		if(isOperator(s[i])){
			contains = true;
		}
		i++;
	}
	return contains;
}
void loadFn(Object function)
{
		memory[function.id] = function;
		systemDefinedFunctions.push_back(function);
}
void loadUserDefinedFn(Object function)
{


	if(!containsOperator(function.id))
	{
		memory[function.id] = function;
		userDefinedFunctions.push_back(function);
	}
	else
	{
		std::cout<<"Error loading "<<function.id<<" due to the use of an operator in its name"<<std::endl;
		std::cout<<"Operators: "<< '=' <<','<< '>' << ',' << '<' <<','<< '$' <<','<<
	    		'P' <<','<< '@' <<','<< '+' <<','<< '-' <<','<<
				'*'<<','<< '/' <<','<< '^' <<','<< '%' <<','<< 'C';
	}
	if(*boolsettings["p"])
		printLoadedMemMessage(function);
}
void loadInitialFunctions()
{
	//Unary boolean Operations
	Object notTrue("not","value=0","");
	loadFn(notTrue);
	Object isTrue("true","value=0=0)","");
	loadFn(isTrue);

	//Calculus Functions
	Object fPrime("fprime","x*0+(f(x+0.000001)-f(x))/0.000001","");
	loadFn(fPrime);

	//Trig Functions
	Object sinD("sinD","sin(toRad(theta))","");
	loadFn(sinD);
	Object sin("sin","notSin(theta%(2*pi))","");
	loadFn(sin);
	Object notSin("notSin","theta-theta^3/6+theta^5/120-theta^7/5040+theta^9/362800-theta^11/39916800+theta^13/6227020800-theta^15/(15P15)","");
	loadFn(notSin);
	Object csc("csc","1/sin(theta)","");
	loadFn(csc);
	Object cosD("cosD","cos(toRad(theta))","");
	loadFn(cosD);
	Object cos("cos","sin(theta+pi/2)","");
	loadFn(cos);
	Object sec("sec","1/cos(theta)","");
	loadFn(sec);
	Object tan("tan","sin(theta)/cos(theta)","");
	loadFn(tan);
	//Helpful functions
	Object fib("fib","loc counter = 0;loc first = 0;loc second = 1;loc int = 0;while counter<x{set int = second;set second = second+first;set first = int;set counter = counter+1;};first","");
	loadFn(fib);
	Object fibr("fibr","when x > 1 then fibr(x-1)+fibr(x-2) else x end","");
	loadFn(fibr);
	Object gcd("gcd","a*b*0+ when b=0 then a when a=0=0 then gcd(b,a%b) end","");
	loadFn(gcd);
	Object sumBetween("sumBetween","0*numberzxa*numberzxb + "
			"when (numberzxb<numberzxa)+(numberzxb=numberzxa) then sumOneTo(numberzxa)-sumOneTo(numberzxb)"
			"when (numberzxb>numberzxa) then sumOneTo(numberzxb)-sumOneTo(numberzxa) end"
			,"");
	loadFn(sumBetween);
	Object sumOneTo("sumOneTo","(numberzxa(numberzxa+1))/2","");
	loadFn(sumOneTo);
	//factorial of >= 171 overflows double datatype
	Object rfact("rfact",
			"when (i>1)*(i<171) then rfact(i-1)*i when not(i>1) then 1 else 0 end","");
	loadFn(rfact);
	Object absoluteValue("abs","when numberx<0 then numberx*-1 else numberx end","");
	loadFn(absoluteValue);
	Object ln("ln","when argument>0 then e@argument else "+MAX+" end","");
	loadFn(ln);
	Object log("log","when argument>0 then 10@argument else "+MAX+" end","");
	loadFn(log);
	Object logbx("logbx","base@argument","");
	loadFn(logbx);
	Object sqrt("sqrt","2$radicand","");
	loadFn(sqrt);
	Object factorial("fact","when numberzxa<171 then numberzxaPnumberzxa when not(numberzxa<171) then 0 end","");
	loadFn(factorial);
	Object dist("dist","2$((dx)^2+(dy)^2)","");
	loadFn(dist);
	Object toDeg("toDeg","rad*180/pi","");
	loadFn(toDeg);
	Object toRad("toRad","deg*pi/180","");
	loadFn(toRad);

	//Stats Functions
	Object binProbDist("binprob","(total C events) * probabilityOfSuccess^events * (1-probabilityOfSuccess)^(total-events)","");
	loadFn(binProbDist);

	//Dietician Functions
	Object eKCal("ekcal","(0*male*kg*cm*age*activity+ true(male)*5+not(male)*-161+(10*kg)+(6.25*cm)-(5*age))*activity","");
	loadFn(eKCal);
	//Object desWeight("desw","(percent/100)*(usual-desired)+desired","");
	//loadFn(desWeight);

	//Constants Definition
	Object pi("pi","3.14159265359","");
	loadFn(pi);
	Object e("e","2.718281828459045","");
	loadFn(e);
	Object inf("nf",MAX+"2","");
	loadFn(inf);
	Object null("null",MAX,"");
	loadFn(null);
}
void printAllFunctions()
{
	std::cout<<"All Functions in Memory:"<<std::endl;
	for(auto &[key,value] : memory)
	{
		std::cout<<std::endl<<"Function Name: "<<key<<std::endl<<"Function Expression: "<<value.instructionsToString()<<std::endl;
	}
	std::cout<<std::endl<<"End of All Functions in Memory."<<std::endl;
}
void printAllUDF()
{
	std::cout<<"User Defined Functions:"<<std::endl;
	for(Object & data:userDefinedFunctions)
	{
		std::cout<<std::endl<<"Function Name: "<<data.id<<std::endl<<"Function Expression: "<<data.instructionsToString()<<std::endl;
	}
	std::cout<<std::endl<<"End of User Defined Functions."<<std::endl;
}
void printAllSDF()
{
	std::cout<<"System Defined Functions:"<<std::endl;
	for(Object & data:systemDefinedFunctions)
	{
		std::cout<<std::endl<<"Function Name: "<<data.id<<std::endl<<"Function Expression: "<<data.instructionsToString()<<std::endl;
	}
	std::cout<<std::endl<<"End of System Defined Functions."<<std::endl;
}

void initParamMapper()
{
	inputMapper["import"] = importAction;
	inputMapper["while"] = whileAction;
	inputMapper["when"] = whenAction;
	inputMapper["plot"] = plotAction;
	inputMapper["quit"] = quitAction;
	inputMapper["print"] = printCommand;
	inputMapper["set"] = setAction;
	inputMapper["const"] = constNewVar;
	inputMapper["let"] = letNewVar;
	inputMapper["loc"] = locNewVar;
	inputMapper["cloc"] = clocNewVar;
	inputMapper["delete"] = deleteObject;
	inputMapper["u"] = undoAction;
	inputMapper["r"] = redoAction;
	inputMapper["-u"] = undoAction;
	inputMapper["-r"] = redoAction;
	inputMapper["l"] = loopBoolSetting;
	inputMapper["o"] = showOpBoolSetting;
	inputMapper["p"] = printBoolSetting;
	inputMapper["t"] = timeToRunBoolSetting;
#if DEBUG == 1
	inputMapper["d"] = debugBoolSetting;
#endif
}
char toLower(char data)
{
	if(data < 96)
		data +=32;
	return data;
}
int main(int argc,char* argv[])
{
  /*
   * Initializing values in system hashmaps
   * */
  {
	  //initializes operations hashmap that maps operators like + to the appropriate function
	  initOperations<double>();
	  //initializes inputMapper hashmap for user input
	  initParamMapper();
	  //Load Standard Library of functions fo Ascal
	  loadInitialFunctions();
	  setting<bool> set(
    		/*name*/
    			"Show Operations",
    		/*command line command*/
    			"o",
    		/*variable*/
    			false);

    	boolsettings[set.getCommand()] = set;

        set = setting<bool> (
        		/*name*/
        			"Print all expressions results",
        		/*command line command*/
        			"p",
        		/*variable*/
        			true);

	    boolsettings[set.getCommand()] = set;
    	set = setting<bool>(
    		/*name*/
    			"Debug Mode",
    		/*command line command*/
    			"d",
    		/*variable*/
    			false);
    	boolsettings[set.getCommand()] = set;
    	set = setting<bool>(
    		/*name*/
    			"Loop calculator",
    		/*command line command*/
    			"l",
    		/*variable*/
    			true);
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
  //Variable to save user defined expressions to be run
  std::string expr;
  //Beginning of section interpreting program parameters from command line
  if(argc > 1)
  {
	//setting for main program loop makes use of overloaded operator = on setting class
	boolsettings["l"] = false;
	//Set Expression to be calculated with console supplied parameter
    expr = argv[1];
    std::string arg = "";

    for(int i = 1;i<argc;i++)
    {
    	std::unordered_map<std::string,Object> localMemory;
    	std::map<std::string,Object> paramMemory;
    	arg = argv[i];
        if(arg.size() > 6)
        {
        	if(arg[arg.size()-6] == '.' && toLower(arg[arg.size()-5]) == 'a' &&
        			toLower(arg[arg.size()-4]) == 's' && toLower(arg[arg.size()-3]) == 'c' &&
					toLower(arg[arg.size()-2]) == 'a' && toLower(arg[arg.size()-1]) == 'l')
        	{
        		std::cout<<"Loading file: "<<arg<<std::endl;
        		arg = "";
        	}
        }
    	try{
    	interpretParam(arg,localMemory,paramMemory,true);
	}
	catch(std::string &exception)
	{
		std::cout<<exception<<std::endl;
		std::cout<<"Failed to exec: "<<arg<<std::endl;
	}
    }
  }
  //End of section interpreting program parameters from command line
  //beginning of main program loop, if only one parameter is given in the
  //command line by the user loop will not run, unless that param was the loop command
  //by default the loop runs
  do
  {
	  //runs the calculation using the global variable expr
	  //this function also infers the datatype of the expression based on
	  //if there is a . present in the string it will use doubles else long


	  if(*boolsettings["l"])
	  {
		  //Interpreter prompt to let user know program is expecting a command/expression
		  std::cout<<std::endl<<">>";
		  getline(std::cin, expr);

	    	std::unordered_map<std::string,Object> localMemory;
	    	std::map<std::string,Object> paramMemory;
	    	try{
		  interpretParam(expr,localMemory,paramMemory,true);
		}
		catch(std::string &exception)
		{
			std::cout<<exception<<std::endl;
			std::cout<<"Failed to exec: "<<expr<<std::endl;
		}

	  }
  } while(*boolsettings["l"]);
  if(undoneExp.length() > 1 || lastExp.length()>1)
  {
	  std::cout<<std::endl<<"Ascal exited"<<std::endl;
  }
  return 0;
}
std::string importAction(const std::string & expr,std::unordered_map<std::string,Object>& localMemory,
		AscalParameters &params, std::map<std::string,Object> &paramMemory,bool s)
{
		std::string line;
		std::ifstream inputFile;
		int startIndex = 7;
		while(expr[startIndex] == ' ')
			startIndex++;
		std::string filePath = expr.substr(startIndex,expr.find(';')-startIndex);
		inputFile.open(filePath);
		if(!inputFile)
		{
			inputFile.open(filePath+".asl");
		}
		if(!inputFile)
		{
			throw std::string("Malformed path: "+filePath);
		}
		getline(inputFile, line);
		std::cin.rdbuf(inputFile.rdbuf());
		bool print = *boolsettings["p"];
		*boolsettings["p"] = false;
		bool timeCalc = *boolsettings["t"];
		*boolsettings["t"] = false;
		while(inputFile)
		{
			std::unordered_map<std::string,Object> calledLocalMemory;
	    	std::map<std::string,Object> calledParamMemory;

	    	try{

	    		interpretParam(line,calledLocalMemory,calledParamMemory,false);
	    	}
	    	catch(std::string &exception)
	    	{
	    		std::cout<<exception<<std::endl;
	    		std::cout<<"Failed to exec: "<<expr<<std::endl;
	    	}
			getline(inputFile, line);
		}
		std::cin.rdbuf(stream_buffer_cin);

		*boolsettings["p"] = print;
		*boolsettings["t"] = timeCalc;
		return MAX;
}
/*
std::string derivativeAction(const std::string &expr,std::unordered_map<std::string,Object>& localMemory,
		AscalParameters &params, std::map<std::string,Object> &paramMemory,bool s)
{

}*/
std::string showOpBoolSetting(const std::string & expr,std::unordered_map<std::string,Object>& localMemory,
		AscalParameters &params, std::map<std::string,Object> &paramMemory,bool s)
{
	updateBoolSetting(expr);
	return MAX;
}
std::string loopBoolSetting(const std::string & expr,std::unordered_map<std::string,Object>& localMemory,
		AscalParameters &params, std::map<std::string,Object> &paramMemory,bool s)
{
	updateBoolSetting(expr);
	return MAX;
}
std::string printBoolSetting(const std::string & expr,std::unordered_map<std::string,Object>& localMemory,
		AscalParameters &params, std::map<std::string,Object> &paramMemory,bool s)
{
	int index = 0;
	updateBoolSetting(getVarName(expr,index).data);
	return MAX;
}
std::string debugBoolSetting(const std::string & expr,std::unordered_map<std::string,Object>& localMemory,
		AscalParameters &params, std::map<std::string,Object> &paramMemory,bool s)
{
	updateBoolSetting(expr);
	return MAX;
}

std::string timeToRunBoolSetting(const std::string & expr,std::unordered_map<std::string,Object>& localMemory,
		AscalParameters &params, std::map<std::string,Object> &paramMemory,bool s)
{
	updateBoolSetting(expr);
	return MAX;
}
std::string quitAction(const std::string & expr,std::unordered_map<std::string,Object>& localMemory,
		AscalParameters &params, std::map<std::string,Object> &paramMemory,bool s)
{
	if(*boolsettings["p"])
		std::cout<<"Quitting Ascal, have a nice day!"<<std::endl;
	*boolsettings["l"] = false;
	return MAX;
}
std::string redoAction(const std::string & expr,std::unordered_map<std::string,Object>& localMemory,
		AscalParameters &params, std::map<std::string,Object> &paramMemory,bool s)
{
	std::string value = "0";
	if(!undoneExp.isEmpty()){
		std::string last = "";
		undoneExp.top(last);
		undoneExp.pop();
		lastExp.push(last);
		std::cout<<last<<std::endl;
    	std::unordered_map<std::string,Object> localMemory;
		value = std::to_string(interpretParam(last,localMemory,paramMemory,false));
		*boolsettings["p"] = false;
	}
	else
		std::cout<<"No statements can be redone"<<std::endl;
	return value;
}
std::string undoAction(const std::string & expr,std::unordered_map<std::string,Object>& localMemory,
		AscalParameters &params, std::map<std::string,Object> &paramMemory,bool s)
{
	double value = 0;
	if(!lastExp.isEmpty()){
		std::string last = "";
		lastExp.top(last);
		lastExp.pop();
		undoneExp.push(last);
		std::cout<<last<<std::endl;
    	std::unordered_map<std::string,Object> localMemory;
		value = interpretParam(last,localMemory,paramMemory,false);
		*boolsettings["p"] = false;
	}
	else
		std::cout<<"No previous statements"<<std::endl;
	return std::to_string(value);
}
std::string deleteObject(const std::string& expr,std::unordered_map<std::string,Object>& localMemory,
		AscalParameters &params, std::map<std::string,Object> &paramMemory,bool saveLast)
{

	if(cmpstr(expr.substr(7,3),"all"))
	{
		memory.clear();
		localMemory.clear();
		userDefinedFunctions.clear();
		std::cout<<"All Memory cleared."<<std::endl;
	}
	else
	{
		std::string name = getVarName(expr,7).data;

		if(memory.count(name) > 0)
		{

			std::vector<Object>::iterator position = std::find(userDefinedFunctions.begin(), userDefinedFunctions.end(), memory[name]);
			if(position != userDefinedFunctions.end())
				userDefinedFunctions.erase(position);

			memory.erase(name);
			std::cout<<"Erased "<<name<<" from memory."<<std::endl;
		}
		else if(localMemory.count(name) != 0)
		{
			localMemory.erase(name);
		}
		else
		{
			std::cout<<"	Error could not find "<<name<<" in memory to delete."<<std::endl;
		}
	}
	loadInitialFunctions();
	return MAX;
}
void printHelpMessage(const std::string &expr)
{

	std::cout<<std::endl;
	/*for(char a:expr)
	{
		std::cout<<(int)a<<std::endl;
	}*/
	/*
	 * up
	 * 27,91,65
	 */
	/*
	 * down
	 * 27,91,66
	 */
	/*
	 * right
	 * 27,91,67
	 */
	/*
	 * left
	 * 27,91,68
	 */
	std::cout<<"Error Invalid Parameter \""<<expr<<"\""<<std::endl;
	std::cout<<"Enter a mathematical expression, or you can also use \nparameters to choose between the following options or see below \"redo\" for how to handle variables/functions."<<
			"\nvalid params are:\nt to show time taken to interpret,";
	std::cout<<" and calculate expression";
	std::cout<<"\no to show operations in order of execution in console";
	std::cout<<"\nd to show debug information in console\n";
	std::cout<<"p to show results of all calculations in console\n";


	std::cout<<"u to show execute previous statement in console or \"undo\"\n";
	std::cout<<"r to show \"redo\"\n";
	std::cout<<"\nHow to use variables/functions:";
	std::cout<<"\nlet [variableName] = [expression] to save an expression with the given variable name.\n";
	std::cout<<"const [variableName] = [expression] will save the result of the expression with the given variable name.";
	std::cout<<"\n\nto use a saved expression simply type the variable name, then in parenthesis supply any parameters\n";

	std::cout<<"\nExample:";
	std::cout<<"\nlet x = c^2";
	std::cout<<"\nx(4)";
	std::cout<<"\nand the program will print the result of 4^2";
	std::cout<<"\nbecause it replaces x with c^2, and c with the parameter supplied.\n";

	std::cout<<"\nyou can print the expression a variable holds by typing\n";
	std::cout<<"printvar [variableName] or printall to print everything in memory,\n";
	std::cout<<"printsdf prints only system defined functions\n";
	std::cout<<"printudf prints only user defined functions\n";
	std::cout<<"print [your_expression] will print the result of computing the expression\n";
	std::cout<<"\nYou can also delete a variable by typing delete [variableName]\n";
	std::cout<<"Or delete all saved variables by typing deleteall\n";

	std::cout<<"\nYou can print all variables, and their expressions by typing printall\n";
}
std::string letNewVar(const std::string &expr,std::unordered_map<std::string,Object>& localMemory,
		AscalParameters &params, std::map<std::string,Object> &paramMemory,bool saveLast)
{
	//find index of first char that is alpha find index of last alpha before space or =

				//ignore everything till the =

				//use expr.substr(firstIndex,LastIndex) as var name and id and key in hash map use everything after = as expression

				//set expr = expression part (everything after = ignoring spaces) of object and run calculation

				SubStr exPart = getExpr(expr,expr.find('=')+1);
				SubStr newVarPart = getNewVarName(expr);
				Object var(newVarPart.data,exPart.data,expr.substr(newVarPart.end + 1,exPart.start - 1 ));

				if(memory.count(var.id) != 0)
				{
					std::vector<Object>::iterator position = std::find(userDefinedFunctions.begin(), userDefinedFunctions.end(), memory[var.id]);
					if(position != userDefinedFunctions.end())
						userDefinedFunctions.erase(position);
				}
				//set var defined's value in hashmap
				loadUserDefinedFn(var);
				return MAX;
}
std::string constNewVar(const std::string &expr,std::unordered_map<std::string,Object>& localMemory,
		AscalParameters &params, std::map<std::string,Object> &paramMemory,bool saveLast)
{

	SubStr exPart = getExpr(expr,expr.find('=')+1);
	SubStr newVarPart = getNewVarName(expr);
	bool print = *boolsettings["p"];
	std::string value = std::to_string(calcWithOptions(exPart.data,localMemory,params,paramMemory));
	*boolsettings["p"] = print;
	Object var(newVarPart.data,value,"");


	std::vector<Object>::iterator position = std::find(userDefinedFunctions.begin(), userDefinedFunctions.end(), memory[var.id]);
							if(position != userDefinedFunctions.end())
								userDefinedFunctions.erase(position);

	//set var defined's value in hash map
	loadUserDefinedFn(var);
	return value;
}
std::string locNewVar(const std::string &exp,std::unordered_map<std::string,Object>& localMemory,
		AscalParameters &params, std::map<std::string,Object> &paramMemory,bool saveLast)
{
	SubStr localName = getVarName(exp,exp.find("loc")+4);
	SubStr subexp = getExpr(exp,exp.find('=')+1);

	Object newLocalVar(localName.data,subexp.data,"");
	if(*boolsettings["p"])
	{
		std::cout<<std::endl<<"Local var name: "<<localName.data<< " exp: "<<newLocalVar.instructionsToString()<<std::endl;
	}
	localMemory[newLocalVar.id] = newLocalVar;
	return MAX;
}
std::string clocNewVar(const std::string &exp,std::unordered_map<std::string,Object>& localMemory,
		AscalParameters &params, std::map<std::string,Object> &paramMemory,bool saveLast)
{
	SubStr localName = getVarName(exp,exp.find("loc")+4);
	SubStr subexp = getExpr(exp,exp.find('=')+1);
	std::string value = std::to_string(calculateExpression<double>(subexp.data,params,paramMemory,localMemory));
	Object newLocalVar(localName.data,value,"");
	if(*boolsettings["p"])
	{
		std::cout<<std::endl<<"Const local var name: "<<localName.data<< " exp: "<<value<<std::endl;
	}
	localMemory[newLocalVar.id] = newLocalVar;
	return value;
}
int min(int a,int b)
{
	return a<b?a:b;
}
double getNextDoubleS(const std::string &data,int &index)
{
  bool stillReading = true;
  bool isNegative = false;
  bool afterDecimal = false;
  char previous;
  double num = 0;
  int afterDecimalCount = 1;

  if(index-1 >= 0)
  {
    previous = data[index-1];
  }
  else
  {
    previous = '&';
  }
  if(data[index] == '-')
  {
    isNegative = true;
    index++;
  }
  while(stillReading)
  {
    if(data[index]>=48 && data[index]<58)
    {
    	if(!afterDecimal){
    		num *= 10;
    		num += (double)(data[index]-48);
    	}
    	else
    	{
    		num += (double) (data[index]-48)/afterDecimalCount;
    	}
    }
    if(data[index] == '.')
    {
    	afterDecimal = true;
    }
    else if(!isOperator(previous) && index != 0)
  {

    if(data[index]<48 || data[index]>=58)
    {
      stillReading = false;
    }

  }
    if(afterDecimal)
    {
    	afterDecimalCount *= 10;
    }
    previous = data[index++];

  }
  index -= 2;
  if(isNegative)
    num *= -1;
  return num;
}

std::string plotAction(const std::string &expr,std::unordered_map<std::string,Object>& localMemory,
		AscalParameters &params,std::map<std::string,Object> &paramMemory,bool saveLast)
{
	const int plotKeyWordIndex = expr.find("plot");
	const int endOfFun = expr.find(",");
	const int endOfDomain = expr.find(",",endOfFun+1);
	std::vector<double> sumArea;
	sumArea.push_back(0);
	int index = plotKeyWordIndex+4;
	while(expr[index] && expr[index] == ' ')
	{
		index++;
	}
	std::vector<std::string> functions;
	int trailer = index;
	{
		while(expr[index] && expr[index] != ',')
		{
			if(expr[index] == '|')
			{
				functions.push_back(expr.substr(trailer,index-(trailer)));
				sumArea.push_back(0);
				trailer = index+1;
			}
				index++;
		}
	}
	functions.push_back(expr.substr(trailer,endOfFun - (trailer)));
	index = endOfFun+1;
	const double xMin = getNextDoubleS(expr,index);
	index+=2;
	const double xMax = getNextDoubleS(expr,index);
	index = endOfDomain+1;
	const double yMin = getNextDoubleS(expr,index);
	index +=2;
	const double yMax = getNextDoubleS(expr,index);
	index +=2;
	const double xStepSize = getNextDoubleS(expr,index);
	index += 2;
	const double yStepSize = getNextDoubleS(expr,index);
	int tableWidth = (xMax-xMin)/(xStepSize>0?xStepSize:1);
	int tableHeight = (yMax-yMin)/(yStepSize>0?yStepSize:1);
	double dx = (xMax-xMin)/tableWidth;
	double dy = yStepSize>0?yStepSize:1;
	double xi;
	Vect2D<double> outPuts(tableWidth,functions.size()-1);
	for(int j = 0;j<functions.size();j++)
	{
		std::string function = functions[j];
		std::cout<<"\nProcessing: "<<function<<"\n";
		for(int i = 0;i<tableWidth;i++)
		{
			xi = xMin+dx*(i);
			std::unordered_map<std::string,Object> localMemory1;
			std::map<std::string,Object> paramMemory1;
			AscalParameters params1;
			outPuts.push_back(
					calculateExpression<double>(function+"("+std::to_string(xi)+")",params1,paramMemory1,localMemory1));
			sumArea[j] += outPuts.get(i,j)*dx;
			if(!*boolsettings["o"] && *boolsettings["p"])
				std::cout<<function<<"("<<std::to_string(xi)<<")"<<"="<<outPuts.get(i,j)<<",";
		}
	}

	double yClosestToZero = std::numeric_limits<double>::max();
	for(int y = tableHeight+1;y>=0;y--)
	{
		if(abs(yMin+dy*y)<yClosestToZero)
			yClosestToZero = yMin+dy*y;
	}
	double xClosestToZero = std::numeric_limits<double>::max();
	std::cout<<"\n";
	for(int i = 0;i<=tableWidth;i++)
	{
		xi = (dx*i+xMin);

		if(abs(xi) < abs(xClosestToZero))
			xClosestToZero = xi;
		if(i%5 == 0)
			std::cout<<std::to_string(xi).substr(0,4);
		else if((i-1)%5 == 0 || (i-2)%5 == 0|| (i-3)%5 == 0){}
		else
			std::cout<<" ";
	}
	std::cout<<" "<<"\n";
	for(int x = 0;x<tableWidth;x++)
	{
		std::cout<<"|";
	}
	std::cout<<' '<<xMax<<'\n';
	char symbols[10] = {'*','#','%','$','-','@','&','+','!','='};
	for(double y = tableHeight+1;y>=0;y--)
	{

		for(int x = 0;x<tableWidth;x++)
		{
			bool drawNonFn = true;
			for(int z = outPuts.getHeight(); z>=0;z--)
			{
				const double roundedFunctionOutput = (round((outPuts.get(x,z) - yMin) / dy) * dy) + yMin;
				bool drawFnValue = true;
				for(int a =0;drawFnValue && a < z;a++)
				{
					drawFnValue = (roundedFunctionOutput != (round((outPuts.get(x,a) - yMin) / dy) * dy) + yMin);
				}
				if(drawFnValue && y*dy+yMin == roundedFunctionOutput)
				{
					std::cout<<(symbols[z%10]);
					drawNonFn = false;
				}
			}
			if(drawNonFn)
			{
				if(yMin+dy*y == yClosestToZero)
					std::cout<<"-";
				else if(xMin+dx*x == xClosestToZero)
					std::cout<<"|";
				else
					std::cout<<" ";
			}
		}
		std::cout<<" "<<round((y*dy+yMin)*10000)/10000<<"\n";
	}

	for(int x = 0;x<tableWidth;x++)
	{
		std::cout<<"|";
	}
	std::cout<<"\n"<<expr<<"\n";
	std::cout<<"domain:"<<xMin<<" to "<<xMax<<", range:"<<
			yMin<<" to "<<yMax<<" with a step size in the x of:"<<xStepSize<<", and in the y: "<<yStepSize<<"\n";
	for(int i =0; i<functions.size();i++)
	{
		std::cout<<"Function: "<<functions[i]<<", plotted using symbol: "<<symbols[i%10]<<", function defined as: "<<memory[functions[i]].instructionsToString();
		std::cout<<"Area Under Curve calculated with reimann sum using "<<tableWidth<<" partitions: "<<sumArea[i]<<"\n\n";
	}

	return MAX;
}
std::string setAction(const std::string &expr,std::unordered_map<std::string,Object>& localMemory,
		AscalParameters &params,std::map<std::string,Object> &paramMemory,bool saveLast)
{
	SubStr varName = getVarName(expr,4);
	int startIndex = expr.find("=",varName.end)+1;
	while(expr[startIndex] == ' ')
		startIndex++;
	SubStr subexp = getExpr(expr,startIndex);
	std::string value = std::to_string(calculateExpression<double>(subexp.data,params,paramMemory,localMemory));
	if(localMemory.count(varName.data))
	{
		localMemory[varName.data] = Object(varName.data,value,"");
		if(*boolsettings["p"])
			std::cout<<"Local Var: "<<varName.data<<" set to: "<<value<<std::endl;
	}
	else if(paramMemory.count(varName.data))
	{
		paramMemory[varName.data] = Object(varName.data,value,"");
		if(*boolsettings["p"])
			std::cout<<"Parameter: "<<varName.data<<" set to: "<<value<<std::endl;
	}
	else if(memory.count(varName.data))
	{
		memory[varName.data] = Object(varName.data,value,"");
		if(*boolsettings["p"])
			std::cout<<"Global Var: "<<varName.data<<" set to: "<<value<<std::endl;
	}
	return MAX;
}

std::string ifAction(const std::string &expr,std::unordered_map<std::string,Object>& localMemory,
		AscalParameters &params,std::map<std::string,Object> &paramMemory,bool saveLast)
{
	return MAX;
}
std::string whileAction(const std::string &expr,std::unordered_map<std::string,Object>& localMemory,
		AscalParameters &params,std::map<std::string,Object> &paramMemory,bool saveLast)
{
	//std::cout<<"expression in while: "<<expr<<std::endl;
	int index = expr.find("while")<1000?expr.find("while")+5:0;
	while(expr[index] == ' ')
		index++;

	int startOfBoolExp = index;
	int startOfCodeBlock = index;
	SubStr codeBlock("",0,0);

	//std::cout<<"start of boolExp: "<<startIndex<<" end of bool exp: "<<startOfCodeBlock<<std::endl;
	while(expr[startOfCodeBlock] && expr[startOfCodeBlock] != '{')
	{
		startOfCodeBlock++;
	}
	//get indexes of start and end of the boolean expression for extraction
	while(index < startOfCodeBlock && expr[index] == ' ')
	{
		startOfBoolExp++;
		index++;
	}
	//std::cout<<"while: "<<expr.substr(index,expr.size())<<std::endl;
	//std::cout<<"while after skipping whitespace : "<<expr.substr(index,expr.size())<<std::endl;
	while(index < startOfCodeBlock)
	{
		index++;
	}
	//std::cout<<"while after finding end of exp: "<<expr.substr(index,expr.size())<<" start of Exp Index: "<<startOfBoolExp<<" end Of boolExp Index: "<<index<<std::endl;
	const std::string booleanExpression = expr.substr(startOfBoolExp,index-startOfBoolExp);
	double boolExpValue;
	//std::cout<<"BoolExpression: "<<booleanExpression;
	bool print = *boolsettings["p"];
	bool printTime = *boolsettings["t"];
	//*boolsettings["p"] = false;
	if(!print)
	{
		*boolsettings["t"] = false;
	}
	//std::cout<<"bool expression Being Exec: "<<booleanExpression<<std::endl;
	try{
	boolExpValue = calculateExpression<double>(booleanExpression,params,paramMemory,localMemory);
	}
	catch(std::string &exception)
	{
		throw std::string(exception + "\nIn While boolean exp: " + booleanExpression);
	}
	//std::cout<<" value: "<<boolExpValue<<std::endl;
	codeBlock = getExpr(expr,index);
	if(boolExpValue != 0)
	{
		//Build List of expressions from code block associated with the while
		std::vector<std::string> expressions;
		{
			int trailer = 0;
			for(int i = 0;i<codeBlock.data.size();i++)
			{
				if(codeBlock.data[i] == ';')
				{
					expressions.push_back(codeBlock.data.substr(trailer,i-trailer));
					trailer = i;
				}
			}
		}
			//exprList now is essentially a list of strings that are the expressions
		while(boolExpValue != 0)
		{

			for(std::string &exp:expressions)
			{
				//std::cout<<"expression Being Exec: "<<exp<<" Len: "<<exp.length()<<std::endl;
				try{
				calculateExpression<double>(exp,params,paramMemory,localMemory);
				}
				catch(std::string &exception)
				{
					throw std::string(exception + "\nIn While body subexp: " + exp);
				}
			}
			//std::cout<<"BoolExpression: "<<booleanExpression;
		try{
			boolExpValue = calculateExpression<double>(booleanExpression,params,paramMemory,localMemory);
		}
		catch(std::string &exception)
		{
			throw std::string(exception + "\nIn While boolean exp: " + booleanExpression);
		}
			//std::cout<<" value: "<<boolExpValue<<std::endl;
		}
	}
	*boolsettings["t"] = printTime;
	*boolsettings["p"] = print;
	index = codeBlock.end + startOfCodeBlock;
	while(expr[index] == ';' || expr[index] == ' ' || expr[index] == '}')
		index++;
	//std::cout<<"newExpIndeces length: "<<expr.size()<<" new start: "<<index<<" New Exp After While: "<<expr.substr(index,expr.size())<<std::endl;
	return "a"+expr.substr(index,expr.size());
}
std::string whenAction(const std::string &expr,std::unordered_map<std::string,Object>& localMemory,
		AscalParameters &params,std::map<std::string,Object> &paramMemory,bool saveLast)
{
	//parse expression, start from substr when go up until substr end is found
	//extract substring, and save the other parts of the expression
	//find when evaluate expression between it and then
	//if the expression evaluates to anything other than 0
	//then extract the expression proceeding the then statement
	LOG_DEBUG("Expression before When:"<<expr<<"\n Params: "<<params.toString())

	const int startIndex = expr.find("when")<1000?expr.find("when"):0;
	const int endIndex = expr.find("end")<1000?expr.find("end"):0;
	std::string startOfExp = expr.substr(0,startIndex);
	std::string endOfExp = expr.substr(endIndex+3,expr.length());
	std::string value;
	int index = startIndex + 4;
	std::vector<char> exp;
	//should always start after when is finished to build boolean expression

	int thenIndex;
	int whenIndex = startIndex;
	double boolExpValue;
	int elseIndex = expr.find("else");
	elseIndex = elseIndex==-1?1000000:elseIndex;
	int lastThen = 0;
	//std::cout<<"start: "<<startOfExp<<" End: "<<endOfExp<<"\n";
	do {
		thenIndex = expr.find("then",index);
		//std::cout<<"Starting of exp: "<<index<<":"<<expr[index]<<" thenIndex: "<<thenIndex<<":"<<expr[thenIndex]
		//		<<" endIndex: "<<endIndex<<":"<<expr[endIndex]<<"\n";
		while(expr[index] == ' ' && index < endIndex && index < thenIndex)
			index++;
		while(expr[index] && index < endIndex && index < thenIndex)
		{
			exp.push_back(expr[index]);
			index++;
		}
		//std::cout<<"\n";
		std::string booleanExpression(exp.begin(),exp.end());
		exp.clear();
		bool showOp = *boolsettings["o"];
		*boolsettings["o"] = false;
		boolExpValue = calculateExpression<double>(booleanExpression,params,paramMemory,localMemory);
		*boolsettings["o"] = showOp;
	//false case simply set the index to the next instance of when+4
	//and repeat until true, or at end of case when
		if(boolExpValue == 0)
		{
			index = expr.find("when",index) + 5;
			whenIndex = index - 5;
			if(whenIndex == -1)
			{
				value = getExpr(expr.substr(elseIndex+4,endIndex-(elseIndex+4)),0).data;
				index = endIndex;
				value = startOfExp+value+endOfExp;
				//std::cout<<"In Else Case rtn val: "<<value<<"\n";
			}

		}
	//true case get sub expression associated with this when return value exit loop
		else
		{
			index += 5;
			thenIndex = expr.find("when",index);
			thenIndex = thenIndex==-1?endIndex+1:thenIndex;

			value = getExpr(expr.substr(index,std::min(std::min(endIndex,thenIndex),elseIndex)-index),0).data;
			if(*boolsettings["o"])
			{
				std::cout<<"In choice: "<<expr<<" taking path: "<<value;
				std::cout<<" Params: ";
				for(auto &p:params)
				{
					std::cout<<p<<" ";
				}
				std::cout<<"\n";
			}
			value = startOfExp+value+endOfExp;
		}
		lastThen = thenIndex;
	} while(expr[index] && boolExpValue == 0 && index < endIndex);
	return "a"+value;
}
std::string printCalculation(const std::string &expr,std::unordered_map<std::string,Object>& localMemory,
		AscalParameters &params, std::map<std::string,Object> &paramMemory,bool saveLast)
{
	std::string subexpr = getExpr(expr.substr(expr.find("print")+5,expr.length()),0).data;
	bool print = *boolsettings["p"];
	*boolsettings["p"] = true;
	std::string value = std::to_string(calcWithOptions(subexpr,localMemory,params,paramMemory));
	*boolsettings["p"] = print;
	return value;
}
void printVar(const std::string &expr,bool saveLast)
{
	std::cout<<memory[getVarName(expr,10).data].instructionsToString()<<"\n";
}
std::string printCommand(const std::string &expr,std::unordered_map<std::string,Object>& localMemory,
		AscalParameters &params, std::map<std::string,Object> &paramMemory,bool saveLast)
{
	std::cout<<"\n";
	std::string value  = MAX;
			if(cmpstr(expr.substr(6,3),"all"))
			{
				printAllFunctions();
			}
			else if(cmpstr(expr.substr(6,3),"udf"))
			{
				printAllUDF();
			}
			else if(cmpstr(expr.substr(6,3),"sdf"))
			{
				printAllSDF();
			}
			else if(cmpstr(expr.substr(6,3),"var"))
			{
				printVar(expr,saveLast);
			}
			else
			{
				bool print = *boolsettings["p"];
				printCalculation(expr,localMemory,params,paramMemory,saveLast);
				*boolsettings["p"] = print;
			}
			return value;
}
void updateBoolSetting(const std::string &expr)
{

	lastExp.pop();
	setting<bool> set = boolsettings[expr];
	//inverts setting value via operator overloads of = and *
	bool data = !*set;
	setting<bool> newSetting(set.getName(),set.getCommand(),data);
	boolsettings[set.getCommand()] = newSetting;
	if(*boolsettings["p"])
		std::cout<<set.getName()<<" Status: "<<data<<"\n";
}
//returns a string of all the data it has read in a single string delimited by ;
//and a double which is the result of the last calc

//create function that loops through istream supplied as parameter in as we see {
//if no frames in stack open new stack frame with new local memory
//keep executing statements using the same local memory until } is seen
//if size of stack is greater than one simply pop one off the stack to be used
//
double interpretParam(std::string &expr,std::unordered_map<std::string,Object> &localMemory,
		std::map<std::string,Object>& paramMemory,bool saveLast)
{
	double value = 0;
	std::string firstWord = getVarName(expr,0).data;
	if(expr.length() == 0) {}
	else if(memory.count(firstWord) != 0 || (expr[0] >= 48 && expr[0] < 58) || isOperator(expr[0]) ||
			cmpstr(firstWord,"loc") || localMemory.count(firstWord) != 0 || inputMapper.count(firstWord) != 0 )
	{
		AscalParameters params;
		bool print = *boolsettings["p"];

		if(saveLast && !cmpstr(expr,"u") && !cmpstr(expr,"r"))
			lastExp.push(expr);

		value = calcWithOptions(expr,localMemory,params,paramMemory);
		if(firstWord.size() != 1 && firstWord[0] != 'p')
		*boolsettings["p"] = print;
	}
	else
	{
		printHelpMessage(expr);
	}
	return value;
}

bool cmpstr(const std::string &s1,const std::string &s2)
{
	bool isEqual = true;
	int i = 0;
	while(isEqual && s1[i] && s2[i])
	{
		if(s1[i] != s2[i])
		{
			isEqual = false;
		}
		i++;
	}

	if(s1[i] != s2[i])
	{
		isEqual = false;
	}
	return isEqual;
}
double calcWithOptions(std::string expr,std::unordered_map<std::string,Object> &localMemory,AscalParameters &params, std::map<std::string,Object> &paramMemory)
{
	bool timeInstruction = *boolsettings["t"];


	std::cout<<"";
	std::chrono::system_clock::time_point start,end;
	if(timeInstruction){
	start = std::chrono::system_clock::now();
	}

	//alternate beginning of calculation for doubles
	//-------------------------
	double result = calculateExpression<double>(expr,params,paramMemory,localMemory);
	//------------------------
	//std::cout<<std::to_string(result).length()<<"  max len "<<MAX.length()<<std::endl;
	//if()//returned by function that doesn't return a result
	{
		if(timeInstruction){
			end = std::chrono::system_clock::now();

			std::chrono::duration<double> elapsed_seconds = end-start;
			std::time_t end_time = std::chrono::system_clock::to_time_t(end);

			std::cout << "finished computation at " << std::ctime(&end_time)
			          << "elapsed time: " << elapsed_seconds.count() << "s\n";
		}
		if(std::to_string(result).length() != MAX.length() && *boolsettings["p"])
		{
			std::cout<<"Final Answer: "<<std::endl<<result<<std::endl;
		}
	}
	return result;

}
bool hasChar(const std::string &dat,const char &c)
{
	bool contains = false;
	for(int i = 0;!contains && i<dat.length();i++)
	{
		if(dat[i] == c)
		{
			contains = true;
		}
	}
	return contains;
}
int getClosingIndex(char opening,const std::string &data)
{
	return 0;
}
SubStr getExpr(const std::string &data,int index)
{
	int count = 0;
	int openingCount = 0;
	std::string result;
	std::string line = data;
	do{
		while(line[index] && !isNumeric(line[index]) && !isalpha(line[index]) && !isOperator(line[index]) && line[index] != '}')
		{
			if(line[index] == '{')
			{
				openingCount++;
			}
			else if(line[index] == '}')
			{
				openingCount--;
			}
			index++;
		}
		while(line[index + count] && (line[index + count] != ';' && line[index + count] != '\n')
				&& !(line[index+count] == '}' && openingCount == 1))
		{
			if(line[index+count] == '{')
			{
				openingCount++;
			}
			else if(line[index+count] == '}')
			{
				openingCount--;
			}
			count++;
		}
		if(line[index+count] == '}')
		{
			openingCount--;
		}
		if(count > 0)
		{
			if(line[line.size()-1]!='{' && line[line.size()-1] != ';')
				result += line.substr(index>line.length()?line.length():index,count+1)+';';
			else
				result += line.substr(index>line.length()?line.length():index,count+1);
			//std::cout<<"CurrentLine: "<<line<<" Extracted statement: "<<result<<" Remaining unclosed openings: "<<openingCount<<std::endl;
		}
		if(openingCount > 0 && line.length() <= index+count)
		{
			getline(std::cin, line);
			index = 0;
		}
		else
		{
			index = index+count+1;
		}
		count = 0;
	}while(openingCount > 0 && std::cin);
	result = result.substr(0,result.size()-1);
	return SubStr(result,index,result.length());
}

SubStr getNewVarName(const std::string &data)
{
	int index = data.find("let")<500 || data.find("const")<500?data.find("t ")+1:0;
	while(!isalpha(data[index]) || data[index] == ' ')
	{
		index++;
	}
	return getVarName(data,index);
}

SubStr getVarName(const std::string &s,int index)
{
	int begin = index;
	int count = 0;
	while(s[index] && s[index] == ' ')
	{
		begin++;
		index++;
	}
	while(s.length()>index && !isOperator(s[index]) && (isalpha(s[index]) || isNumeric(s[index])))
	{
		index++;
		count++;
	}
	index--;
	std::string result = s.substr(begin>s.length()?s.length():begin,count);
	return SubStr(result,begin,begin + result.length()-1);
}
std::string replace(std::string &original,std::string &replace,std::string &replacement)
{
	for(int i = 0; original.length()>replace.length()+i;i++)
	{
		if(cmpstr(original.substr(i,replace.length()),replace) && (original[i+1] == 0 || isOperator(original[i+1]) || original[i+1] == ' ' || isNumeric(original[i+1])))
		{
			original = original.substr(0,i-1)+replacement+original.substr(i+replace.length(),original.length());
			i += replacement.length() - replace.length();
		}
	}
	return original;
}

std::string printMemory(std::map<std::string,Object> &memory,std::string delimiter,bool justKey = true,
		std::string secondDelimiter = "\n")
{
	std::string s;
	if(justKey)
		for(auto &[key,value]:memory)
			s+=key+delimiter;
	else
		for(auto &[key,value]:memory)
			s+=key+delimiter+value.instructionsToString()+secondDelimiter;
	return s;
}
std::string printMemory(std::unordered_map<std::string,Object> &memory,std::string delimiter,bool justKey = true,
		std::string secondDelimiter = "\n")
{
	std::string s;
	if(justKey)
		for(auto &[key,value]:memory)
			s+=key+delimiter;
	else
		for(auto &[key,value]:memory)
			s+=key+delimiter+value.instructionsToString()+secondDelimiter;
	return s;
}
template <class t>
t calculateExpression(std::string exp,AscalParameters &params,std::map<std::string,Object> &paramMemory,std::unordered_map<std::string,Object> &localMemory)
{
	//int paramUse = 0;
	//std::cout<<"Printing Params for exp: "<<exp<<std::endl;
	//printVector(params);
	//loop through exp
	//
	 //if current char is Alpha
	 //check if proceeding char is ( if it is take the substring between it and the ) that closes it as params
	//if not check if its alpha add next char to var id to lookup if its a var with params go to first ), then process var with (

	//best would be a function that saves function name and params in stack that have yet to be processed
	//function takes a variable, and if var has ( denoting parameters scan through string until alpha is found if alhpa found
	//pass var with params to this function

	//if not lookup var in memory then calcExpression on its expression
	//if not in hashMap then assume it is a parameter, and scroll past its index in loop and add param according to the param list

	//param list should be a vector of expressions the calculateExpression function takes
	//if param list used up prompt user for expression to use as param


	//Establish there is a while if else if or else block
	//load boolean expression to check to an Object
	//Load associated code block if boolean expression is true otherwise skip until closing }
	//if while execute block in a loop where the condition is the result of the boolean expression defined by user
	//if block is associated with if or else if execute
	//if boolean expression for last check was false and there is an else block associated run the else block


	//get debug setting from boolsettings hashmap,
	//then using * operator to get setting data
	//which is an overloaded operator in the settings class
	//variable that is always a copy
	  char peeker;
	  //variables to hold the operands
	  t and1,and2;
	  //c is a variable to store each of the characters in the expression
	  //in the for loop
	  char currentChar;
	  stack<t> initialOperands;
	  stack<char> initialOperators;
	  LOG_DEBUG("Calculating expression: "<<exp<<std::endl)

	//This loop handles parsing the numbers, and adding the data from the expression
	//to the stacks
	for(int i = 0;i <= exp.length();i++)
	{
	 currentChar = exp[i];
	 initialOperators.top(peeker);
	 if(isalpha(currentChar) && !isOperator(currentChar) && currentChar != 'X')
	 {
		 LOG_DEBUG("Local Memory: "<<printMemory(localMemory,"=",false," "))
		 LOG_DEBUG("Param Memory: "<<printMemory(paramMemory,"=",false," "))

		 bool printValue = *boolsettings["p"];
		 SubStr varName(getVarName(exp,i));
		 Object data = memory.count(varName.data)!=0?memory[varName.data]:Object();
		 Object localData = localMemory.count(varName.data)!=0?localMemory[varName.data]:Object();

		 std::string endOfExp;
		 //Keyword handling only one keyword at the beginning of each statement allowed,
		 //including statements defined in variables
		 if(inputMapper.count(varName.data) != 0)
		 {

			 //std::cout<<"Old Exp Before inputMapper: "<<exp<<std::endl;
			 std::string result = inputMapper[varName.data](exp,localMemory,params,paramMemory,false);

			 while(exp[i] && (exp[i] != ';' && exp[i] != '\n'))
			 {
				 i++;
			 }
			 if((exp[i] == 0 || exp[i+1] == 0) && cmpstr(result,MAX))
			 {
				 exp = MAX;
			 }
			 else if(isalpha(result[0]))
			 {
				 //for returning altered versions of the expression up till a statement separator like ;
				 //use cases: getting user input outside functions and variables
				 //when then when then end if else.
				 //while loops?
				 //just make sure first character in returned string is a throwaway alphabetical character
				 //it will not be parsed so ensure it is not meaningful code
				 exp = result.substr(1,exp.length());
				 //std::cout<<"Expression after statement altering keyword: "<<exp<<std::endl;
				 //std::cout<<"expression returned from when action: "<<exp<<std::endl;
				 while(!initialOperands.isEmpty())
					 initialOperands.pop();
				 while(!initialOperators.isEmpty())
					 initialOperators.pop();

				 i = -1;
				 //currentChar = exp[i];
				 continue;
			 }
			 else
			 {
				 exp = exp.substr(i,exp.length());
			 }
			 //std::cout<<"New Expression after inputMapper: "<<exp<<std::endl;
			 i = 0;
			 currentChar = exp[i];
		 }
		 //Variable handling section
		 else if(data.id.length() != 0)
		 {
			 *boolsettings["p"]  = false;
			 int endOfParams = data.setParams(exp[varName.end+1] == '('?exp.substr(varName.end+1):"");
			 int startOfEnd = data.params.size()==0?varName.end+1:varName.end+endOfParams;
			 endOfExp = exp.substr(startOfEnd,exp.length());


			 LOG_DEBUG("In expression: "<<exp<<" ")
			 //Filling params of called functions with params from calling function where there are undefined vars
			 for(int i =0; i<data.params.size();i++)
			 {

				 //if after calc exp data.params[i] has changed, and the original
				 LOG_DEBUG("Resolving Parameter: "<<data.params[i]<<" To: ")

				 data.params[i] =
						 std::to_string(calculateExpression<double>(data.params[i],params,paramMemory,localMemory));
				 LOG_DEBUG(data.params[i])
			 }
			 std::vector<std::string> expressions = data.getInstructions();
			 int j;

			 std::unordered_map<std::string,Object> calledFunctionLocalMemory;
			 std::map<std::string,Object> calledFunctionParamMemory;
			 for(j = 0;j<expressions.size()-1;j++)
			 {
				 calculateExpression<double>(expressions[j],data.params,calledFunctionParamMemory,calledFunctionLocalMemory);
			 }
			 if(expressions.size() == 1)
				 j = -1;

			 LOG_DEBUG("Global Var parsing exp: "<<exp<<" Resolving: "<<varName.data<<" to: "<<expressions[j+1]<<std::endl)

			 double varValue = calculateExpression<double>(expressions[j+1],data.params,calledFunctionParamMemory,calledFunctionLocalMemory);
			 std::string value = std::to_string(varValue);
			 exp = exp.substr(0,varName.start) + value + endOfExp;
			 i = varName.start;
			 currentChar = exp[i];
			 LOG_DEBUG("Object: "<<varName.data<<" First Part: "<<exp.substr(0,varName.start)<<" Second: "<<value<<" Third: "<<endOfExp<<
					 "\nHello this is loading exp: "<<exp)
			 LOG_DEBUG("Current Char: "<<currentChar)

			 //std::cout<<exp.substr(i)<<std::endl;
			 *boolsettings["p"] = printValue;

		 }
		 else
		 {
			 //std::cout<<params.size()<<" param list len\n";
			 if(localData.id.length() != 0)
			 {
				 *boolsettings["p"]  = false;
				 int endOfParams = localData.setParams(exp[varName.end+1] == '('?exp.substr(varName.end+1):"");
				 int startOfEnd = localData.params.size()==0?varName.end+1:varName.end+endOfParams;
				 endOfExp = exp.substr(startOfEnd,exp.length());
				 std::vector<std::string> expressions = localData.getInstructions();

				 LOG_DEBUG("In expression: "<<exp<<" ")
				 //Filling params of called functions with params from calling function where there are undefined vars
				 for(int i =0; i<localData.params.size();i++)
				 {

					 //if after calc exp data.params[i] has changed, and the original
					 LOG_DEBUG("Resolving Local Data Parameter: "<<localData.params[i]<<" To: ")
					 localData.params[i] = std::to_string(calculateExpression<double>(localData.params[i],params,paramMemory,localMemory));
					 LOG_DEBUG(std::cout<<localData.params[i])
				 }
				 int j;
				 for(j = 0;j<expressions.size()-1;j++)
				 {
					 calculateExpression<double>(expressions[j],localData.params,paramMemory,localMemory);
				 }
				 if(expressions.size() == 1)
					 j = -1;
				 double varValue = calculateExpression<double>(expressions[j+1],localData.params,paramMemory,localMemory);
				 std::string value = std::to_string(varValue);
				 //endOfExp = exp.substr(varName.end+1,exp.length());
				 exp = exp.substr(0,varName.start) + value + endOfExp;
				 i = varName.start;

				 LOG_DEBUG("Object: "<<varName.data<<" First Part: "<<exp.substr(0,varName.start)<<" Second: "<<value<<" Third: "<<endOfExp<<
				 	 "\nHello this is loading exp: "<<exp)

				 currentChar = exp[i];
				 *boolsettings["p"] = printValue;
			 }
			 else if(paramMemory.count(varName.data))
			 {
				 *boolsettings["p"]  = false;
				 Object paramData = paramMemory[varName.data];
				 int endOfParams = paramData.setParams(exp[varName.end+1] == '('?exp.substr(varName.end+1):"");
				 				 int startOfEnd = paramData.params.size()==0?varName.end+1:varName.end+endOfParams;
				 				 endOfExp = exp.substr(startOfEnd,exp.length());
				 				 std::vector<std::string> expressions = paramData.getInstructions();

				 				LOG_DEBUG("In expression: "<<exp<<" ")
				 				 //Filling params of called functions with params from calling function where there are undefined vars
				 				 for(int i =0; i<localData.params.size();i++)
				 				 {

				 					 //if after calc exp data.params[i] has changed, and the original
				 					LOG_DEBUG("Resolving Local Data Parameter: "<<paramData.params[i]<<" To: ")

				 					paramData.params[i] = std::to_string(
				 							calculateExpression<double>(paramData.params[i],params,paramMemory,localMemory));
				 					LOG_DEBUG(std::cout<<paramData.params[i])
				 				 }
				 				 int j;
				 				 for(j = 0;j<expressions.size()-1;j++)
				 				 {
				 					 calculateExpression<double>(expressions[j],paramData.params,paramMemory,localMemory);
				 				 }
				 				 if(expressions.size() == 1)
				 					 j = -1;
				 				 double varValue = calculateExpression<double>(expressions[j+1],paramData.params,paramMemory,localMemory);
				 				 std::string value = std::to_string(varValue);
				 				 //endOfExp = exp.substr(varName.end+1,exp.length());
				 				 exp = exp.substr(0,varName.start) + value + endOfExp;
				 				 i = varName.start;

				 				LOG_DEBUG("Object: "<<varName.data<<" First Part: "<<exp.substr(0,varName.start)<<" Second: "<<
				 						value<<" Third: "<<endOfExp<<"\nHello this is loading exp: "<<exp)

				 				 currentChar = exp[i];
				 				 *boolsettings["p"] = printValue;
			 }
			 else if(params.getUseCount() < params.size())
			 {
				 *boolsettings["p"]  = false;
				 Object localVar(varName.data,params[params.getUseCount()],"");
				 ++params;
				 paramMemory[localVar.id] = localVar;

				 std::unordered_map<std::string,Object> calledFunctionLocalMemory;
				 std::map<std::string,Object> calledFunctionParamMemory;
				 std::vector<std::string> expressions = localVar.getInstructions();
				 int j;
				 for(j = 0;j<expressions.size()-1;j++)
				 {
					 calculateExpression<double>(expressions[j],localVar.params,calledFunctionParamMemory,calledFunctionLocalMemory);
					 LOG_DEBUG("Running instruction: "<<expressions[j]<<" While Loading param: "<<varName.data)
				 }
				 if(expressions.size() == 1)
					 j = -1;
				 double varValue = calculateExpression<double>(expressions[j+1],localVar.params,calledFunctionParamMemory,calledFunctionLocalMemory);
				 std::string value = std::to_string(varValue);

				 endOfExp = exp.substr(varName.end+1,exp.length());

				 LOG_DEBUG("Param Object name: "<<varName.data<<" paramuseIndex: "<<params.getUseCount()<<" First Part: "<<
						 exp.substr(0,varName.start)<<" Second: "<<value<<" Third: "<<endOfExp<<"\nHello this is loading exp: "<<exp)

				 exp = exp.substr(0,varName.start)+value+endOfExp;
				 //exp = replace(exp,varName.data,params[paramUse-1]);
				 //std::cout<<"First: "<<exp.substr(0,varName.start)<<" Second: "<<params[paramUse-1]<<" Third: "<<endOfExp;
				 i = varName.start;
				 //std::cout<<"Hello this is loading exp: "<<exp<<std::endl;
				 currentChar = exp[i];
				 //exp = exp.substr(i,exp.length());
				 //std::cout<<exp.substr(i,exp.length())<<std::endl;
				 //std::cout<<"Pos in Exp: "<<i<<" currentChar "<<currentChar<<std::endl;
				 *boolsettings["p"] = printValue;
			 }
			 else
			 {
				#if THROWERRORS == 1
				 if(cmpstr(varName.data,"inf"))
					 throw std::string("Arithmetic Overflow\nIn Expression: "+exp);
				 else
					 throw std::string("Invalid reference: "+varName.data+"\nIn Expression: "+exp);
				#endif
			 }
		 }

	 }
	 else
	    if (currentChar==')')
	    {
	    	LOG_DEBUG("Parentheses Process: "<<exp)
	      initialOperators.top(peeker);
	      stack<t> inParenthesesOperands;
	      stack<char> inParenthesesOperators;
	      while(!initialOperators.isEmpty() && peeker != '(')
	      {
	        initialOperands.top(and1);
	        initialOperands.pop();
	        inParenthesesOperands.push(and1);
	        inParenthesesOperators.push(peeker);
	        initialOperators.pop();
	        initialOperators.top(peeker);
	      }

	      initialOperands.top(and2);
	      initialOperands.pop();
	      inParenthesesOperands.push(and2);
	      initialOperators.pop();
	      //Send expression in parentheses to processStack for evaluation
	      try{
	      and2 = processStack(inParenthesesOperands,inParenthesesOperators);
	      }catch(std::string &error)
	      {
	    	  throw std::string("In Expression: "+exp+" "+error);
	      }

	      initialOperands.push(and2);
	      if(isNumeric(exp[i+1]) || exp[i+1] == '(')
	      {
	        initialOperators.push('*');
	      }

	      LOG_DEBUG("Result of PP: "<<and2<<" Back to initial processing: "<<exp)
	    }
	    //Section to parse numeric values from expression as a string to be inserted into
	    //initialOperands stack
	    if(isNumeric(currentChar) ||
	    ((currentChar == '-' || currentChar == '.')&& (i == 0 || isNonParentheticalOperator(exp[i-1]) || exp[i-1] =='(')
	    ))
	    {
	    	//This comment block must be enabled to use Integer instead of long
	    	//as the number's datatype
	    	/*if(cmpstr(typeid(and1).name(),"i"))
	    	{
	  	      int nextInt = getNextInt<int>(exp,i);
	  	      initialOperands.push(nextInt);
	    	}
	    	else */
	    		double nextDouble = getNextDouble(exp,i);
	    		initialOperands.push(nextDouble);
	    }
	    //This is to support multiplication when expressions look like 2(4)
	    else if(currentChar == '(' && isNumeric(exp[i-1]))

	    {
	      initialOperators.push('*');
	      initialOperators.push(currentChar);
	    }

	    //Section to parse operator's values from expression as a string to be inserted into
	    //initialOperators stack
	    else if(currentChar == 'X')
	    {
	    	initialOperators.push('*');
	    }
	    else if(isOperator(currentChar) && currentChar != ')')
	    {
	      initialOperators.push(currentChar);
	    }
	  }
	//Finally pop all values off initial stack onto final stacks for processing
	  stack<t> finalOperands;
	  stack<char> finalOperators;
	while(!initialOperands.isEmpty() || !initialOperators.isEmpty())
	{
	  if(!initialOperands.isEmpty()){
	  initialOperands.top(and1);
	  finalOperands.push(and1);
	  initialOperands.pop();
	  }
	  if(!initialOperators.isEmpty()){
	  initialOperators.top(peeker);
	  finalOperators.push(peeker);
	  initialOperators.pop();
	  }
	}

	LOG_DEBUG("\nFinal Process: "<<exp)
	t data;
	//process values in stacks, and return final solution
	try{
		data = processStack(finalOperands,finalOperators);
	}catch(std::string &error)
	{
		throw std::string("In Expression: "+exp+" "+error);
	}
	return data;
}
template <class t>
void printStack(stack<t> &operands,stack<char> &operators)
{
	t and1;
	char ator;
	std::cout << "Program Stack: "<<std::endl;
	while(!operands.isEmpty() && !operators.isEmpty())
	{
		operands.top(and1);
		operators.top(ator);
		operands.pop();
		operators.pop();
		std::cout << " "<<and1<<" "<<ator;
	}
	std::cout<<std::endl;
}
template <class t>
t processStack(stack<t> &operands,stack<char> &operators)
{
	  t result = 0,and1 = 0,and2 = 0;
	  char firstOperator,nextOperator;
	  //loop to process expression saved in stack
	 while(!operators.isEmpty() && operands.length()>1)
	 {
	//initialize values so we can make sure no garbage, or previous values is in the variables
	//so top function can work properly
	  nextOperator = 'a',firstOperator = 'a';
	//now with more stacks!
	  //these stacks handle respecting priority, the can handle as many priority levels as you like
	stack<t> savedOperands;
	stack<char> savedOperators;
	//do while loop to search for operation with highest priority
	  do
	  {
		  operands.top(and1);
		  operands.pop();

		  operands.top(and2);

		  operators.top(firstOperator);
		  operators.pop();

		  operators.top(nextOperator);
		  if(getPriority(nextOperator)>getPriority(firstOperator))
		  {
			  savedOperators.push(firstOperator);
			  savedOperands.push(and1);
		  }

	  } while(getPriority(nextOperator)>getPriority(firstOperator));
	  //perform found operation, and remove remaining operand from stack
	  operands.pop();
	  operands.push(calc(firstOperator,and1,and2));
	  //add overridden operators back to original stack
	  while(!savedOperators.isEmpty())
	  {
		  savedOperators.top(firstOperator);
		  savedOperators.pop();
		  operators.push(firstOperator);
	  }
	  //replace overridden operands back in original stack
	  while(!savedOperands.isEmpty())
	  {
		  savedOperands.top(and1);
		  savedOperands.pop();
		  operands.push(and1);
	  }
	}

#if THROWERRORS == 1
	if(operands.size()>1 || operators.size()>0)
	{

		if(operands.size()>1 && operators.size()==0)
		{
			printStack(operands,operators);
			throw std::string("Error, too many operands. ");
		}
		else if(operators.size()>0 && operands.size()==1)
		{
			printStack(operands,operators);
			throw std::string("Error, too many operators.");
		}
		else
		{
			printStack(operands,operators);
			throw std::string("Error, too few operators, and too few operands.");
		}

	}
#endif
	//get result from processing expression
	operands.top(result);
	  return result;
}

template <class t>
t getNextInt(std::string data,int &index)//index is a reference so that when we return
//The for loop which passed it's counter variable is updated to look at the next char after
//This number
{
  bool stillReading = true;
  bool isNegative = false;
  char previous;
  t num = 0;
  if(index-1 >= 0)
  {
    previous = data[index-1];
  }
  else
  {
    previous = 'a';
  }
  if(isOperator(data[index]) && (isOperator(previous) || index == 0))
  {
    if(data[index] == '-'){
      isNegative = true;
    }
    index++;
  }
  while(stillReading)
  {

    if(data[index]>=48 && data[index]<58)
    {
      num *= 10;
      num += (int)(data[index]-48);
    }
    if(!isOperator(previous) && index != 0)
    {
    	if(data[index]<48 || data[index]>=58)
    	{
    		stillReading = false;
    	}
    }

    previous = data[index++];
  }
  index -= 2;
  if(isNegative)
    num *= -1;
  return num;
}
double getNextDouble(const std::string &data,int &index)
{
  bool stillReading = true;
  bool isNegative = false;
  bool afterDecimal = false;
  char previous;
  double num = 0;
  int afterDecimalCount = 1;

  if(index-1 >= 0)
  {
    previous = data[index-1];
  }
  else
  {
    previous = '&';
  }
  if(data[index] == '-' && (isOperator(previous) || index == 0))
  {
    if(data[index] == '-')
      isNegative = true;
    index++;
  }
  while(stillReading)
  {
	  //std::cout<<"index: "<<index<<" "<<data[index]<<" "<<num<<std::endl;
    if(data[index]>=48 && data[index]<58)
    {
    	if(!afterDecimal){
    		num *= 10;
    		num += (double)(data[index]-48);
    	}
    	else
    	{
    		num += (double) (data[index]-48)/afterDecimalCount;
    	}
    }
    if(data[index] == '.')
    {
    	afterDecimal = true;
    }
    else if(!isOperator(previous) && index != 0)
  {

    if(data[index]<48 || data[index]>=58)
    {
      stillReading = false;
    }

  }
    if(afterDecimal)
    {
    	afterDecimalCount *= 10;
    }
    previous = data[index++];

  }
  index -= 2;
  if(isNegative)
    num *= -1;
  return num;
}
int getPriority(char ator)
{
  int priority = 0;
  if(ator == '(' || ator == ')')
  {
    priority = 0;
  }
  else if (ator == '^')
  {
    priority = 90;
  }
  else if (ator == '*' || ator == '/' || ator == '%' || ator == '$' || ator == 'P' || ator == 'C')
  {
    priority = 80;
  }
  else if (ator == '+' || ator == '-')
  {
    priority = 70;
  }
  else if(ator == '=' || ator == '>' || ator == '<')
  {
	  priority = 60;
  }
  return priority;
}
template <class t>
t add(t &and1,t &and2){	return and1+and2;}
template <class t>
t subtract(t &and1,t &and2){return and1-and2;}
template <class t>
t multiply(t &and1,t &and2){return and1*and2;}
template <class t>
t divide(t &and1,t &and2){int ind;
	  		return and2!=0?and1/and2:getNextDoubleS(MAX,ind);}
template <class t>
t doubleModulus(t &and1,t &and2)
{
	t result;
  	if(and2 != 0)
  	{
  		long quotient = and1/and2;
  		result = and1 - and2*quotient;

  		if(and1*and2 < 0 && result >0)
  		{
  			result *= -1;
  		}
  		else if(and1*and2 > 0 && result <0)
  		{
  			result *= -1;
  		}
  	}
  	else
  	{
  		int ind;
  		result = getNextDoubleS(MAX,ind);
  	}
  	return result;
}
template <class t>
t exponentiate(t &and1,t &and2){return pow(and1,and2);}
template <class t>
t permute(t &and1,t &and2){
	t result = 1;

	for(int i = 0;i<and2 && and1 - i > 1;i++)
	{
		result *= and1 - i;
	}
	return result;
}
template <class t>
t combinations(t &and1,t &and2)
{
	t result = 1;
	for(int i = 0;i<and2 && and1 - i > 1;i++)
	{
		result = (result*(and1 - i))/(and2-i);
	}
	return result;
}
template <class t>
t log(t &and1,t &and2){	return log(and2)/log(and1);}
template <class t>
t rootOp(t &and1,t &and2){	return root(and2,and1);}
template <class t>
t equals(t &and1,t &and2){	return and1==and2;}
template <class t>
t lessThan(t &and1,t &and2){return and1<and2;}
template <class t>
t greaterThan(t &and1,t &and2){	return and1>and2;}

template <class t>
void initOperations()
{
	operations<t>['+'] = add<t>;
	operations<t>['-'] = subtract<t>;
	operations<t>['*'] = multiply<t>;
	operations<t>['/'] = divide<t>;
	operations<t>['%'] = doubleModulus<t>;
	operations<t>['^'] = exponentiate<t>;
	operations<t>['P'] = permute<t>;
	operations<t>['C'] = combinations<t>;
	operations<t>['@'] = log<t>;
	operations<t>['$'] = rootOp<t>;
	operations<t>['='] = equals<t>;
	operations<t>['<'] = lessThan<t>;
	operations<t>['>'] = greaterThan<t>;
}
template <class t>
t calc(char op,t and1,t and2)
{
	t result = 0;
        if(operations<t>.count(op) != 0)
        {
        	result = operations<t>[op](and1,and2);
        }
        else
          result = -400;
    if(*boolsettings["o"])
      std::cout<<and1<<op<<and2<<" = "<<result<<'\n';
    return result;
}

bool isOperator(char s)
{
  return  isNonParentheticalOperator(s) || s == '(' || s == ')';
}
bool isNonParentheticalOperator(char s)
{
    return operations<double>.count(s);
}
bool isNumeric(char c)
{
  return (c >= 48 && c<58);
}
