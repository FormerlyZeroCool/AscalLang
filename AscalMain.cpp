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
//Currently only used for keyword in development comment this, and the call to fork out if you get any trouble
//Commenting this, and the call to fork out won't currently cause any issue as of 2.02
#include <unistd.h>
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
#define PATH_SEPARATOR '/'
#endif

#if DEBUG==1
#define LOG_DEBUG(x)  if(*boolsettings["d"]) {std::cout<<x<<std::endl;}
short stackDepth = 0;
short maxDepth = 0;
#else
#define LOG_DEBUG(x)
#endif

std::string to_string(double input);
const std::string MAX = std::to_string(std::numeric_limits<double>::max());
////////////////////////////////////////////////////////////////////////////////
///////////////Keyword helper functions/object resolution functions
template <class t>
t callOnFrame(AscalFrame<t>* callingFrame,std::string subExp);
template <class t>
t doubleModulus(t &and1,t &and2);
void printVar(const std::string &expr,bool saveLast);
void printHelpMessage(const std::string &expr);
void updateBoolSetting(AscalFrame<double>* frame);
Object* resolveNextExprSafe(AscalFrame<double>* frame, SubStr &varName);
template <typename string_type>
bool isObj(string_type &s);

Object* resolveNextObjectExpressionPartial(AscalFrame<double>* frame, SubStr &varName, Object *o = nullptr);
std::string getListElementAction(AscalFrame<double>* frame, Object&);

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//Start Ascal System Defined  Keyword functionality Executed by lookup in inputMapper unordered_map

SubStr getFollowingExpr(AscalFrame<double>* frame,const std::string &id, char start = '(', char end = ')');
std::string printStrAction(AscalFrame<double>* frame,bool saveLast);
std::string lenAction(AscalFrame<double>* frame,bool saveLast);
std::string splitStrAction(AscalFrame<double>* frame,bool saveLast);
std::string loadStrAction(AscalFrame<double>* frame,bool saveLast);
std::string printCharAction(AscalFrame<double>* frame,bool saveLast);
std::string arrSetValAction(AscalFrame<double>* frame,bool saveLast);
std::string getArrValAction(AscalFrame<double>* frame,bool saveLast);
std::string pushValAction(AscalFrame<double>* frame,bool saveLast);
std::string simplifyfnAction(AscalFrame<double>* frame,bool saveLast);
std::string plotGUIAction(AscalFrame<double>* frame,bool saveLast);
std::string derivefnAction(AscalFrame<double>* frame,bool saveLast);
std::string tryAction(AscalFrame<double>* frame,bool saveLast);
std::string flushOutAction(AscalFrame<double>* frame,bool saveLast);
std::string fprimeAction(AscalFrame<double>* frame,bool saveLast);
std::string fibrcppAction(AscalFrame<double>* frame,bool saveLast);
std::string forRangeAction(AscalFrame<double>* frame,bool saveLast);
std::string srandomAction(AscalFrame<double>* frame,bool saveLast);
std::string randomAction(AscalFrame<double>* frame,bool saveLast);
std::string pauseAction(AscalFrame<double>* frame,bool saveLast);
std::string sleepAction(AscalFrame<double>* frame,bool saveLast);
std::string approxIntAction(AscalFrame<double>* frame,bool saveLast);
std::string printDefaultAction(AscalFrame<double>* frame,bool saveLast);
std::string elseAction(AscalFrame<double>* frame,bool saveLast);
std::string runAction(AscalFrame<double>* frame,bool s);
std::string existsAction(AscalFrame<double>* frame,bool saveLast);
std::string ifAction(AscalFrame<double>* frame,bool saveLast);
std::string printStringAction(AscalFrame<double>* frame,bool s);
std::string inputAction(AscalFrame<double>* frame,bool s);
std::string importAction(AscalFrame<double>* frame,bool s);
std::string setAction(AscalFrame<double>* frame,bool saveLast);
std::string whileAction(AscalFrame<double>* frame,bool saveLast);
std::string plotAction(AscalFrame<double>* frame,bool saveLast);
std::string quitAction(AscalFrame<double>* frame,bool s);
std::string whenAction(AscalFrame<double>* frame,bool saveLast);
std::string clocNewVar(AscalFrame<double>* frame,bool saveLast);
std::string locNewVar(AscalFrame<double>* frame,bool saveLast);
std::string printCommand(AscalFrame<double>* frame,bool saveLast);
std::string printCalculation(AscalFrame<double>* frame,bool saveLast);
std::string constNewVar(AscalFrame<double>* frame,bool saveLast);
std::string letNewVar(AscalFrame<double>* frame,bool saveLast);
std::string deleteObject(AscalFrame<double>* frame,bool saveLast);
std::string undoAction(AscalFrame<double>* frame,bool s);
std::string redoAction(AscalFrame<double>* frame,bool s);
std::string showOpBoolSetting(AscalFrame<double>* frame,bool s);
std::string updateBoolSettingAction(AscalFrame<double>* frame,bool s);
std::string loopBoolSetting(AscalFrame<double>* frame,bool s);
std::string printBoolSetting(AscalFrame<double>* frame,bool s);
std::string debugBoolSetting(AscalFrame<double>* frame,bool s);
std::string timeToRunBoolSetting(AscalFrame<double>* frame,bool s);
std::string arctanAction(AscalFrame<double>* frame,bool saveLast);
std::string arcsinAction(AscalFrame<double>* frame,bool saveLast);
std::string arccosAction(AscalFrame<double>* frame,bool saveLast);
std::string tanAction(AscalFrame<double>* frame,bool saveLast);
std::string sinAction(AscalFrame<double>* frame,bool saveLast);
std::string cosAction(AscalFrame<double>* frame,bool saveLast);
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
SubStr getVarName(const std::string &s,int index);
int getClosingIndex(char opening,const std::string &data);
SubStr getExpr(const std::string &data,int startingIndex,char opening = '{',char closing = '}',char lineBreak = ';');
SubStr getCodeBlock(AscalFrame<double> *frame, int index);
std::vector<std::string> getExprVec(const std::string &data,int index,char opening,char closing,char lineBreak);
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
t calculateExpression(AscalFrame<double>* frame);
template <class t>
t calc(char op,t and1,t and2);
template <class t>
t processStack(stack<t> &operands,stack<char> &operators);

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
void loadFile(const std::string & expr,int startIndex);
double interpretParam(AscalFrame<double>* frame,bool);
double calcWithOptions(AscalFrame<double>* frame);
//End UI handling functions
/////////////////////////////

//list of previous expressions for u command in interpretParam fn
static stack<std::string> lastExp;
//list of previous undone expressions for r command in interpretParam fn
static stack<std::string> undoneExp;
/////////////////////////////
Object& getObject(AscalFrame<double>* frame, std::string &functionName);

static int lineCount = 1;
void get_line(std::istream &in,std::string&data)
{
    getline(in,data);
}

void printLoadedMemMessage(Object function)
{
	if(*boolsettings["o"]){
		std::cout<<"Loaded Function: "<<function.id<<"\nexpression: "<<function.instructionsToFormattedString()<<std::endl<<std::endl;
	}
}

bool containsOperator(std::string s)
{
    bool contains = false;
    int i = 0;
    while(!contains && s[i])
    {
        contains = isOperator(s[i]);
        i++;
    }
    return contains;
}
void loadFn(Object function)
{
        memory[function.id] = function;
        systemDefinedFunctions.push_back(function);
}
template <typename t>
void loadUserDefinedFn(Object &function, t &mem)
{


    if(!containsOperator(function.id))
    {
        mem[function.id] = function;
        if((void*)&mem == (void*)&memory)
        	userDefinedFunctions.push_back(function);
    }
    else
    {
        std::cerr<<"Error loading "<<function.id<<" due to the use of an operator in its name"<<std::endl;
        std::cerr<<"Operators: "<< '=' <<','<< '>' << ',' << '<' <<','<< '$' <<','<<
                'P' <<','<< '@' <<','<< '+' <<','<< '-' <<','<<
                '*'<<','<< '/' <<','<< '^' <<','<< '%' <<','<< 'C';
        throw std::string("Error Operator in Variable Name");
    }
}
void loadInitialFunctions()
{
    //Unary boolean Operations
    loadFn(Object("not","value=0",""));
    loadFn(Object("true","value=0=0",""));
    loadFn(Object("True","1",""));
    loadFn(Object("False","0",""));

    //Calculus Functions
    loadFn(Object("fprime","x*0+(f(x+0.000001)-f(x))/0.000001",""));

    //Trig Functions
    loadFn(Object("sinDeg","sin(toRad(theta))",""));
    loadFn(Object("csc","1/sin(theta)",""));
    loadFn(Object("cosDeg","cos(toRad(theta))",""));
    loadFn(Object("sec","1/cos(theta)",""));
    //Helpful functions
    loadFn(Object("fib","(x){loc counter = 0;loc first = 0;loc second = 1;loc int = 0;while counter<x{set int = second;set second = second+first;set first = int;set counter = counter+1;};first}",""));
    loadFn(Object("fibr","(x){loc fr = (x){when x > 1 then fr(x-1)+fr(x-2) else x end;};loc z = 0;memoize 1;set z = fr(x);memoize 0;z}",""));
    loadFn(Object("rfibr","(x){when x > 1 then rfibr(x-1)+rfibr(x-2) else x end;}",""));
    loadFn(Object("ack","when m=0 + n*0  then n+1 when n=0 then ack(m-1,1) when  m+n > 0 then ack(m-1,ack(m,n-1)) else 0 end",""));
    loadFn(Object("fastAck","(m,n){loc z = 0;memoize 1;set z = ack(m,n);memoize 0;z;}",""));
    loadFn(Object("gcd","(a,b){when b=0 then a when a=0=0 then gcd(b,a%b) else 0 end}",""));
    loadFn(Object("sumBetween","(numberzxa,numberzxb){"
            "when (numberzxb<numberzxa)+(numberzxb=numberzxa) then sumOneTo(numberzxa)-sumOneTo(numberzxb-1)"
            "else sumOneTo(numberzxb)-sumOneTo(numberzxa-1) end}"
            ,""));
    loadFn(Object("sumOneTo","(numberzxa*(numberzxa+1))/2",""));
    //factorial of >= 171 overflows double datatype
    loadFn(Object("rfact",
            "when (i>1)*(i<171) then rfact(i-1)*i when not(i>1) then 1 else 0 end",""));
    loadFn(Object("abs","when numberx<0 then numberx*-1 else numberx end",""));
    loadFn(Object("ln","when argument>0 then e@argument else null end",""));
    loadFn(Object("log","when argument>0 then 10@argument else null end",""));
    loadFn(Object("logbx","base@argument",""));
    loadFn(Object("sqrt","radicand^0.5",""));
    loadFn(Object("fact","when numberzxa<171 then numberzxaPnumberzxa when not(numberzxa<171) then 0 end",""));
    loadFn(Object("dist","sqrt((dx)^2+(dy)^2)",""));
    loadFn(Object("dist3d","sqrt((dx)^2+(dy)^2+(dz)^2)",""));
    loadFn(Object("toDeg","rad*180/pi",""));
    loadFn(Object("toRad","deg*pi/180",""));
    loadFn(Object("println","(x){loc counter = 0;while counter<x{set counter = counter +1;print \"endl\";};null",""));
    loadFn(Object("clear","println(150)",""));
    loadFn(Object("floor","x-x%1",""));
    loadFn(Object("ceiling","when x%1=0 then x else x+1-x%1 end",""));
    loadFn(Object("round","when x%1<0.5 then floor(x) else ceiling(x) end",""));

    //Stats Functions
    Object binProbDist("binprob","(total C events) * probabilityOfSuccess^events * (1-probabilityOfSuccess)^(total-events)","");
    loadFn(binProbDist);

    //Dietitian Functions
    Object eKCal("ekcal","(0*male*kg*cm*age*activity+ true(male)*5+not(male)*-161+(10*kg)+(6.25*cm)-(5*age))*activity","");
    loadFn(eKCal);

    //Constants Definition
    loadFn(Object("pi","3.141592653589793238462643383279",""));
    loadFn(Object("e","2.718281828459045",""));
    loadFn(Object("null",MAX,""));
    loadFn(Object("printf","print \"(x)endl\"",""));
}
void printAllFunctions()
{
    std::cout<<"All Functions in Memory:"<<std::endl;
    for(auto &[key,value] : memory)
    {
        std::cout<<std::endl<<"Function Name: "<<key<<"\nFunction Expression: "<<value.toString()<<std::endl;
    }
    std::cout<<std::endl<<"End of All Functions in Memory."<<std::endl;
}
void printAllUDF(AscalFrame<double> *frame)
{
    std::cout<<"User Defined Functions:"<<std::endl;
    for(Object & data:userDefinedFunctions)
    {
        std::cout<<std::endl<<"Function Name: "<<data.id<<std::endl<<"Function Expression: "<<getObject(frame, data.id).toString()<<std::endl;

    }
    std::cout<<std::endl<<"End of User Defined Functions."<<std::endl;
}
void printAllSDF()
{
    std::cout<<"System Defined Functions:"<<std::endl;
    for(Object & data:systemDefinedFunctions)
    {
        std::cout<<std::endl<<"Function Name: "<<data.id<<std::endl<<"Function Expression: "<<data.instructionsToFormattedString()<<std::endl;
    }
    std::cout<<std::endl<<"End of System Defined Functions."<<std::endl;
}
std::string printVersion(AscalFrame<double>* frame,bool s)
{
    std::cout<<VERSION<<std::endl;
    return MAX;
}
std::string popandAction(AscalFrame<double>* frame,bool s)
{
    frame->initialOperands.pop();
    return MAX;
}
void initParamMapper()
{
	objectActionMapper[ObjectKey("[","")] = getListElementAction;
	inputMapper["simplify"] = simplifyfnAction;
	//inputMapper["this"] = thisAction;
	inputMapper["printStr"] = printStrAction;
	inputMapper["arrLen"] = lenAction;
	inputMapper["splitStr"] = splitStrAction;
	inputMapper["loadStr"] = loadStrAction;
	inputMapper["printChar"] = printCharAction;
	inputMapper["arrSet"] = arrSetValAction;
	inputMapper["arrGet"] = getArrValAction;
	inputMapper["arrPush"] = pushValAction;
	inputMapper["derive"] = derivefnAction;
	inputMapper["plotGUI"] = plotGUIAction;
	inputMapper["flush"] = flushOutAction;
	inputMapper["try"] = tryAction;
	inputMapper["fprime"] = fprimeAction;
	inputMapper["fibrcpp"] = fibrcppAction;
	inputMapper["for"] = forRangeAction;
	inputMapper["pause"] = pauseAction;
	inputMapper["rand"] = randomAction;
	inputMapper["srand"] = srandomAction;
	inputMapper["sleep"] = sleepAction;
	inputMapper["sci"] = updateBoolSettingAction;
	inputMapper["tan"] = tanAction;
	inputMapper["sin"] = sinAction;
	inputMapper["cos"] = cosAction;
	inputMapper["arctan"] = arctanAction;
	inputMapper["arcsin"] = arcsinAction;
	inputMapper["arccos"] = arccosAction;
    inputMapper["popand"] = popandAction;
    inputMapper["version"] = printVersion;
    inputMapper["ver"] = printVersion;
    inputMapper["approxInt"] = approxIntAction;
    inputMapper["run"] = runAction;
    inputMapper["exists"] = existsAction;
    inputMapper["if"] = ifAction;
    inputMapper["else"] = elseAction;
    inputMapper["input"] = inputAction;
    inputMapper["import"] = importAction;
    inputMapper["while"] = whileAction;
    inputMapper["when"] = whenAction;
    inputMapper["plot"] = plotAction;
    inputMapper["quit"] = quitAction;
    inputMapper["set"] = setAction;
    inputMapper["const"] = constNewVar;
    inputMapper["let"] = letNewVar;
    inputMapper["loc"] = locNewVar;
    inputMapper["cloc"] = clocNewVar;
    inputMapper["delete"] = deleteObject;
    inputMapper["u"] = undoAction;
    inputMapper["r"] = redoAction;
    inputMapper["l"] = updateBoolSettingAction;
    inputMapper["o"] = updateBoolSettingAction;
    inputMapper["p"] = updateBoolSettingAction;
    inputMapper["t"] = updateBoolSettingAction;
    inputMapper["memoize"] = timeToRunBoolSetting;
    inputMapper["printc"] = printCommand;
    inputMapper["print"] = printDefaultAction;
    inputMapper["printTxt"] = printStringAction;
#if DEBUG == 1
    inputMapper["d"] = debugBoolSetting;
#endif
}
char toLower(char data)
{
    return data ^ 32*(data < 96);
}
char invertCase(char data)
{
    return 32 ^ data;
}
struct {
	char ** argv;
	int argc;
	int index;
}commandLineParams;
int main(int argc,char* argv[])
{
	commandLineParams.argc = argc;
	commandLineParams.argv = argv;
	commandLineParams.index = 1;
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
  //Beginning of section interpreting program parameters from command line

  std::string arg = "";
    try{
  if(argc > 1)
  {
    //setting for main program loop makes use of overloaded operator = on setting class
    boolsettings["l"] = false;

    for(commandLineParams.index = 1; commandLineParams.index < argc; commandLineParams.index++)
    {
        arg = argv[commandLineParams.index];
        //Auto execute if param specificies a .asl file
        if(arg.size() > 4)
        {
            if(arg[arg.size()-4] == '.' && toLower(arg[arg.size()-3]) == 'a' &&
                    toLower(arg[arg.size()-2]) == 's' && toLower(arg[arg.size()-1]) == 'l')
            {
            	//Exec code in filepath specified in command line parameter
            	try{
                loadFile(arg,0);
            	}catch(std::string &exception)
            	{
                    std::cerr<<"Function call stack trace.\n";
                    std::cerr<<exception<<std::endl;
                    std::cerr<<"End of function call stack trace.\n";
                    std::cerr<<"Failed to exec: "<<arg<<std::endl;
            	}
                continue;
            }
        }
        try{
            FunctionFrame<double>* frame = new FunctionFrame<double>(nullptr,nullptr,nullptr);
            allocated += sizeofFrame;
            frame->exp = arg;
            interpretParam(frame,true);
            lineCount = 0;
    }
    catch(std::string &exception)
    {
        std::cerr<<"Function call stack trace.\n";
        std::cerr<<exception<<std::endl;
        std::cerr<<"End of function call stack trace.\n";
        std::cerr<<"Failed to exec: "<<arg<<std::endl;
    }
    }
  }
  //End of section interpreting program parameters from command line
  //beginning of main program loop, if only one parameter is given in the
  //command line by the user loop will not run, unless that param was the loop command
  //by default the loop runs

  while(std::cin && *boolsettings["l"])
  {
      allocated += sizeofFrame;
          //Interpreter prompt to let user know program is expecting a command/expression
      FunctionFrame<double>* frame = new FunctionFrame<double>(nullptr,nullptr,nullptr);
      std::cout<<std::endl<<">>";
      get_line(std::cin, arg);
      arg = getExpr(arg,0).data;
      frame->exp = arg;
        try{
        	interpretParam(frame, true);
        	lineCount = 0;
        }
        catch(std::string &exception)
        {
        	lineCount = 0;
            std::cerr<<"Function call stack trace.\n";
            std::cerr<<exception<<std::endl;
            std::cerr<<"Failed to exec: "<<arg<<std::endl;
        }
  }
  if(undoneExp.length() > 1 || lastExp.length() > 1)
  {
      std::cout<<std::endl<<"Ascal exited"<<std::endl;
  }
  }
  catch(int exitCode){
      return exitCode;
  }
  return 0;
}
double floor(double input)
{
    double one = 1;
    return input - doubleModulus(input,one);
}

#include <sstream>
template <typename T>
std::string to_string_with_precision(const T a_value, const int n = 10)
{
    std::ostringstream out;
    out.precision(n);
    out << std::fixed << a_value;
    return out.str();
}
std::string to_string(double input)
{
    int offsetFromEnd = 0;
    bool brek = true;
    std::string data = std::to_string(input);
    brek = (data[data.length()-offsetFromEnd-1] == '0');
    offsetFromEnd += brek;
    brek = brek && (data[data.length()-offsetFromEnd-1] == '0');
    offsetFromEnd += brek;
    brek = brek && (data[data.length()-offsetFromEnd-1] == '0');
    offsetFromEnd += brek;
    brek = brek && (data[data.length()-offsetFromEnd-1] == '0');
    offsetFromEnd += brek;
    brek = brek && (data[data.length()-offsetFromEnd-1] == '0');
    offsetFromEnd += brek;
    brek = brek && (data[data.length()-offsetFromEnd-1] == '0');
    offsetFromEnd += brek;
    offsetFromEnd += (data[data.length()-offsetFromEnd-1] == '.');

    return data.substr(0,data.length()-offsetFromEnd);
}
std::string printStringAction(AscalFrame<double>* frame,bool s)
{
        const int startOfPrint = frame->exp.find("\"",frame->index)+1;
        const int endOfPrint = frame->exp.find("\"",startOfPrint);
        if(endOfPrint <= startOfPrint)
            throw std::string("Error, no terminating \" in print string command");

        std::string result;
        if(endOfPrint != -1)
        {
            result = frame->exp.substr(startOfPrint,endOfPrint-startOfPrint);
            int index = 0;
            SubStr subexp("",0,0);
            while(result[index] && result[index] != '\"')
            {
                if(result[index] == '(')
                {
                    SubStr subexp = getExpr(result,index,'(',')',';');
                    std::string value = to_string(callOnFrame(frame, subexp.data));
                    std::string first = result.substr(0,index);
                    std::string last = result.substr(index+subexp.end-2,frame->exp.size());

                    result = first+value+last;
                    index = first.size()+value.size()-1;
                }
                else if(result.size()-index>3 && result[index] == 'e' &&
                        result[index+1] == 'n' && result[index+2] == 'd' && result[index+3] == 'l')
                {
                    result = result.substr(0,index)+'\n'+result.substr(index+4,result.size());
                }
                index++;
            }
            index++;
        }
        else
        {
            result = "";
        }
        std::cout<<result;
        return MAX;
}
std::string inputAction(AscalFrame<double>* frame,bool s)
{
        const int startOfPrint = frame->exp.find("\"")+1;
        int endOfPrint = frame->exp.find("\"",startOfPrint);

        std::string usrPrompt;
        if(endOfPrint != -1)
        {
            usrPrompt = frame->exp.substr(startOfPrint,endOfPrint-startOfPrint);
            int index = 0;
            SubStr subexp("",0,0);
            while(usrPrompt[index] && usrPrompt[index] != '\"')
            {
                if(usrPrompt[index] == '(')
                {
                    SubStr subexp = getExpr(usrPrompt,index,'(',')',';');
                    std::string value = to_string(callOnFrame(frame,subexp.data));
                    std::string first = usrPrompt.substr(0,index);
                    std::string last = usrPrompt.substr(index+subexp.end-3,frame->exp.size());
                    usrPrompt = first+value+last;
                    index = first.size()+value.size()-1;
                }
                else if(usrPrompt[index] == 'e' && usrPrompt.size()-index>3 &&
                        usrPrompt[index+1] == 'n' && usrPrompt[index+2] == 'd' && usrPrompt[index+3] == 'l')
                {
                    usrPrompt = usrPrompt.substr(0,index)+'\n'+usrPrompt.substr(index+4,usrPrompt.size());
                }
                index++;
            }
            index++;
        }
        else
        {
            endOfPrint = frame->index+4;
        }
        std::string input;
        if(commandLineParams.index+1 < commandLineParams.argc)
        {
            input = std::string(commandLineParams.argv[++commandLineParams.index]);
        }
        else
        {
            std::cout<<usrPrompt;
        	std::streambuf* currentBuffer = std::cin.rdbuf();
            std::cin.rdbuf(stream_buffer_cin);
            get_line(std::cin,input);
            std::cin.rdbuf(currentBuffer);
            if(input.size()==0)
            	throw 0;
        }
        frame->initialOperands.push(callOnFrame(frame,input));
        if(*boolsettings["o"])
          std::cout<<"Received "<<input<<" as input.\n";
    return "a"+frame->exp.substr(endOfPrint+1,frame->exp.size());

}
void loadFile(const std::string &expr,int startIndex)
{
    std::ifstream inputFile;
    while(expr[startIndex] == ' ' || expr[startIndex] == ':')
        startIndex++;
    std::string filePath = expr.substr(startIndex,expr.find(';')-startIndex);
    inputFile.open(filePath);
    if(!inputFile)
    {
        inputFile.open(filePath+".asl");
    }
    if(!inputFile)
    {
        std::vector<char> data;
        for(char &c:filePath)
        {
            if(c == '.')
                c = (PATH_SEPARATOR);
        }
        if(filePath.size() > 4 &&
        		filePath[filePath.size()-4] == PATH_SEPARATOR && toLower(filePath[filePath.size()-3]) == 'a' &&
                    toLower(filePath[filePath.size()-2]) == 's' && toLower(filePath[filePath.size()-1]) == 'l')
			filePath[filePath.size()-4] = '.';
        inputFile.open(filePath);
        if(!inputFile)
        {
            inputFile.open(filePath+".asl");
        }
        if(!inputFile)
        {
        	inputFile.open(filePath.substr(0, filePath.size()-1)+".asl");
        }
    }
    if(!inputFile)
    {
        throw std::string("Malformed path: "+filePath);
    }
    if(*boolsettings["o"])
    {
    	std::cout<<"Loading file: "<<filePath<<"\n";
    }
    std::streambuf* cinrdbuf = std::cin.rdbuf();
    std::cin.rdbuf(inputFile.rdbuf());
    int locLineCount = lineCount;
    lineCount = 0;
    FunctionFrame<double>* calledFunctionMemory = new FunctionFrame<double>(nullptr,nullptr,nullptr);
    allocated += sizeofFrame;
    calledFunctionMemory->setIsDynamicAllocation(false);
    while(true)
    {
        get_line(inputFile, calledFunctionMemory->exp);
    	if(!inputFile)
    		break;
        //reset so it is like we are executing a new frame with shared memory, and if/else flag state
        calledFunctionMemory->index = 0;
        calledFunctionMemory->level = 0;
        calledFunctionMemory->memoPointer = 0;
        calledFunctionMemory->setIsFirstRun(true);
        calledFunctionMemory->setAugmented(false);
        //finish reset
        lineCount++;
        try{
        	uint32_t i = 0;
        	while(calledFunctionMemory->exp[i] && calledFunctionMemory->exp[i] == ' ')
        		i++;
        	if(calledFunctionMemory->exp[i] != '#')
        	{
        		//evaluate statement as an expression
        		calculateExpression<double>(calledFunctionMemory);
        	}
        }
        //catch only ascal runtime exceptions, not exit signal exceptions, or anything from C++
        catch(std::string &exception)
        {
            std::cerr<<exception<<std::endl;
            lineCount = locLineCount;
        }
    }
    deallocated += sizeofFrame;
    delete calledFunctionMemory;
    inputFile.close();
    std::cin.rdbuf(cinrdbuf);
    if(*boolsettings["o"])
    {
    	std::cout<<"Finished loading file: "<<filePath<<"\n";
    }
}

std::string importAction(AscalFrame<double>* frame,bool s)
{
    try{
        if(*boolsettings["o"])
        {
        	std::cout<<"Attempting to import file\n";
        }
        loadFile(frame->exp,7);
        if(*boolsettings["o"])
        {
        	std::cout<<"Finished importing file\n";
        }
    }catch(std::string &exception)
    {
        throw std::string("while importing file\n"+exception);
    }
    return MAX;
}
std::string runAction(AscalFrame<double>* frame,bool s)
{
    try{
        if(*boolsettings["o"])
        {
        	std::cout<<"Attempting to run file\n";
        }
        loadFile(frame->exp,4);
        if(*boolsettings["o"])
        {
        	std::cout<<"Finished running file\n";
        }
    }catch(std::string &exception)
    {
        throw std::string("while running file\n"+exception);
    }
    return MAX;
}
/*
std::string derivativeAction(const std::string &expr,std::map<std::string,Object>& localMemory,
        AscalParameters &params, std::map<std::string,Object> &paramMemory,bool s)
{

}*/
std::string showOpBoolSetting(AscalFrame<double>* frame,bool s)
{
    updateBoolSetting(frame);
    return MAX;
}
std::string updateBoolSettingAction(AscalFrame<double>* frame,bool s)
{
    updateBoolSetting(frame);
    return MAX;
}
std::string loopBoolSetting(AscalFrame<double>* frame,bool s)
{
    updateBoolSetting(frame);
    return MAX;
}
std::string printBoolSetting(AscalFrame<double>* frame,bool s)
{
    updateBoolSetting(frame);
    return MAX;
}
std::string debugBoolSetting(AscalFrame<double>* frame,bool s)
{
    updateBoolSetting(frame);
    return MAX;
}

std::string timeToRunBoolSetting(AscalFrame<double>* frame,bool s)
{
    updateBoolSetting(frame);
    return MAX;
}
std::string quitAction(AscalFrame<double>* frame,bool s)
{
    if(*boolsettings["o"])
        std::cout<<"Quitting Ascal, have a nice day!"<<std::endl;
    throw 0;
    return MAX;
}
std::string redoAction(AscalFrame<double>* frame,bool s)
{
    std::string value = "0";
    if(!undoneExp.isEmpty()){
        std::string last = "";
        undoneExp.top(last);
        undoneExp.pop();
        lastExp.push(last);
        std::cout<<last<<std::endl;
        value = callOnFrame(frame,"print "+last);
    }
    else
        std::cout<<"No statements can be redone"<<std::endl;
    return MAX;
}
std::string undoAction(AscalFrame<double>* frame,bool s)
{
    double value = 0;
    if(!lastExp.isEmpty()){
        std::string last = "";
        lastExp.top(last);
        lastExp.pop();
        undoneExp.push(last);
        std::cout<<last<<std::endl;
        value = callOnFrame(frame,"print "+last);
    }
    else
        std::cout<<"No previous statements"<<std::endl;
    return MAX;
}
std::string deleteObject(AscalFrame<double>* frame,bool saveLast)
{

    if(cmpstr(frame->exp.substr(7,3),"all"))
    {
        memory.clear();
        frame->getLocalMemory()->clear();
        userDefinedFunctions.clear();
        if(*boolsettings["o"])
        	std::cout<<"All Memory cleared."<<std::endl;
    }
    else
    {
        std::string name = getVarName(frame->exp,7).data;

        if(memory.count(name) > 0)
        {

            std::vector<Object>::iterator position = std::find(userDefinedFunctions.begin(), userDefinedFunctions.end(), memory[name]);
            if(position != userDefinedFunctions.end())
                userDefinedFunctions.erase(position);

            memory.erase(name);
            if(*boolsettings["o"])
            	std::cout<<"Erased "<<name<<" from memory."<<std::endl;
        }
        else if(frame->getLocalMemory()->count(name) != 0)
        {
            frame->getLocalMemory()->erase(name);
        }
        else
        {
            throw std::string("Error could not find "+name+" in memory to delete.\n");
        }
    }
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
bool isDouble(std::string &exp);
std::string letNewVar(AscalFrame<double>* frame,bool saveLast)
{
                SubStr exPart = getExpr(frame->exp,frame->exp.find('=',frame->index)+1);
                SubStr newVarPart = getVarName(frame->exp, frame->index+4);
                Object *parent = resolveNextObjectExpressionPartial(frame, newVarPart);
                Object var(newVarPart.data,exPart.data,frame->exp.substr(newVarPart.end + 1,exPart.start - 1 ));
                if(parent)
                {
                	if(isDouble(var.id))
                		parent->setList(var, stoi(var.id));
                	else
                		parent->loadChild(var);
                }
                else if(memory.count(var.id) != 0)
                {
                    std::vector<Object>::iterator position = std::find(userDefinedFunctions.begin(), userDefinedFunctions.end(), memory[var.id]);
                    if(position != userDefinedFunctions.end())
                        userDefinedFunctions.erase(position);
                }
                //set var defined's value in hashmap
                loadUserDefinedFn(var, memory);
                if(*boolsettings["o"])
                {
                    std::cout<<std::endl<<"New global function: "<<var.id
                    		<< " exp: "<<var.instructionsToFormattedString()<<std::endl;
                }
                return MAX;
}
std::string constNewVar(AscalFrame<double>* frame,bool saveLast)
{
	SubStr exPart = getExpr(frame->exp,frame->exp.find('=',frame->index)+1);
	    SubStr newVarPart = getVarName(frame->exp,frame->index+5);
        Object *parent = resolveNextObjectExpressionPartial(frame, newVarPart, frame->getContext()->getThis());
	    Object var(newVarPart.data,"","");
	    Object *newObj = nullptr;

        if(parent)
        {
        	if(isDouble(var.id))
        		newObj = &parent->setList(var, stoi(var.id));
        	else
        		newObj = &parent->loadChild(var);

        	Object *c = frame->getContext();
        	frame->setContext(newObj);
    	    std::string value = to_string(callOnFrame(frame, exPart.data));
        	frame->setContext(c);
    	    newObj->getInstructions() = value;
        }
        else
        {
    	    std::string value = to_string(callOnFrame(frame, exPart.data));
    	    var.getInstructions() = value;
        	//delete old  reference from list not really needed
    	    std::vector<Object>::iterator position = std::find(userDefinedFunctions.begin(), userDefinedFunctions.end(), memory[var.id]);
    	                            if(position != userDefinedFunctions.end())
    	                                userDefinedFunctions.erase(position);
    	    //set var defined's value in hash map
    	    loadUserDefinedFn(var, memory);
        }
	    if(*boolsettings["o"])
	    {
	        std::cout<<std::endl<<"New global var: "<<newVarPart.data<< " = "
	        		<<var.instructionsToFormattedString()<<std::endl;
	    }
	    return MAX;
}
std::string locNewVar(AscalFrame<double>* frame,bool saveLast)
{
    SubStr localName = getVarName(frame->exp,frame->exp.find("loc",frame->index)+4);
    static uint32_t startOfExp;
    startOfExp = frame->exp.find('=')+1;
    SubStr subexp = getExpr(frame->exp,startOfExp);
    if(!isObj(subexp.data))
    {
    	Object newLocal = Object(localName.data,subexp.data,"");
        loadUserDefinedFn(newLocal, (*frame->getLocalMemory()));
        if(*boolsettings["o"])
        {
            std::cout<<std::endl<<"New Local function: "<<localName.data<< " exp: "<<newLocal.instructionsToFormattedString()<<std::endl;
        }
    }
    else
    {
    	SubStr rightVarStart = getVarName(frame->exp,startOfExp);
    	Object *newVar = resolveNextExprSafe(frame, rightVarStart);
        loadUserDefinedFn(*newVar, (*frame->getLocalMemory()));
        if(*boolsettings["o"])
        {
            std::cout<<std::endl<<"New Local function: "<<localName.data<< " exp: "<<newVar->toString()<<std::endl;
        }
    }
    return MAX;
}
std::string clocNewVar(AscalFrame<double>* frame,bool saveLast)
{
    SubStr localName = getVarName(frame->exp,frame->exp.find("loc",frame->index)+4);
    SubStr subexp = getExpr(frame->exp,frame->exp.find('=',frame->index)+1);
    std::string value = to_string(callOnFrame(frame, subexp.data));
    Object newLocalVar(localName.data,value,"");
    if(*boolsettings["o"])
    {
        std::cout<<std::endl<<"New local var: "<<localName.data<< " = "<<value<<std::endl;
    }
    loadUserDefinedFn(newLocalVar, (*frame->getLocalMemory()));
    return MAX;
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
std::vector<std::string> split(std::string &s, std::string &&delimiter)
{
	std::vector<std::string> result;
	size_t pos = 0;
	while ((pos = s.find(delimiter)) != std::string::npos) {
	    result.push_back(s.substr(0, pos));
	    s.erase(0, pos + delimiter.length());
	}
	result.push_back(s);
	return result;
}

std::string simplifyfnAction(AscalFrame<double>* frame,bool saveLast)
{
	static const std::string keyWord = "simplify";
    SubStr exp = getFollowingExpr(frame, keyWord);
    if(exp.data.size() == 1)
    	throw std::string("simplify <function>");
    //Get definition by calling function.getIntsructions(), or for formatted version getInstructionsFormated
    Object* function;
    try{
        SubStr vns = getVarName(frame->exp, frame->index+keyWord.size());
    	function = resolveNextExprSafe(frame, vns);
    }catch(std::string &objNotFoundErr)
    {
    	throw std::string("Error could not find function: " + exp.data + " to simplify.");
    }
    //your code

    //end of your code is defining simplifiedfn
    std::string simplifiedfn;
    std::stringstream call;
    call<<"set "<<exp.data<<" = "<<simplifiedfn;
    callOnFrame(frame, call.str());
	return MAX;
}
std::string derivefnAction(AscalFrame<double>* frame,bool saveLast)
{
	//Feel free to change anything, I've just added the boilerplate, and some convenience code for you
	static const std::string keyWord = "derive";
    SubStr exp = getFollowingExpr(frame, keyWord);
    std::vector<SubStr> params = Object("","",exp.data).params;
    //you can change this, just getting some params for you
    if(params.size()<3)
    	throw std::string("derive <function name, variable to differentiate, derivative degree>");
    //id is the function name, getInstructions() will return a reference to the code saved for the function as a string
    SubStr vns = getVarName(frame->exp, frame->index+keyWord.size()+params[0].start);
    Object *function = resolveNextExprSafe(frame, vns);
    std::string withRespectTo = params[1].data;
    uint16_t degree = (uint16_t) callOnFrame(frame, params[2].data);
    //Your code here

    //end of your code is defining the string derivative
    std::string derivative;
    std::stringstream call;
    call<<"let "<<function->id<<"prime"<<degree<<" = "<<function->getInstructions();
    //Saves function that when it is a first derivative of f looks like
    //fprime1 = first derivative of f
    callOnFrame(frame, call.str());
    //Automatically moves to next expression after keyword execution because we return MAX
    return MAX;
}
//Rummy you'll probably want to use this function
Vect2D<double> calcTable(const std::vector<std::string> &functions, double xMin, double xMax, double xStepSize, double yStepSize);
std::string plotGUIAction(AscalFrame<double>* frame,bool saveLast)
{
	static const std::string keyWord = "plotGUI";
    SubStr exp = getFollowingExpr(frame, keyWord);
    std::vector<SubStr> params = Object("","",exp.data).params;
    if(params.size() < 7)
    	throw std::string("plotGUI <fun1|fun2...>,<x lower bound>, <x upper bound>, <y upper bound>, <y lower bound>, <delta x, delta y>");
    std::vector<std::string> functions = split(params[0].data, "|");
    //Here to aid potential future development
    double xMin = callOnFrame(frame, params[1].data);
    double xMax = callOnFrame(frame, params[1].data);
    double yMin = callOnFrame(frame, params[1].data);
    double yMax = callOnFrame(frame, params[1].data);
    double dx = callOnFrame(frame, params[1].data);
    double dy = callOnFrame(frame, params[1].data);

    //int pid = fork();
    //if(!pid)
    {
    	//Essentially a 1d vector but you can address it with x, and y coordinates, y addresses different functions
    	//x addresses the x point xMin+dx*x, using xMin, and dx defined above
    	Vect2D<double> points = calcTable(functions, xMin, xMax, dx, dy);
    	//Your GUI code
    	//throw 0;
    }
    if(*boolsettings["o"])
    {
    	std::cout<<"plotGUI"+exp.data.substr(0,exp.data.size()-2)+"\n";
    }
    //Automatically moves to next expression after keyword execution because we return MAX
    return MAX;
}
//For plotGUI to use the y index of the Vect2D corresponds to the index of the function name in the functions vector
//the x index in cartesian space is x*dx+xMin
Vect2D<double> calcTable(const std::vector<std::string> &functions, double xMin, double xMax, double xStepSize, double yStepSize)
{
    int tableWidth = (xMax-xMin)/(xStepSize>0?xStepSize:1);
	double dx = (xMax-xMin)/tableWidth;
	    double dy = yStepSize>0?yStepSize:1;
	    double xi;
	    Vect2D<double> outPuts(tableWidth,functions.size()-1);
	    std::stringstream exp;
	    for(int j = 0;j<functions.size();j++)
	    {
	        std::string function = functions[j];
	        for(int i = 0;i<tableWidth;i++)
	        {
	            xi = xMin+dx*(i);
	            allocated += sizeofFrame;
	            FunctionFrame<double>* calledFunction = new FunctionFrame<double>(nullptr,nullptr,nullptr);
	            exp << function << '(' << to_string(xi) << ')';
	            calledFunction->exp = exp.str();
	            exp.str(std::string());
	            outPuts.push_back(
	                    calculateExpression<double>(calledFunction));
	        }
	    }
	    return outPuts;
}

std::string plotAction(AscalFrame<double>* frame,bool saveLast)
{
    const int plotKeyWordIndex = frame->exp.find("plot",frame->index);
    const int endOfFun = frame->exp.find(",", plotKeyWordIndex);
    const int endOfDomain = frame->exp.find(",",endOfFun+1);
    std::vector<double> sumArea;
    sumArea.push_back(0);
    int index = plotKeyWordIndex+4;
    while(frame->exp[index] && frame->exp[index] == ' ')
    {
        index++;
    }
    std::vector<std::string> functions;
    int trailer = index;
    {
        while(frame->exp[index] && frame->exp[index] != ',')
        {
            if(frame->exp[index] == '|')
            {
                functions.push_back(frame->exp.substr(trailer,index-(trailer)));
                sumArea.push_back(0);
                trailer = index+1;
            }
                index++;
        }
    }
    functions.push_back(frame->exp.substr(trailer,endOfFun - (trailer)));
    index = endOfFun+1;
    const double xMin = getNextDoubleS(frame->exp,index);
    index+=2;
    const double xMax = getNextDoubleS(frame->exp,index);
    index = endOfDomain+1;
    const double yMin = getNextDoubleS(frame->exp,index);
    index +=2;
    const double yMax = getNextDoubleS(frame->exp,index);
    index +=2;
    const double xStepSize = getNextDoubleS(frame->exp,index);
    index += 2;
    const double yStepSize = getNextDoubleS(frame->exp,index);
    int tableWidth = (xMax-xMin)/(xStepSize>0?xStepSize:1);
    int tableHeight = (yMax-yMin)/(yStepSize>0?yStepSize:1);
    double dx = (xMax-xMin)/tableWidth;
    double dy = yStepSize>0?yStepSize:1;
    double xi;
    Vect2D<double> outPuts(tableWidth,functions.size()-1);
    std::stringstream exp;
    for(int j = 0;j<functions.size();j++)
    {
        std::string function = functions[j];
        if(*boolsettings["o"])
        	std::cout<<"\nProcessing: "<<function<<"\n";
        for(int i = 0;i<tableWidth;i++)
        {
            xi = xMin+dx*(i);
            allocated += sizeofFrame;
            FunctionFrame<double>* calledFunction = new FunctionFrame<double>(nullptr,nullptr,nullptr);
            exp << function << '(' << to_string(xi) << ')';
            calledFunction->exp = exp.str();
            exp.str(std::string());
            outPuts.push_back(
                    calculateExpression<double>(calledFunction));
            sumArea[j] += outPuts.get(i,j)*dx;
        }
    }

    double yClosestToZero = yMax;
    for(int y = tableHeight+1;y>=0;y--)
    {
        if(std::abs(yMin+dy*y)<yClosestToZero)
            yClosestToZero = yMin+dy*y;
    }
    double xClosestToZero = xMax;
    std::cout<<"\n";
    for(int i = 0;i<=tableWidth;i++)
    {
        xi = (dx*i+xMin);

        if(std::abs(xi) < std::abs(xClosestToZero))
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
    char symbols[10] = {'*','#','%','@','$','&','=','+','-','!'};
    for(double y = tableHeight+1;y>=0;y--)
    {

        for(int x = 0;x<tableWidth;x++)
        {
            bool drawNonFn = true;
            for(int z = outPuts.getHeight(); z>=0;z--)
            {
                const double roundedFunctionOutput = (std::round((outPuts.get(x,z) - yMin) / dy) * dy) + yMin;
                bool drawFnValue = true;
                //this inner loop is to check if another symbol would have been printed for this point
                //runs n^2 where n is basically limited to between 1 and 10, but could def be optimized, and it's run a lot at once
                for(int a =0;drawFnValue && a < z;a++)
                {
                    drawFnValue = (roundedFunctionOutput != (std::round((outPuts.get(x,a) - yMin) / dy) * dy) + yMin);
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
        std::cout<<" "<<std::round((y*dy+yMin)*10000)/10000<<"\n";
    }

    for(int x = 0;x<tableWidth;x++)
    {
        std::cout<<"|";
    }
    std::cout<<"\n"<<frame->exp<<"\n";
    std::cout<<"domain:"<<xMin<<" to "<<xMax<<", range:"<<
            yMin<<" to "<<yMax<<" with a step size in the x of:"<<xStepSize<<", and in the y: "<<yStepSize<<"\n";
    for(int i =0; i<functions.size();i++)
    {
        std::cout<<"Function: "<<functions[i]<<", plotted using symbol: "<<symbols[i%10]<<", function defined as: "<<memory[functions[i]].instructionsToFormattedString();
        std::cout<<"Area Under Curve calculated with reimann sum using "<<tableWidth<<" partitions: "<<sumArea[i]<<"\n\n";
    }

    return MAX;
}

std::string approxIntAction(AscalFrame<double>* frame,bool saveLast)
{
    const int plotKeyWordIndex = frame->exp.find("approxInt",frame->index);
    const int endOfFun = frame->exp.find(",",frame->index);
    const int endOfDomain = frame->exp.find(",",endOfFun+1);
    std::vector<double> sumArea;
    sumArea.push_back(0);
    int index = plotKeyWordIndex+9;
    while(frame->exp[index] && frame->exp[index] == ' ')
    {
        index++;
    }
    std::vector<std::string> functions;
    int trailer = index;
    {
        while(frame->exp[index] && frame->exp[index] != ',')
        {
            if(frame->exp[index] == '|')
            {
                functions.push_back(frame->exp.substr(trailer,index-(trailer)));
                sumArea.push_back(0);
                trailer = index+1;
            }
                index++;
        }
    }
    functions.push_back(frame->exp.substr(trailer,endOfFun - (trailer)));
    index = endOfFun + 1;
    //getNextDouble updates index to be the character at the end of the double
    const double xMin = getNextDoubleS(frame->exp,index);
    index += 2;
    const double xMax = getNextDoubleS(frame->exp,index);
    index = endOfDomain + 1;
    const int tableWidth = getNextDoubleS(frame->exp,index);
    index += 2;
    const char methodIdentifier = frame->exp[index];
    if(methodIdentifier != 'l' && methodIdentifier != 't' && methodIdentifier != 'r')
    	index -=2;
    const double xStepSize = (xMax-xMin)/(tableWidth>0?tableWidth:1);
    double dx = (xMax-xMin)/tableWidth;
    double xi;
    double thisIndex = 0;
    double lastIndex = 0;
    std::stringstream exp;
    for(int j = 0;j<functions.size();j++)
    {
        std::string function = functions[j];
        for(int i = 0;i<tableWidth;i++)
        {
            xi = xMin+dx*(i+1);
            allocated += sizeofFrame;
            FunctionFrame<double>* calledFunction = new FunctionFrame<double>(nullptr,nullptr,nullptr);
            exp << function << '(' << to_string(xi) << ')';
            calledFunction->exp = exp.str();
            exp.str(std::string());
            thisIndex = calculateExpression<double>(calledFunction);//outPuts.get(i,j)*dx;

            sumArea[j] +=
                    //Trapezoidal this is the defaulr
                    ((methodIdentifier != 'r' && methodIdentifier != 'l') * ((thisIndex + lastIndex) * dx / 2)) +
                    //left Riemann sum
                    ((methodIdentifier == 'l') * dx*lastIndex) +
                    //right Riemann sum
                    ((methodIdentifier == 'r') * dx*thisIndex);

            lastIndex = thisIndex;
        }
    }
    std::string calculationType;

    if(methodIdentifier == 'r')
    {
        calculationType = "right handed";
    }
    else if(methodIdentifier == 'l')
    {
        calculationType = "left handed";
    }
    else
    {
        calculationType = "trapezoidal";
    }
    std::cout<<"\n"<<frame->exp<<"\n";
    std::cout<<"domain:"<<xMin<<" to "<<xMax<<" with a step size in the x of: "<<xStepSize<<"\n";
    for(int i =0; i<functions.size();i++)
    {
        std::cout<<"Function: "<<functions[i]<<", function defined as: "<<memory[functions[i]].instructionsToFormattedString();
        std::cout<<"Area Under Curve calculated with "<<calculationType<<" Reimann sum using "<<tableWidth<<" partitions: "<<sumArea[i]<<"\n\n";
    }

    return "a"+frame->exp.substr(index+1);
}
std::string existsAction(AscalFrame<double>* frame,bool saveLast)
{
    int index = frame->exp.find("exists",frame->index)+6;
    while(frame->exp[index++] == ' '){}
    SubStr varName = getVarName(frame->exp,index);
    index += varName.data.length();

    if(frame->getLocalMemory()->count(varName.data) || frame->getParamMemory()->count(varName.data) || memory.count(varName.data))
    {
    	frame->initialOperands.push(1);
        return "a"+frame->exp.substr(index,frame->exp.size());
    }
	frame->initialOperands.push(0);
    return "a"+frame->exp.substr(index,frame->exp.size());
}

SubStr getExprInString(const std::string &data,int index,char opening,char closing,char lineBreak);

SubStr getFollowingExpr(AscalFrame<double>* frame,const std::string &id, char start, char end)
{
	int index = frame->index+id.length();
	while(frame->exp[index] == ' ')
		index++;
	SubStr exp = getExprInString(frame->exp,index, start, end,'\1');
	index += exp.data.length()>3?exp.data.length()-3:0;
	exp.end = index;
    index = frame->index+id.length();
    return exp;
}
Object& getObject(AscalFrame<double>* frame, std::string &functionName)
{

	if(frame->getLocalMemory()->count(functionName))
	            {
	                return (*frame->getLocalMemory())[functionName];
	            }
	            else if(frame->getParamMemory()->count(functionName))
	            {
	                return *(*frame->getParamMemory())[functionName];
	            }
	            else if(memory.count(functionName))
	            {
	                return memory[functionName];
	            }
	            else
	            {
	                throw std::string("Error locating object "+functionName+"\n");
	            }
}
Object* getObjectNoError(AscalFrame<double>* frame, std::string &functionName)
{

	if(frame->getLocalMemory()->count(functionName))
	            {
	                return &(*frame->getLocalMemory())[functionName];
	            }
	            else if(frame->getParamMemory()->count(functionName))
	            {
	                return (*frame->getParamMemory())[functionName];
	            }
	            else if(memory.count(functionName))
	            {
	                return &memory[functionName];
	            }
	            else
	            {
	                return nullptr;
	            }
}
std::string objectAction(AscalFrame<double>* frame,bool saveLast)
{
	frame->index += 5;
	SubStr varName = getVarName(frame->exp, frame->index);
	frame->index += varName.data.length();

	ObjectKey key(frame->exp.substr(frame->index, frame->index), varName.data);
	Object *obj = resolveNextExprSafe(frame, varName);
	frame->initialOperands.push(callOnFrame(frame, objectActionMapper[key](frame, *obj)));
	return 'a'+frame->exp.substr(frame->index);
}
std::string getListElementAction(AscalFrame<double>* frame, Object &obj)
{
	SubStr expIndex = getExpr(frame->exp,frame->index,'[',']');
	frame->index += expIndex.data.length();
	while(frame->exp[frame->index] == ' ')
	{
		frame->index++;
	}
	if(frame->exp[frame->index] == '(')
	{
		SubStr params = getExpr(frame->exp,frame->index, '(', ')');
		frame->index += params.data.length();

	}
	return obj[expIndex.data].getInstructions();
}

std::string fprimeAction(AscalFrame<double>* frame,bool saveLast)
{
	static const std::string keyWord = "fprime";
    SubStr exp = getFollowingExpr(frame, keyWord);
    Object data("","","");
    data.setParams(exp.data);
    if(data.params.size() < 2)
    	throw std::string("Error missing params for fprime <function name> <value to evaluate at>");
    std::string fn = data.params[0].data;
    std::string x = data.params[1].data;
    double functionValue = callOnFrame(frame,fn+"("+x+")");
    double steppedFunctionValue = callOnFrame(frame,fn+"("+x+"+0.00001)");
    frame->initialOperands.push((steppedFunctionValue-functionValue)/0.00001);
    if(*boolsettings["o"])
    {
    	std::cout<<fn<<"("<<x<<"+0.00001) = "<<steppedFunctionValue<<" - "<<fn<<"("<<x<<") = "<<functionValue;
    	std::cout<<fn<<"prime("<<exp.data<<") = "<<(steppedFunctionValue-functionValue)/0.00001<<'\n';
    }
    return 'a'+frame->exp.substr(exp.end,frame->exp.size());
}
std::string pushValAction(AscalFrame<double>* frame,bool saveLast)
{
	static const std::string keyWord = "arrPush";
    SubStr exp = getFollowingExpr(frame, keyWord);
    std::vector<SubStr> params = Object("","",exp.data).params;
    if(params.size() < 2)
    	throw std::string("arrPush(<object/array>,<value as Ascal expression>)");
    double valueToPush = callOnFrame(frame,params[1].data);
    SubStr vns = getVarName(frame->exp, frame->index+keyWord.size()+params[0].start);
    Object *list = resolveNextExprSafe(frame, vns);
    list->pushList(Object("",to_string(valueToPush),""));
    if(*boolsettings["o"])
    {
    	std::cout<<"pushed value "<<params[1].data<<" to list "<<params[0].data<<" at position: "<<(list->getListSize())<<"\n";
    }
    return 'a'+frame->exp.substr(exp.end,frame->exp.size());
}
std::string getArrValAction(AscalFrame<double>* frame,bool saveLast)
{
	static const std::string keyWord = "arrGet";
    SubStr exp = getFollowingExpr(frame, keyWord);
    std::vector<SubStr> params = Object("","",exp.data).params;
    if(params.size() < 2)
    	throw std::string("arrGet(<array>,<Index as Ascal expression>)");
    double indexToGet = callOnFrame(frame,params[1].data);
    SubStr vns = getVarName(frame->exp, frame->index+keyWord.size()+params[0].start);
    Object *element = &resolveNextExprSafe(frame, vns)->getListElement(indexToGet, memory);
    SubStr paramsForListElementFn("", 0 , exp.end);
    if(exp.data[exp.end] == '(')
    {
    	frame->index = exp.end;
    	paramsForListElementFn = getFollowingExpr(frame, "");
    }
    std::stringstream fnDef;
    fnDef<<"loc "<<element->id<<"="<<element->getInstructions()<<";\n"<<element->id<<'('<<paramsForListElementFn.data<<')';

    frame->initialOperands.push(callOnFrame(frame,fnDef.str()));

    callOnFrame(frame, "delete "+element->id);
    if(*boolsettings["o"])
    {
    	double t;
    	frame->initialOperands.top(t);
    	std::cout<<"got element at index: "<<params[1].data<<" from list "<<params[0].data<<" value: "<<(t)<<"\n";
    }
    return 'a'+frame->exp.substr(paramsForListElementFn.end,frame->exp.size());
}
std::string arrSetValAction(AscalFrame<double>* frame,bool saveLast)
{
	static const std::string keyWord = "arrSet";
    SubStr exp = getFollowingExpr(frame, keyWord);
    std::vector<SubStr> params = Object("","",exp.data).params;
    if(params.size() < 3)
    	throw std::string("arrGet(<array>,<Index as Ascal expression>,<value as expression>)");
    double indexToSet = callOnFrame(frame,params[1].data);
    double value = callOnFrame(frame,params[2].data);
    Object newObj("",to_string(value),"");
    SubStr vns = getVarName(frame->exp, frame->index+keyWord.size()+params[0].start);
    resolveNextExprSafe(frame, vns)->setList(newObj, indexToSet);
    memory["null"].getInstructions() = MAX;
    if(*boolsettings["o"])
    {
    	double t;
    	frame->initialOperands.top(t);
    	std::cout<<"got element at index: "<<params[1].data<<" from list "<<params[0].data<<" value: "<<(t)<<"\n";
    }
    return 'a'+frame->exp.substr(exp.end,frame->exp.size());
}
std::string printCharAction(AscalFrame<double>* frame,bool saveLast)
{
    SubStr exp = getFollowingExpr(frame, "printChar");
    char input = callOnFrame(frame,exp.data);
    std::cout<<( input);
    if(*boolsettings["o"])
    {
    	std::cout<<"printChar("<<input<<") = "<<(char)(input)<<'\n';
    }
    return 'a'+frame->exp.substr(exp.end,frame->exp.size());
}
std::string loadStrAction(AscalFrame<double>* frame,bool saveLast)
{
	static const std::string keyWordName = "loadStr";
    SubStr exp = getFollowingExpr(frame, keyWordName);
    std::vector<SubStr> params = Object("","",exp.data).params;
    if(params.size() < 2)
    	throw std::string("loadStr (<Object name>, <\"string in quotes\">");
    SubStr vns = getVarName(frame->exp, frame->index+keyWordName.size()+params[0].start);
    Object *obj = resolveNextExprSafe(frame, vns);
    obj->clearList();
    //parse only text between quotes
    size_t strStart = 0;
    while(params[1].data[strStart] && params[1].data[strStart++] != '\"'){}
    size_t strEnd = params[1].data.length()-1;
    while(params[1].data[strEnd] && params[1].data[strEnd--] != '\"'){}
    //at this point str start, and end point to the appropriate places in the parameter
    string_view s((char*)params[1].data.c_str()+strStart, strEnd);
    obj->loadString(s);

    if(*boolsettings["o"])
    {
    	std::cout<<"loadstr("<<") = "<<(char)(65)<<'\n';
    }
    return MAX;
}
std::string printStrAction(AscalFrame<double>* frame,bool saveLast)
{
	static const std::string keyWord = "printStr";
    SubStr exp = getFollowingExpr(frame, keyWord);
    std::vector<SubStr> params = Object("","",exp.data).params;
    if(params.size() < 1)
    	throw std::string("loadStr (<object name>)");
    SubStr vns = getVarName(frame->exp, frame->index+keyWord.size()+params[0].start);
    Object *obj = resolveNextExprSafe(frame, vns);
    obj->printList(memory);
    if(*boolsettings["o"])
    {
    	std::cout<<"loadstr("<<") = "<<(char)(65)<<'\n';
    }
    return MAX;
}
std::string splitStrAction(AscalFrame<double>* frame,bool saveLast)
{
	static const std::string keyWord = "splitStr";
    SubStr exp = getFollowingExpr(frame, keyWord);
    std::vector<SubStr > params = Object("","",exp.data).params;
    if(params.size() < 2)
    	throw std::string("splitStr (<object name>, <\"delimiter\">");
    SubStr vns = getVarName(frame->exp, frame->index+keyWord.size()+params[0].start);
    Object *obj = resolveNextExprSafe(frame, vns);
    //parse only text between quotes
    size_t strStart = 0;
    while(params[1].data[strStart] && params[1].data[strStart++] != '\"'){}
    size_t strEnd = params[1].data.length()-1;
    while(params[1].data[strEnd] && params[1].data[strEnd--] != '\"'){}

    string_view filter((char*) params[1].data.c_str()+strStart, strEnd-1);
    //at this point string start, and end point to the appropriate places in the parameter
    Object arr = obj->splitString(filter, memory);
    (*frame->getLocalMemory())[arr.id] = arr;
    if(*boolsettings["o"])
    {
    	std::cout<<"split string("<<arr.id<<") added to memory\n";
    }
    return MAX;
}
Object* resolveNextObjectExpression(AscalFrame<double>* frame, SubStr &varName, Object *obj = nullptr);
Object* resolveNextExprSafe(AscalFrame<double>* frame, SubStr &varName)
{
	Object* obj = resolveNextObjectExpression(frame, varName);
	if(!obj)
	{
		throw std::string("Error Resolving object expression near index: "+to_string(varName.start)+" last parsed var name: "+varName.data);
	}
	return obj;
}
//Desc of resolveNextObjectExpression algorithm
//if proceeding char is . then get object associated with varname before .
//increment index, and getVarName again one place after the period, and use lookup next varname
//in the previously gotten object's objectMap

//if next char is [ parse param between []
	//if numberic access object gotten above and retrieve value at the numeric index supplied else lookup in hashmap
	//if first char is & then try to lookup object using the operand's object map converted to a string
	//if first char is " act like it was connected with a .

//if following char is not . or [ then return currently pointed to object otherwise loop
Object* resolveNextObjectExpression(AscalFrame<double>* frame, SubStr &varName, Object *obj)
{
	//getvarname
	frame->index = varName.end+1;
	if(!obj)
	{
		obj = getObjectNoError(frame, varName.data);
	}
	if(frame->getContext() && (!obj &&
			(varName.data.size() == 4 && varName.data[0] == 't' && varName.data[1] == 'h' && varName.data[2] == 'i' && varName.data[3] == 's')))
		obj = frame->getContext()->getThis();
	bool parsing;
	do {
		parsing = frame->exp[frame->index] == '.';
		while (obj && frame->exp.size() > frame->index && parsing)
		{
			varName = getVarName(frame->exp, frame->index);
			parsing = frame->exp[varName.end+1] == '.';
			frame->index = varName.end+1 + parsing;
			obj = &(*obj)[varName.data];
		}
		if(obj && frame->exp.size() > frame->index && frame->exp[frame->index] == '[')
		{
			//if index in array lookup
			if((isNumeric(frame->exp[++frame->index]) || isalpha(frame->exp[frame->index])) && frame->exp[frame->index])
			{
				int index = frame->index-1;
				SubStr lStr = getExprInString(frame->exp, index, '[',']',';');
				varName.data = lStr.data;
				double lookup = callOnFrame(frame, lStr.data);
				obj = &(*obj)[lookup];
				frame->index = index+1;
				varName.end = index+lStr.data.size()-4;
			}
			//else dictionary based lookup by string
			else if(frame->exp.size() > frame->index)
			{
				//By variable name representing string to lookup
				if(frame->exp[frame->index] == '&')
				{
					varName = getVarName(frame->exp, ++frame->index);
					frame->index = varName.start;
					obj = &(*obj)[resolveNextObjectExpression(frame, varName)->listToString(memory)];
					varName.end++;
				}
				//By string literal
				else if(frame->exp[frame->index] == '\"')
				{
					varName = getVarName(frame->exp, ++frame->index);
					frame->index = varName.end+1;
					while(frame->index < frame->exp.size() && frame->exp[frame->index++] == ']'){}
					varName.end = frame->index ;
					obj = &(*obj)[varName.data];
				}
				else
				{
					throw std::string("Error invalid lookup parameters supplied to map: "+obj->id);
				}
			}
		}
		parsing = frame->exp[++frame->index] == '.' || frame->exp[frame->index] == '[';
	} while(obj && parsing);
	varName.start = varName.end - varName.data.size() + 1;
	return obj;
}
Object* resolveNextObjectExpressionPartial(AscalFrame<double>* frame, SubStr &varName, Object *obj)
{
	//getvarname
	frame->index = varName.end+1;
	if(!obj)
		obj = getObjectNoError(frame, varName.data);
	if((!obj && frame->getContext()) || (varName.data.size() == 4 && varName.data[0] == 't' && varName.data[1] == 'h' && varName.data[2] == 'i' && varName.data[3] == 's'))
		obj = frame->getContext()->getThis();
	bool parsing = frame->exp[frame->index] == '.';
	Object *possible = nullptr;
	do {
		parsing = frame->exp[frame->index] == '.';
		while (obj && frame->exp.size() > frame->index && parsing)
			{
				varName = getVarName(frame->exp, frame->index);
				parsing = frame->exp[varName.end+1] == '.';
				frame->index = varName.end+1 + parsing;
				if(parsing || frame->exp[varName.end+1] == '[')
					obj = &(*obj)[varName.data];
			}
			if(obj && frame->exp.size() > frame->index && frame->exp[frame->index] == '[')
			{
				//if index in array lookup
				if((isNumeric(frame->exp[++frame->index]) || isalpha(frame->exp[frame->index])) && frame->exp[frame->index])
				{
					int index = frame->index-1;
					varName = getExprInString(frame->exp, index, '[',']',';');
					varName.data = to_string(callOnFrame(frame, varName.data));
					frame->index = index+1;
				}
				//else dictionary based lookup by string
				else if(frame->exp.size() > frame->index)
				{
					//By variable name representing string to lookup
					if(frame->exp[frame->index] == '&')
					{
						varName = getVarName(frame->exp, ++frame->index);
						frame->index = varName.start;
						varName.data = resolveNextObjectExpression(frame, varName)->listToString(memory);
						frame->index = varName.end+1;
						while(frame->index < frame->exp.size() && frame->exp[frame->index++]){}
						possible = &obj->getMapUnsafe(varName.data);
					}
					//By string literal
					else if(frame->exp[frame->index] == '\"')
					{
						varName = getVarName(frame->exp, ++frame->index);
						frame->index = varName.end+1;
						while(frame->index < frame->exp.size() && frame->exp[frame->index++] == ']'){}
						varName.end = frame->index - 1;
						possible = &obj->getMapUnsafe(varName.data);
					}
					else
					{
						throw std::string("Error invalid lookup parameters supplied to map: "+obj->id);
					}
				}
			}
			if(frame->exp[++frame->index] == '.' || frame->exp[frame->index] == '[')
			{
				parsing = true;
				obj = possible;
				possible = nullptr;
			}
	} while(obj && parsing);

	return obj;
}
std::string lenAction(AscalFrame<double>* frame,bool saveLast)
{
	static const std::string keyWord = "arrLen";
    SubStr exp = getFollowingExpr(frame, keyWord);
    SubStr objname = getVarName(exp.data, 0);
    SubStr vns = getVarName(frame->exp, frame->index+keyWord.size());
    Object *obj = resolveNextExprSafe(frame, vns);
    frame->initialOperands.push(obj->getListSize());
    if(*boolsettings["o"])
    {
    	std::cout<<"arrLen(&"<<obj->id<<") = "<<obj->getListSize()<<'\n';
    }
    return 'a'+frame->exp.substr(exp.end,frame->exp.size());
}
std::string returnAction(AscalFrame<double>* frame,bool saveLast)
{
	static const std::string keyWord = "return";
    SubStr exp = getFollowingExpr(frame, keyWord);
    SubStr objname = getVarName(exp.data, 0);
    Object *obj = resolveNextExprSafe(frame, objname);
    if(frame->returnPointer)
    {
    	(*frame->returnPointer->getLocalMemory())["1return"] = *obj;
    }
    if(*boolsettings["o"])
    {
    	std::cout<<"return("<<obj->id<<") = "<<obj->toString()<<'\n';
    }
    return 'a'+frame->exp.substr(exp.end,frame->exp.size());
}
std::string sinAction(AscalFrame<double>* frame,bool saveLast)
{
	static const std::string keyWord = "sin";
    SubStr exp = getFollowingExpr(frame, keyWord);
    double input = callOnFrame(frame,exp.data);
    frame->initialOperands.push(sin(input));
    if(*boolsettings["o"])
    {
    	std::cout<<"sin("<<input<<") = "<<sin(input)<<'\n';
    }
    return 'a'+frame->exp.substr(exp.end,frame->exp.size());
}
std::string tryAction(AscalFrame<double>* frame,bool saveLast)
{
	static const std::string keyWord = "try";
    SubStr exp = getFollowingExpr(frame, keyWord);
    try{
    	double input = callOnFrame(frame,exp.data);
    	frame->initialOperands.push(input);
    }
    catch(std::string &s)
    {
    	frame->initialOperands.push(std::floor(std::numeric_limits<double>::max()));
    	callOnFrame(frame, "loc errorMessageLog = {"+s+"};\nloc hasErrorOccurred = 1");
    }
    if(*boolsettings["o"])
    {
    	std::cout<<"try("<<exp.data<<")\n";
    }
    return 'a'+frame->exp.substr(exp.end,frame->exp.size());
}
std::string flushOutAction(AscalFrame<double>* frame,bool saveLast)
{
    std::fflush(stdout);
    if(*boolsettings["o"])
    {
    	std::cout<<"Flushing std out"<<'\n';
    }
    return MAX;
}
std::string sleepAction(AscalFrame<double>* frame,bool saveLast)
{
	static const std::string keyWord = "sleep";
    SubStr exp = getFollowingExpr(frame, keyWord);
    double input = callOnFrame(frame, exp.data);

#if defined(WIN32) || defined(_WIN32)
    usleep(((int)input)*100));
#else
    std::this_thread::sleep_for(std::chrono::microseconds(((int)input)*100));
#endif
    if(*boolsettings["o"])
    {
    	std::cout<<"sleeping for "<<input<<" tenths of a milli-second\n";
    }
    return 'a'+frame->exp.substr(exp.end,frame->exp.size());
}
uint64_t lehmer64();
//can also use time(NULL) instead of clock()
static uint64_t hashRand = clock(), hr2 = 0;
long ascalPRNG()
{
	hashRand ^= lehmer64();
	hashRand ^= hashRand<<30;
	hashRand ^= hashRand>>32;
	hashRand ^= hashRand<<7;
	hashRand ^= hashRand>>45;
	hashRand &= ((uint64_t)1<<31)-1;
	hashRand *= ((hashRand&(1L<<16))!=0)*-1 + ((hashRand&(1L<<16))==0);
	return hashRand;
}
uint64_t lehmer64()
{
	hr2 += 0xe120fc15;
	uint64_t tmp;
	tmp = (uint64_t)hr2 * 0x4a39b70d;
	uint64_t m1 = (tmp >> 32) ^ tmp;
	tmp = (uint64_t) m1 * 0x12fad5c9;
	uint64_t m2 = (tmp >> 32) ^ tmp;
	return (m2^(m2>>32));

}
std::string srandomAction(AscalFrame<double>* frame,bool saveLast)
{
	static const std::string keyWord = "srand";
    SubStr exp = getFollowingExpr(frame, keyWord);
    double input = callOnFrame(frame,exp.data);
    double ib = input/100000;
    input *= 100000;
    input += ib;
    if(exp.data.length() > 5)
    {
    	hr2 = input;
    	hashRand = input;
    }
    else
    {
    	//can also use time(NULL) instead of clock()
    	input = hashRand + clock();
    	hr2 = input;
    	hashRand = input;
    }
    double hash = ascalPRNG();
	frame->initialOperands.push(hash);
    if(*boolsettings["o"])
    {
    	std::cout<<"srand("<<(input/100000)<<") = "<<to_string(hash)<<'\n';
    }
    return 'a'+frame->exp.substr(exp.end,frame->exp.size());
}
std::string randomAction(AscalFrame<double>* frame,bool saveLast)
{
	double hash = ascalPRNG();
	frame->initialOperands.push(hash);
    if(*boolsettings["o"])
    {
    	std::cout<<"rand = "<<to_string(hash)<<"\n";
    }
    return 'a'+frame->exp.substr(frame->index+4,frame->exp.size());
}
std::string pauseAction(AscalFrame<double>* frame,bool saveLast)
{
    static std::string s;
    std::cout<<"Paused press enter to continue.\n";

	std::streambuf* currentBuffer = std::cin.rdbuf();
    std::cin.rdbuf(stream_buffer_cin);
    std::getline(std::cin,s);
    std::cin.rdbuf(currentBuffer);
    return 'a'+frame->exp.substr(frame->index+5,frame->exp.size());
}
std::string cosAction(AscalFrame<double>* frame,bool saveLast)
{
	static const std::string keyWord = "cos";
    SubStr exp = getFollowingExpr(frame, keyWord);
    double input = callOnFrame(frame,exp.data);
    frame->initialOperands.push(cos(input));
    if(*boolsettings["o"])
    {
    	std::cout<<"cos("<<input<<") = "<<cos(input)<<'\n';
    }
    return 'a'+frame->exp.substr(exp.end,frame->exp.size());
}
std::string tanAction(AscalFrame<double>* frame,bool saveLast)
{
	static const std::string keyWord = "tan";
    SubStr exp = getFollowingExpr(frame, keyWord);
    double input = callOnFrame(frame,exp.data);
    frame->initialOperands.push(tan(input));
    if(*boolsettings["o"])
    {
    	std::cout<<"tan("<<input<<") = "<<tan(input)<<'\n';
    }
    return 'a'+frame->exp.substr(exp.end,frame->exp.size());
}
std::string arctanAction(AscalFrame<double>* frame,bool saveLast)
{
	static const std::string keyWord = "arctan";
    SubStr exp = getFollowingExpr(frame, keyWord);
    double input = callOnFrame(frame,exp.data);
    frame->initialOperands.push(atan(input));
    if(*boolsettings["o"])
    {
    	std::cout<<"arctan("<<input<<") = "<<atan(input)<<'\n';
    }
    return 'a'+frame->exp.substr(exp.end,frame->exp.size());
}
std::string arcsinAction(AscalFrame<double>* frame,bool saveLast)
{
	static const std::string keyWord = "arcsin";
    SubStr exp = getFollowingExpr(frame, keyWord);
    double input = callOnFrame(frame,exp.data);
    frame->initialOperands.push(asin(input));
    if(*boolsettings["o"])
    {
    	std::cout<<"arcsin("<<input<<") = "<<asin(input)<<'\n';
    }
    return 'a'+frame->exp.substr(exp.end,frame->exp.size());
}
double fibr(double x)
{
	if(x>0)
	{
		return fibr(x-1)+fibr(x-2);
	}
	else
	{
		return x;
	}
}
std::string fibrcppAction(AscalFrame<double>* frame,bool saveLast)
{
	static const std::string keyWord = "fibrcpp";
    SubStr exp = getFollowingExpr(frame, keyWord);
    double input = callOnFrame(frame,exp.data);
    Object paramsList("", "", exp.data);
    frame->initialOperands.push(fibr(input)*-1);
    if(*boolsettings["o"])
    {
    	std::cout<<"fibrcpp("<<input<<") = "<<fibr(input*-1)<<'\n';
    }
    return 'a'+frame->exp.substr(exp.end,frame->exp.size());
}
std::string arccosAction(AscalFrame<double>* frame,bool saveLast)
{
	static const std::string keyWord = "arccos";
    SubStr exp = getFollowingExpr(frame, keyWord);
    double input = callOnFrame(frame,exp.data);
    frame->initialOperands.push(acos(input));
    if(*boolsettings["o"])
    {
    	std::cout<<"arccos("<<input<<") = "<<acos(input)<<'\n';
    }
    return 'a'+frame->exp.substr(exp.end,frame->exp.size());
}
template <typename string_type>
bool isObj(string_type &s)
{
	uint32_t i = 0;
	while(s[i] && s[i++] == ' '){}
	bool obj = isalpha(s[i]);
	for(;obj && i < s.size(); i++)
	{
		obj = (isalpha(s[i]) || isNumeric(s[i]) || s[i] == '.' || s[i] == '[' || s[i] == ']' || s[i] == '"' || s[i] == ';');
	}
	return obj;
}
std::string setAction(AscalFrame<double>* frame,bool saveLast)
{

	static const std::string keyWord = "set";
    SubStr varName = getVarName(frame->exp,frame->exp.find(keyWord,frame->index)+keyWord.size());
    int startIndex = frame->exp.find("=",varName.end)+1;
    while(frame->exp[startIndex] && frame->exp[startIndex] == ' ')
        startIndex++;
    SubStr subexp = getExpr(frame->exp,startIndex);
    std::string value = to_string(callOnFrame(frame,subexp.data));
    Object *obj = resolveNextExprSafe(frame, varName);
    if(!isObj(subexp.data))
    {
        std::string value = to_string(callOnFrame(frame,subexp.data));
        *obj = Object(obj->id,value,"");
    }
    else
    {
    	SubStr rightHandObjectLookup = getVarName(frame->exp, startIndex);
    	Object *rightObj = resolveNextExprSafe(frame, rightHandObjectLookup);
    	*obj = *rightObj;
    }
    /*if(frame->getLocalMemory()->count(varName.data))
    {
        (*frame->getLocalMemory())[varName.data] = Object(varName.data,value,"");
        if(*boolsettings["o"])
        {
            std::cout<<"Local Var: "<<varName.data<<" set to: "<<value<<"\n\n";
        }
    }
    else if(frame->getParamMemory()->count(varName.data))
    {
        (*frame->getParamMemory())[varName.data] = Object(varName.data,value,"");
        if(*boolsettings["o"])
        {
            std::cout<<"Parameter: "<<varName.data<<" set to: "<<value<<"\n\n";
        }
    }
    else if(memory.count(varName.data))
    {
        memory[varName.data] = Object(varName.data,value,"");
        if(*boolsettings["o"])
        {
            std::cout<<"Global Var: "<<varName.data<<" set to: "<<value<<"\n\n";
        }
    }
    else
    {
        throw std::string("Error invalid variable name "+varName.data);
    }*/
    return MAX;
}
std::vector<std::string> parseExpList(const SubStr &codeBlock)
{
    std::vector<std::string> expressions;

    int i = 0;
    i = i + (codeBlock.data[0] == '{');

    while(i<codeBlock.data.size())
    {
        SubStr exp = getExpr(codeBlock.data,i);
        expressions.push_back(exp.data);
        //when exp.end == 0 then 1 else exp.end-1 end
        i += exp.end == 0?1:exp.end-1;

    }
    if(expressions.size() == 0)
    {
        expressions.push_back(codeBlock.data);
    }

    return expressions;
}
void execExpList( std::vector<std::string> &expressions,std::map<std::string,Object>& localMemory,
        AscalParameters &params,std::map<std::string,Object> &paramMemory)
{
    for(std::string &exp:expressions)
    {
        try{
            //std::cout<<"Execedin exec explist: "<<exp<<std::endl;
            allocated += sizeofFrame;
            FunctionFrame<double>* calledFunction = new FunctionFrame<double>(nullptr,nullptr,nullptr);
            calledFunction->exp = exp;
            calculateExpression<double>(calledFunction);
            }
            catch(std::string &exception)
            {
                throw std::string(exception + "\nIn exp: "+exp);
            }
    }
}
std::string elseAction(AscalFrame<double>* frame,bool saveLast)
{
    int index = frame->exp.find("else",frame->index) + 4;
    while(frame->exp[index] == ' ')
        index++;
    SubStr codeBlock = getCodeBlock(frame, index);
    if(frame->ifFlag() && !frame->ifResultFlag())
    {
        //if there is an if after the else this must be set
        if(frame->exp[index] == 'i' && frame->exp[index+1] == 'f')
        {
            if(*boolsettings["o"])
            {
                std::cout<<"Executing else case:\n";
            }
            frame->setComingfromElse(true);
        }
        else
        {
            frame->setComingfromElse(false);
            if(index + codeBlock.end < frame->exp.size())
                index += codeBlock.end;
            else
                index = frame->exp.size();

            if(*boolsettings["o"])
            {
                std::cout<<"Executing else case:\n";
            }
            try{
                //std::cout<<"Code Block in Else: "<<expr<<std::endl;
                //std::cout<<"Extracted Data: "<<codeBlock.data<<std::endl;
                callOnFrame(frame,codeBlock.data);
            }
            catch(std::string &exception)
            {
                throw std::string(exception + "\nIn else body subexp: ");
            }

        }
    }
    else
    {
        if(*boolsettings["o"])
        {
            std::cout<<"Skipping else case\n";
        }
        //std::cout<<"Index: "<<index<<" end of cb: "<<codeBlock.end-1 <<" len: "<< expr.size()<<std::endl;
        if(index + codeBlock.end - 1 < frame->exp.size())
            index += codeBlock.end-1;
        else
        	index = frame->exp.size();
    }
    frame->setIfFlag(false);
    frame->index = 0;
    return "a"+frame->exp.substr(index,frame->exp.size());
}
std::string ifAction(AscalFrame<double>* frame,bool saveLast)
{
    int index = frame->exp.find("if",frame->index)!=-1?frame->exp.find("if",frame->index)+2:0;
    while(frame->exp[index] == ' ')
    {
            index++;
    }

        int startOfBoolExp = index;
        int startOfCodeBlock = index;
        SubStr codeBlock("",0,0);

        while(frame->exp[startOfCodeBlock] && frame->exp[startOfCodeBlock] != '{')
        {
            startOfCodeBlock++;
        }
        index = startOfCodeBlock;
        const std::string booleanExpression = frame->exp.substr(startOfBoolExp,index-startOfBoolExp);
        if(booleanExpression.size() == 0)
        {
            throw std::string("Error no boolean expression provided in if.\n");
        }
        double boolExpValue;
        if(*boolsettings["o"])
        {
            std::cout<<"Executing boolean expression in if: "<<booleanExpression<<'\n';
        }
        try{
            boolExpValue = callOnFrame(frame,booleanExpression);
        }
        catch(std::string &exception)
        {
            throw std::string(exception + "\nIn if boolean exp: " + booleanExpression);
        }

        if(*boolsettings["o"])
        {
            std::cout<<"Boolean expression evaluation complete result: "<<(boolExpValue?"true":"false")<<'\n';
        }
        codeBlock = getCodeBlock(frame, startOfCodeBlock);

        index = startOfCodeBlock + codeBlock.end - 1;
        if(index > frame->exp.size())
            index = frame->exp.size();
        if(boolExpValue != 0)
        {
            if(!frame->comingfromElse() || !frame->ifResultFlag())
            {
                frame->setIfFlag(true);
                frame->setIfResultFlag(true);
            }
            else
            {
                frame->setIfFlag(false);
                frame->setIfResultFlag(true);
            }
            if(*boolsettings["o"])
            {
                std::cout<<"Executing code block in if: "<<codeBlock.data<<'\n';
            }
            try{
                callOnFrame(frame,codeBlock.data);
            }
            catch(std::string &exception)
            {
                throw std::string(exception + "\nIn if body subexp: "+codeBlock.data);
            }
            if(*boolsettings["o"])
            {
                std::cout<<"If block Execution Complete.\n";
            }

        }
        else
        {

            if(*boolsettings["o"])
            {
                std::cout<<"Skipping if code block\n";
            }
            if(!frame->comingfromElse() || !frame->ifResultFlag())
            {
                frame->setIfFlag(true);
                frame->setIfResultFlag(false);
            }
            else
            {
                frame->setIfFlag(false);
                frame->setIfResultFlag(false);
            }
        }
        frame->setComingfromElse(false);

        frame->index = 0;
        return "a"+frame->exp.substr(index-1,frame->exp.size());
}
template <class t>
t callOnFrame(AscalFrame<t>* callingFrame,std::string subExp)
{
    allocated += sizeofFrame;
    int preCallLineCount = lineCount;
    lineCount = 0;
    ParamFrame<t> executionFrame(callingFrame->getParams(),callingFrame->getParamMemory(),callingFrame->getLocalMemory());
    executionFrame.exp = subExp;
    executionFrame.setIsDynamicAllocation(false);
    executionFrame.setContext(callingFrame->getContext());
    t data;
    try{
    data = calculateExpression<t>(&executionFrame);
    }catch(std::string &error)
    {
        lineCount = preCallLineCount;
        deallocated += sizeofFrame;
        throw error;
    }
    lineCount = preCallLineCount;
    deallocated += sizeofFrame;
    return data;
}
template <class t>
t callOnFrameFormatted(AscalFrame<t>* callingFrame,std::string subExp)
{
    allocated += sizeofFrame;
    ParamFrame<t> executionFrame(callingFrame->getParams(),callingFrame->getParamMemory(),callingFrame->getLocalMemory());
    executionFrame.exp = subExp;
    executionFrame.setIsDynamicAllocation(false);
    t data = calcWithOptions(&executionFrame);
    deallocated += sizeofFrame;
    return data;
}
SubStr getCodeBlock(AscalFrame<double> *frame, int index)
{
    SubStr codeBlock = getExpr(frame->exp,index);
    if(codeBlock.data.size() == 0)
    {
        codeBlock.loadedNew = true;
    	std::string nextLine;
        while(std::cin && codeBlock.data.size() == 0)
        {
        	get_line(std::cin,nextLine);
        	codeBlock = getExpr(nextLine, 0);
        }
    }
    return codeBlock;
}
std::string whileAction(AscalFrame<double>* frame,bool saveLast)
{
    std::string expbkp = frame->exp;
    //std::cout<<"expression in while: "<<expr<<std::endl;
    int index = frame->exp.find("while",frame->index)<1000?frame->exp.find("while",frame->index)+5:0;
    while(frame->exp[index] == ' ')
        index++;

    int startOfBoolExp = index;
    int startOfCodeBlock = index;
    SubStr codeBlock("",0,0);

    while(frame->exp[startOfCodeBlock] && frame->exp[startOfCodeBlock] != '{')
    {
        startOfCodeBlock++;
    }

    index = startOfCodeBlock;

    //std::cout<<"while after finding end of exp: "<<expr.substr(index,expr.size())<<" start of Exp Index: "<<startOfBoolExp<<" end Of boolExp Index: "<<index<<std::endl;

    const std::string booleanExpression = frame->exp.substr(startOfBoolExp,index-startOfBoolExp);
    if(booleanExpression.size() == 0)
    {
        throw std::string("Error no boolean expression provided in while.\n");
    }
    double boolExpValue;
    try{
        if(*boolsettings["o"])
        {
            std::cout<<"Executing While Boolean Expression: "<<booleanExpression<<"\n";
        }
        boolExpValue = callOnFrame(frame,booleanExpression);
    }
    catch(std::string &exception)
    {
        throw std::string(exception +"\nIn While Boolean Expression");
    }
    if(*boolsettings["o"])
    {
        std::cout<<"Execution Complete. "<<(boolExpValue?"true":"false")<<"\n\n";
    }
    int preBodyLineCount = lineCount;
    codeBlock = getCodeBlock(frame, index);
    int postBodyLineCount = lineCount;
    lineCount = preBodyLineCount;
    if(boolExpValue != 0)
    {
        while(boolExpValue != 0)
        {

            if(*boolsettings["o"])
            {
                std::cout<<"Executing While loop code block:\n"<<codeBlock.data<<'\n';
            }
            try{
                callOnFrame(frame,codeBlock.data);
            }
            catch(std::string &exception)
            {
                throw std::string(exception + "\nIn While body subexp: ");
            }
            if(*boolsettings["o"])
            {
                std::cout<<"While block Execution Complete.\n\n";
                std::cout<<"Jumping back to execute While Boolean Expression: "<<booleanExpression<<"\n";
            }
        try{
            boolExpValue = callOnFrame(frame,booleanExpression);
            if(*boolsettings["o"])
            {
                if(boolExpValue)
                {
                    std::cout<<"Execution result true.\nContinuing loop execution.\n\n";
                }
                else
                {
                    std::cout<<"Execution result false.\nExiting loop.\n\n";
                }
            }
        }
        catch(std::string &exception)
        {
            throw std::string(exception  + "\nIn While Boolean Expression");
        }
        }
    }
    lineCount = postBodyLineCount;
    index = codeBlock.end + startOfCodeBlock-2;
    frame->index = 0;
    while(frame->exp[index] == ';' || frame->exp[index] == ' ' || frame->exp[index] == '}')
        index++;
    return "a"+expbkp.substr((index-2<frame->exp.size()?index-2:frame->exp.size()),frame->exp.size());
}
bool firstChar(std::string &s, char c)
{
	static uint32_t it;
	for(it = 0; s[it] == ' '; it++){}

	return s[it] == c;
}
std::string forRangeAction(AscalFrame<double>* frame,bool saveLast)
{
    std::string expbkp = frame->exp;
    int index = frame->index+4;
    while(frame->exp[index] == ' ')
        index++;

    SubStr itVar = getVarName(frame->exp, index);
    const int postRangeIndex = frame->exp.find("in range", index)+8;
    SubStr limitExpr = getExprInString(frame->exp, postRangeIndex, '(', ')', '{');
    Object limitParams("","","");
    limitParams.setParams(limitExpr.data);
    if(limitParams.params.empty())
    	throw std::string("Error no limit in for loop condition");
    callOnFrame(frame, "loc "+itVar.data+" = 0");
    SubStr limitStr = limitParams.params.size()>1?limitParams.params[1]:limitParams.params[0];
    int startOfCodeBlock = limitExpr.start;
    SubStr codeBlock("",0,0);

    while(frame->exp[startOfCodeBlock] && frame->exp[startOfCodeBlock] != '{')
    {
        startOfCodeBlock++;
    }
    index = startOfCodeBlock;

    int preBodyLineCount = lineCount;
    codeBlock = getCodeBlock(frame, index);
    int postBodyLineCount = lineCount;
    lineCount = preBodyLineCount;

	double i = limitParams.params.size()>1?callOnFrame(frame,limitParams.params[0].data):0;
    if(firstChar(limitStr.data,'&'))
    {
    	SubStr limitPartial = getVarName(frame->exp, postRangeIndex+limitStr.start);
    	limitStr.start = limitStr.data.find("&")+1;
    	size_t limitStrEnd = limitStr.data.find(".")-1;


    	Object *list = resolveNextExprSafe(frame, limitPartial);
    	if(list->getListSize() > 0)
    	{

        	if(limitParams.params.size()<3 || callOnFrame(frame,limitParams.params[2].data) > 0)
        	{
            	for(;i < list->getListSize(); i += limitParams.params.size()>2?callOnFrame(frame,limitParams.params[2].data):1)
            	{
                    if(*boolsettings["o"])
                    {
                        std::cout<<"Executing for loop code block:\n"<<codeBlock.data<<'\n';
                    }
                    try{
                        (*frame->getLocalMemory())[itVar.data] = list->getListElement(i, memory);
                        callOnFrame(frame,codeBlock.data);
                    }
                    catch(std::string &exception)
                    {
                        throw std::string(exception + "\nIn for body subexp: ");
                    }
                    if(*boolsettings["o"])
                    {
                        std::cout<<"for block Execution Complete.\n\n";
                        std::cout<<"Jumping back to execute for Boolean Expression: "<<i+1<<"<"<<list->getListSize()<<"\n";
                    }
            	}
        	}
        	else
        	{
        		if(i == 0)
        			i = list->getListSize()-1;
            	for(;i >= 0; i += callOnFrame(frame,limitParams.params[2].data))
            	{
                    if(*boolsettings["o"])
                    {
                        std::cout<<"Executing for loop code block:\n"<<codeBlock.data<<'\n';
                    }
                    try{
                        (*frame->getLocalMemory())[itVar.data] = list->getListElement(i, memory);
                        callOnFrame(frame,codeBlock.data);
                    }
                    catch(std::string &exception)
                    {
                        throw std::string(exception + "\nIn for body subexp: ");
                    }
                    if(*boolsettings["o"])
                    {
                        std::cout<<"for block Execution Complete.\n\n";
                        std::cout<<"Jumping back to execute for Boolean Expression: "<<i+1<<"<"<<list->getListSize()<<"\n";
                    }
            	}
        	}
    	}
    }
    else
    {
        double limit = callOnFrame(frame, limitStr.data);
    	    if(i < limit)
    	        for(; i < limit;
    	        		i += limitParams.params.size()>2?callOnFrame(frame,limitParams.params[2].data):1)
    	        {

    	            if(*boolsettings["o"])
    	            {
    	                std::cout<<"Executing for loop code block:\n"<<codeBlock.data<<'\n';
    	            }
    	            try{
    	                (*frame->getLocalMemory())[itVar.data] = Object(itVar.data,to_string(i),"");
    	                callOnFrame(frame,codeBlock.data);
    	            }
    	            catch(std::string &exception)
    	            {
    	                throw std::string(exception + "\nIn for body subexp: ");
    	            }
    	            if(*boolsettings["o"])
    	            {
    	                std::cout<<"for block Execution Complete.\n\n";
    	                std::cout<<"Jumping back to execute for Boolean Expression: "<<i+1<<"<"<<limit<<"\n";
    	            }
    	        }
    	    else
    	        for(; i > limit;
    	        		i += limitParams.params.size()>2?callOnFrame(frame,limitParams.params[2].data):-1)
    	        {

    	            if(*boolsettings["o"])
    	            {
    	                std::cout<<"Executing for loop code block:\n"<<codeBlock.data<<'\n';
    	            }
    	            try{
    	                (*frame->getLocalMemory())[itVar.data] = Object(itVar.data,to_string(i),"");
    	                callOnFrame(frame,codeBlock.data);
    	            }
    	            catch(std::string &exception)
    	            {
    	                throw std::string(exception + "\nIn for body subexp: ");
    	            }
    	            if(*boolsettings["o"])
    	            {
    	                std::cout<<"for block Execution Complete.\n\n";
    	                std::cout<<"Jumping back to execute for Boolean Expression: "<<i+1<<"<"<<limit<<"\n";
    	            }
    	        }

    }

     callOnFrame(frame, "delete "+itVar.data);


    lineCount = postBodyLineCount;
    index = codeBlock.end + startOfCodeBlock-2;
    frame->index = 0;
    while(frame->exp[index] == ';' || frame->exp[index] == ' ' || frame->exp[index] == '}')
        index++;
    return "a"+expbkp.substr((index-2<frame->exp.size()?index-2:frame->exp.size()),frame->exp.size());
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
    return s.substr(0,s.size()-secondDelimiter.size());
}
std::string printMemory(std::map<std::string,Object*> &memory,std::string delimiter,bool justKey = true,
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
std::string printMemory(std::unordered_map<std::string,Object> &memory,std::string delimiter,bool justKey = true,
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
std::string whenAction(AscalFrame<double>* frame,bool saveLast)
{
    //parse expression, start from substr when go up until substr end is found
    //extract substring, and save the other parts of the expression
    //find when evaluate expression between it and then
    //if the expression evaluates to anything other than 0
    //then extract the expression proceeding the then statement
    //frame->index = 0;
	static const int invalidIndex = 2000000000;
    const int startIndex = frame->index;

    int endIndex = frame->index;
    {
    int i = endIndex;
    uint8_t qCount = 0;
    while(i < frame->exp.length()-2)
    {
    	qCount = ((frame->exp[i] == '\"') + qCount) & 1;
    	if(frame->exp[i] == 'e' && frame->exp[i+1] == 'n' && frame->exp[i+2] == 'd' && qCount == 0)
    	{
    		endIndex = i;
    		i = invalidIndex;
    	}
    	i++;
    }
    }
    //std::string startOfExp = expr.substr(0,startIndex);
    std::string endOfExp = frame->exp.substr(endIndex+3,frame->exp.length());
    std::string value;
    int index = startIndex + 4;
    std::stringstream exp;
    //should always start after when is finished to build boolean expression

    int thenIndex;
    int whenIndex = startIndex;
    double boolExpValue;
    int elseIndex = frame->exp.find("else",frame->index);
    elseIndex = elseIndex==-1?invalidIndex:elseIndex;
    int lastThen = 0;
    do {
        thenIndex = frame->exp.find("then",index);
        //Parsing boolean expression
        while(frame->exp[index] == ' ' && index < endIndex && index < thenIndex)
            index++;
        while(frame->exp[index] && index < endIndex && index < thenIndex)
        {
            exp<<(frame->exp[index]);
            index++;
        }
        std::string booleanExpression = exp.str();
        if(booleanExpression.size() == 0)
        {
            throw std::string("Error no boolean expression provided in when.\n");
        }
        exp.str("");

        if(*boolsettings["o"])
        {
            std::cout<<"Executing Boolean Expression: "<<booleanExpression<<'\n';
        }
        try{
            boolExpValue = callOnFrame(frame,booleanExpression);
        }
        catch(std::string &exception)
        {
            throw std::string("In When Action Calculating BooleanExpression: "+booleanExpression+"\n"+exception);
        }
    //false case simply set the index to the next instance of when+4
    //and repeat until true, or at end of case when
        if(boolExpValue == 0)
        {
            index = frame->exp.find("when",index) + 5;
            whenIndex = index - 5;
            if(whenIndex == -1 && elseIndex != invalidIndex)
            {
                value = getExpr(frame->exp.substr(elseIndex+4,endIndex-(elseIndex+4)),0).data;
                index = endIndex;
            }

        }
    //true case get sub expression associated with this when return value exit loop
        else
        {
            index += 5;
            thenIndex = frame->exp.find("when",index);
            thenIndex = thenIndex==-1?endIndex+1:thenIndex;
            value = getExpr(frame->exp.substr(index,std::min(std::min(endIndex,thenIndex),elseIndex)-index),0).data;
        }
        lastThen = thenIndex;
    } while(frame->exp[index] && boolExpValue == 0 && index < endIndex);
    if(*boolsettings["o"])
    {
        std::cout<<"Executing Branch: "<<value<<" Params: "<<printMemory(*frame->getParamMemory()," = ",false,"|");
        std::cout<<"\n";
    }
    frame->index = 0;
    return "a"+value.substr(0,value.length()-1)+endOfExp;
}
std::string printCalculation(AscalFrame<double>* frame,bool saveLast)
{
    //the 6 added here could be 5 as print is only 5 char long,
    //but I also want to accomodate printc sending unaltered statements to this function,
    //and it's not like the expression itself can start at 5+ the index of print,
    //that must begin at 6+ the index of the p in print at least
    std::string exp = getExpr(frame->exp.substr(frame->exp.find("print",frame->index)+6,frame->exp.length()),0).data;
    bool print = *boolsettings["p"];
    *boolsettings["p"] = true;
    callOnFrameFormatted(frame, exp);
    *boolsettings["p"] = print;
    return MAX;
}
void printVar(AscalFrame<double>* frame,bool saveLast)
{
	std::string data;
	static const std::string keyWord = "print var";
    SubStr exp = getFollowingExpr(frame, keyWord);
	SubStr varName = getVarName(frame->exp, frame->index+keyWord.size());
    std::cout<<"print var "<<exp.data.substr(0,exp.data.find(';'))<<" = "<<resolveNextExprSafe(frame, varName)->toString()<<"\n";
}

std::string printDefaultAction(AscalFrame<double>* frame,bool saveLast)
{
    int endOfStatement = frame->index;
    while(frame->exp[endOfStatement] && (frame->exp[endOfStatement] != ';' && frame->exp[endOfStatement] != '\n' &&
                                         frame->exp[endOfStatement] != '\0'))
    {
        endOfStatement++;
    }
    uint32_t quoteIndex = frame->exp.find("\"",frame->index);
    if(quoteIndex != -1 && quoteIndex < endOfStatement)
    {
        printStringAction(frame,false);
    }
    else
    {
        printCommand(frame,false);
    }
    return MAX;
}

std::string printCommand(AscalFrame<double>* frame,bool saveLast)
{
    std::cout<<"\n";
    std::string value  = MAX;
    int start = frame->exp.find("print",frame->index)+6;
            if(cmpstr(frame->exp.substr(start,3),"all"))
            {
                printAllFunctions();
            }
            else if(cmpstr(frame->exp.substr(start,3),"udf"))
            {
                printAllUDF(frame);
            }
            else if(cmpstr(frame->exp.substr(start,3),"sdf"))
            {
                printAllSDF();
            }
            else if(cmpstr(frame->exp.substr(start,3),"var"))
            {
                printVar(frame,saveLast);
            }
            else
            {
                printCalculation(frame,saveLast);
            }
            return value;
}
SubStr getVarNameOrNum(const std::string &s,int index)
{
    int begin = index;
    int count = 0;
    while(s[index] && !isalpha(s[index]) && !isNumeric(s[index]))
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

void updateBoolSetting(AscalFrame<double>* frame)
{

    lastExp.pop();
    SubStr id = getVarName(frame->exp,frame->index);
    int endOfId = id.start+id.data.length();
    while(frame->exp[endOfId] == ' ')
    	endOfId++;
    SubStr expression = getVarNameOrNum(frame->exp,endOfId);
    short value = expression.data.length()>0?callOnFrame(frame, expression.data):-1;
    setting<bool> set = boolsettings[id.data];
    //inverts setting value via operator overloads of = and *
    bool data = value<0?!*set:value;
    setting<bool> newSetting(set.getName(),set.getCommand(),data);
    boolsettings[set.getCommand()] = newSetting;
    if(*boolsettings["o"])
    {
        std::cout<<set.getName()<<" Status: "<<data<<"\n";
    }
}
//returns a string of all the data it has read in a single string delimited by ;
//and a double which is the result of the last calc

//create function that loops through istream supplied as parameter in as we see {
//if no frames in stack open new stack frame with new local memory
//keep executing statements using the same local memory until } is seen
//if size of stack is greater than one simply pop one off the stack to be used
//
double interpretParam(AscalFrame<double>* frame,bool saveLast)
{
    double value = 0;
    SubStr firstWord = getVarName(frame->exp,frame->index);

        if(saveLast && !cmpstr(firstWord.data,"u") && !cmpstr(firstWord.data,"r"))
            lastExp.push(frame->exp);

        value = calcWithOptions(frame);

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
//now with more stacks!
  //these stacks handle respecting priority, the can handle as many priority levels as you like
stack<double> savedOperands;
stack<char> savedOperators;
stack<double> processOperands;
stack<char> processOperators;
std::unordered_map<uint64_t,double> memoPad;
template <typename t>
void cleanOnError(bool timeInstruction, t start, t end)
{
	if(timeInstruction)
	    	        {
	    	            end = std::chrono::system_clock::now();

	    	            std::chrono::duration<double> elapsed_seconds = end-start;
	    	            std::time_t end_time = std::chrono::system_clock::to_time_t(end);

	    	            std::cout <<
	    	                      "Stack frames used: "<<frameCount<<'\n'
	    	                        <<"Variables accessed: "<<varCount<<'\n'
	    							<<"Values used from memo tables: "<<rememberedFromMemoTableCount<<'\n'
	    	            <<"Currently active allocations: "<<allocated-deallocated<<" Total Allocated: "<<allocated<<" Total freed: "<<deallocated<<'\n'
	    				<< "finished computation at " << std::ctime(&end_time)
	    				                      << "elapsed time: " << elapsed_seconds.count() << "s\n";

	    	            //lastRunAllocatedTotal = allocated;
	    	        }

	    	    frameCount = 1;
	    	    varCount = 0;
	    	    rememberedFromMemoTableCount = 0;
	    	    savedOperands.clear();
	    	    savedOperands.clear();
	    	    processOperands.clear();
	    	    processOperators.clear();
	    	    memoPad.clear();
}
double calcWithOptions(AscalFrame<double>* frame)
{
    bool timeInstruction = *boolsettings["t"];


    std::cout<<"";
    std::chrono::system_clock::time_point start,end;
    if(timeInstruction){
    start = std::chrono::system_clock::now();
    }

    //alternate beginning of calculation for doubles
    //-------------------------
    double result;
    try{
    	result = calculateExpression<double>(frame);
    }catch(std::string &error)
    {
    	cleanOnError(timeInstruction, start, end);
    	throw error;
    }catch(int exitCode){
    	cleanOnError(timeInstruction, start, end);
    	throw exitCode;
    }
    memoPad.clear();
    //------------------------
    //std::cout<<std::to_string(result).length()<<"  max len "<<MAX.length()<<std::endl;
    //if()//returned by function that doesn't return a result
    {
        if(timeInstruction)
        {
            end = std::chrono::system_clock::now();

            std::chrono::duration<double> elapsed_seconds = end-start;
            std::time_t end_time = std::chrono::system_clock::to_time_t(end);

            std::cout <<
                      "Stack frames used: "<<frameCount<<'\n'
                        <<"Variables accessed: "<<varCount<<'\n'
						<<"Values used from memo tables: "<<rememberedFromMemoTableCount<<'\n'
            <<"Currently active allocations: "<<allocated-deallocated<<" Total Allocated: "<<allocated<<" Total freed: "<<deallocated<<'\n'
			<< "finished computation at " << std::ctime(&end_time)
			                      << "elapsed time: " << elapsed_seconds.count() << "s\n";

            //lastRunAllocatedTotal = allocated;
        }
        //std::cout<<"\nresult: "<<to_string(result)<<"\nMax: "<<Max<<std::endl>;
        if(std::to_string(result).length() != MAX.length() && *boolsettings["p"])
        {
        	if(*boolsettings["sci"])
        		std::cout<<result<<std::endl;
        	else
        		std::cout<<to_string(result)<<std::endl;
        }
    }
    frameCount = 1;
    varCount = 0;
    rememberedFromMemoTableCount = 0;
    savedOperands.clear();
    savedOperands.clear();
    processOperands.clear();
    processOperators.clear();
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
std::vector<std::string> getExprVec(const std::string &data,int index,char opening,char closing,char lineBreak)
{
    int count = 0;
    int openingCount = 0;
    int maxOpeningCount = 0;
    std::vector<std::string> result;
    if(data.length()<256)
        result.reserve(256);
    else
        result.reserve(data.length());
    std::string line = data;
    std::string lineBreakStr(" ");
    lineBreakStr[0] = (lineBreak);
    do{
        while(line[index] && !isNumeric(line[index]) && !isalpha(line[index]) && !isOperator(line[index]) &&
                line[index] != closing && line[index] != opening)
        {
            openingCount = (openingCount + (line[index] == opening) - (line[index] == closing));
            maxOpeningCount = (maxOpeningCount + (line[index] == opening));
            index++;
        }
        while(line[index + count] && (line[index + count] != lineBreak && line[index + count] != '\n')
                && !(line[index+count] == closing && openingCount == 1))
        {
            openingCount = (openingCount + (line[index+count] == opening) - (line[index+count] == closing));
            maxOpeningCount = (maxOpeningCount + (line[index+count] == opening));
            count++;
        }
        openingCount -= (line[index+count] == closing);

        if(count > 0)
        {

            /*for(int i = index<line.size()?index:line.size();line[i] && i <= index+count;i++)
            {
                result.push_back(line[i]);
            }*/
            result.push_back(line.substr(index,count));
            if(line[line.size()-1]!=opening && result[result.size()-1][result[result.size()-1].size()-1] != lineBreak)
                result.push_back(lineBreakStr);


        }
        if(openingCount > 0 && line.length() <= index+count)
        {
            get_line(std::cin, line);
            index = 0;
        }
        else
        {
            index = index+count+1;
        }
        count = 0;
    }while(openingCount > 0 && std::cin);

    if(maxOpeningCount > 0){
        std::string d(" ");
        d[0] = closing;
        result.push_back(d);
    }
    result.push_back(0);

    return result;
}
SubStr getExpr(const std::string &data,int index,char opening,char closing,char lineBreak)
{
    int count = 0;
    int openingCount = 0;
    int maxOpeningCount = 0;
    SubStr block("",0,0);
    std::vector<char> result;
    if(data.length()<256)
        result.reserve(256);
    else
        result.reserve(data.length());
    std::string line = data;

    do{
        while(line[index] && !isNumeric(line[index]) && !isalpha(line[index]) && !isOperator(line[index]) &&
                line[index] != closing && line[index] != opening)
        {
            openingCount += (line[index] == opening) - (line[index] == closing);
            maxOpeningCount += (line[index] == opening);
            index++;
        }
        while(line[index + count] && (line[index + count] != lineBreak && line[index + count] != '\n')
                && !(line[index+count] == closing && openingCount == 1))
        {
            openingCount += (line[index+count] == opening) - (line[index+count] == closing);
            maxOpeningCount += (line[index+count] == opening);
            count++;
        }
        openingCount -= (line[index+count] == closing);

        if(count > 0)
        {

            for(int i = index<line.size()?index:line.size();line[i] && i <= index+count;i++)
            {
                result.push_back(line[i]);
            }
            if(line[line.size()-1]!=opening && result[result.size()-1] != lineBreak)
                result.push_back(lineBreak);
        }
        if(openingCount > 0 && line.length() <= index+count)
        {
            block.loadedNew = true;
        	uint32_t i;
        	do{
        		lineCount++;
        		get_line(std::cin, line);
        		i = 0;
        		while(line[i] == ' ')
        			i++;
        	}while(line[i] == '#');


            index = 0;
        }
        else
        {
            index += count+1;
        }
        count = 0;
    }while(openingCount > 0 && std::cin);

    if(maxOpeningCount > 0)
        result.push_back(closing);
    block.data = std::string(result.begin(),result.end());
    block.start = index;
	block. end = result.size();
    return block;
}
SubStr getExprInString(const std::string &data,int index,char opening,char closing,char lineBreak)
{
    int count = 0;
    int openingCount = 0;
    int maxOpeningCount = 0;
    std::vector<char> result;
    if(data.length()<256)
        result.reserve(256);
    else
        result.reserve(data.length());
    std::string line = data;

    do{
        while(line[index] && !isNumeric(line[index]) && !isalpha(line[index]) && !isOperator(line[index]) &&
                line[index] != closing && line[index] != opening)
        {
            openingCount = (openingCount + (line[index] == opening) - (line[index] == closing));
            maxOpeningCount = (maxOpeningCount + (line[index] == opening));
            index++;
        }
        while(line[index + count] && (line[index + count] != lineBreak && line[index + count] != '\n')
                && !(line[index+count] == closing && openingCount == 1))
        {
            openingCount = (openingCount + (line[index+count] == opening) - (line[index+count] == closing));
            maxOpeningCount = (maxOpeningCount + (line[index+count] == opening));
            count++;
        }
        openingCount -= (line[index+count] == closing);

        if(count > 0)
        {

            for(int i = index<line.size()?index:line.size();line[i] && i <= index+count;i++)
            {
                result.push_back(line[i]);
            }
            if(line[line.size()-1]!=opening && result[result.size()-1] != lineBreak)
                result.push_back(lineBreak);
        }
        if(openingCount > 0 && line.length() <= index+count)
        {
        	std::stringstream s;
			s<<"Error, no closing ";
			s<<closing;
        	throw s.str();
        }
        else
        {
            index = index+count+1;
        }
        count = 0;
    }while(openingCount > 0 && std::cin);

    if(maxOpeningCount > 0)
        result.push_back(closing);
    result.push_back(0);

    return SubStr(std::string(result.begin(),result.end()),index,result.size());
}

SubStr getNewVarName(const std::string &data)
{
    int index = data.find("let")<5000 || data.find("const")<5000?data.find("t ")+1:0;
    return getVarName(data,index);
}

SubStr getVarName(const std::string &s,int index)
{
    int begin = index;
    while(s[index] && !isalpha(s[index]))
    {
        begin++;
        index++;
    }
    while(s.length()>index && (isalpha(s[index]) || isNumeric(s[index])))
    {
        index++;
    }
    std::string result = s.substr(begin>s.length()?s.length():begin,index - begin);
    index--;
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
template <class t>
void printStack(stack<t> &operands)
{
    t and1;
    std::cout << "Stack: "<<std::endl;
    while(!operands.isEmpty())
    {
        operands.top(and1);
        operands.pop();
        std::cout << " "<<and1<<", ";
    }
    std::cout<<std::endl;
}
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

template <typename t>
void createFrame(linkedStack<AscalFrame<t>* > &executionStack, AscalFrame<t>* currentFrame, Object *obj, int i,uint64_t hash);

static const std::hash<std::string> hashfn;
uint64_t hashFunctionCall(std::string &exp)
{
    return hashfn(exp);
}
static uint64_t h;
uint64_t hashFunctionCall(uint64_t hash,AscalParameters& params)
{
	h = hash;
    for(Object *s:params)
    {
        hash += hashfn(s->getInstructions());
        hash ^= hash>>2;
        hash ^= hash<<5;
        hash ^= hash>>12;
        hash ^= hash<<18;
        hash ^= hash>>21;
        hash ^= hash<<26;
        hash ^= hash<<29;
    }
    return hash+h;
}
uint64_t hash(AscalFrame<double>* currentFrame)
{
	uint64_t frameptr = (uint64_t) currentFrame;
	uint64_t rtnptr =   (uint64_t) currentFrame->returnPointer;
	uint64_t paramsptr = (uint64_t) currentFrame->getParamMemory();
	uint64_t locvarptr = (uint64_t) currentFrame->getLocalMemory();
	uint64_t hash = ((frameptr<<(1 + (15&rtnptr)))^currentFrame->memoPointer^paramsptr^locvarptr);
	hash ^= paramsptr>>16;
	hash ^= paramsptr<<16;
	hash ^= rtnptr>>6;
	hash ^= rtnptr<<26;
	hash ^= currentFrame->memoPointer>>3;
	hash ^= currentFrame->memoPointer<<20;
	hash ^= hash>>5;
	hash ^= hash<<10;
	hash ^= hash>>15;
	hash ^= hash<<20;
	hash ^= hash<<28;
	hash += frameptr<<8;
	hash += currentFrame->memoPointer;
	return hash;
}
void clearStackOnError(bool printStack, std::string &error, linkedStack<AscalFrame<double>* > &executionStack, AscalFrame<double>* currentFrame, AscalFrame<double>* frame, AscalFrame<double>* rtnFrame);

template <class t>
t calculateExpression(AscalFrame<double>* frame)
{
    int statementCount = 1;
    //variable that is always a copy
    char peeker;
    //variables to hold the operands
    t and1,and2;
    //c is a variable to store each of the characters in the expression
    //in the for loop
    char currentChar;

    t data = 0;
    AscalFrame<double>* rtnFrame = frame->returnPointer;
    frame->returnPointer = nullptr;
    //This loop handles parsing the numbers, and adding the data from the expression
    //to the stacks
    linkedStack<AscalFrame<t>* > executionStack;
    currentStack = &executionStack;
    AscalFrame<t>* currentFrame = frame;
    executionStack.push(currentFrame);
    try{
    while(!executionStack.isEmpty())
    {
        new_frame_execution:
        executionStack.top(currentFrame);
        if(currentFrame->isFunction() && currentFrame->isFirstRun())
        {
            currentFrame->setIsFirstRun(false);
        	if(*boolsettings["memoize"])
        	{
        		currentFrame->memoPointer = hashFunctionCall(currentFrame->memoPointer,*(currentFrame->getParams()));
        		if(memoPad.count(currentFrame->memoPointer))
        		{
        			data = memoPad[currentFrame->memoPointer];
        			currentFrame->returnResult(data, memory);
        			currentFrame->returnPointer = nullptr;
        			currentFrame->index = currentFrame->exp.length();
        			++rememberedFromMemoTableCount;

        			if(*boolsettings["o"])
        			{
        				if(std::to_string(data).length() != MAX.length())
        				{
        					std::cout<<"retrieving return value from memo tables\n";
        					std::cout<<"Returning value: "<<data<<'\n';
        				}
        			}
        		}
        	}

        }
        //If complex object return register is set on frame
        //we should internally have a pointer to an object that exists on the heap
        //so at the end of the process this needs to be cleaned
        //functioncall(&object.functioncall()) to handle this we need to be aware we are in a pfp frame
        //we need to alter the return value previously specified in pfp frame inside of a return keyword,
        //and set it equal to the obj returned which should be copied to the heap just in case it is
        //locally scoped in the function call
        //should be resolved in the return with the normal resolveNextObjectSafe return keyword must set rtn flag in parent frme

        //object.functioncall().fieldFromObjectReturnedByFnCall
        //object.functioncall().fieldThatIsAnFnFromObjectReturnedByFnCall()
        //object.functioncall[lookup]().fieldFromObjectReturnedByFnCall
        //object.functioncall[lookup]().fieldThatIsAnFnFromObjectReturnedByFnCall()
        //remainder after initial will be
        //.fieldFromObjectReturnedByFnCall or .fieldThatIsAnFnFromObjectReturnedByFnCall()
        for(int i = currentFrame->index;i <= currentFrame->exp.length();i++)
        {
         currentFrame->index = i;
         currentChar = currentFrame->exp[i];
         currentFrame->initialOperators.top(peeker);
         //anything used outside of the first { gets dropped from program stack, for param definition
         currentFrame->level += (currentChar == '{') - (currentChar == '}');


         if(currentChar == '{' && currentFrame->level == 1)
         {
            while(!currentFrame->initialOperands.isEmpty())
                currentFrame->initialOperands.pop();
            while(!currentFrame->initialOperators.isEmpty())
                currentFrame->initialOperators.pop();
         }
         else if(isalpha(currentChar) && (isalpha(frame->exp[i+1]) || !isOperator(currentChar)))
         {
        	 //This needs to be updated, and simplified it makes conditional jumps very expensive
             SubStr varName(getVarName(currentFrame->exp,i));

             //Keyword handling only one keyword at the beginning of each statement allowed,
             //including statements defined in variables

             if(inputMapper.count(varName.data) != 0)
             {

                 currentFrame->setAugmented(true);
                 bool isDynamicBackup = currentFrame->isDynamicAllocation();
                 std::string result;
                 try{
                 result = inputMapper[varName.data](currentFrame,false);
                 currentFrame->setIsDynamicAllocation(isDynamicBackup);
                 }catch(std::string& s){
                     currentFrame->setIsDynamicAllocation(isDynamicBackup);
                     throw s;
                 }

                 uint8_t subLevel = 0;
                 while((subLevel != 0 && currentFrame->exp[i]) || (currentFrame->exp[i] && currentFrame->exp[i] != ';' && currentFrame->exp[i] != '\n'))
                 {
                     i++;
                     subLevel += (currentFrame->exp[i]=='{') - (currentFrame->exp[i] == '}');

                 }
                 statementCount++;
                 if((currentFrame->exp[i] == 0 || currentFrame->exp[i+1] == 0) && result.length() == MAX.length())
                 {
                     currentFrame->exp = MAX;
                     if(currentFrame->initialOperands.size() != 0)
                         throw std::string("\nError called keyword: '"+varName.data+"'\nwithout statement separator ';' before previous expression.");
                 }
                 else if(isalpha(result[0]))
                 {
                     //for returning altered versions of the expression up till a statement separator like ;
                     //use cases: getting user input outside functions and variables
                     //when then when then end if else.
                     //while loops?
                     //just make sure first character in returned string is a throwaway alphabetical character
                     //it will not be parsed so ensure it is not meaningful code
                     currentFrame->exp = result.substr(1,currentFrame->exp.length());

                     i = -1;
                     currentFrame->index = 0;
                     continue;
                 }
                 else
                 {
                     currentFrame->exp = currentFrame->exp.substr(i,currentFrame->exp.length());
                 }
                 i = -1;
                 currentFrame->index = i;
             }
             else
             {
            	 uint32_t startOfparams = currentFrame->exp[varName.end+1] =='('?
                 varName.end+1:currentFrame->exp.size();
                 //Variable handling section
            	 currentFrame->index = varName.start;
                 {
                	// std::cout<<"cframe exp: "<<currentFrame->exp<<"\nP use count: "<<currentFrame->getParams()->getUseCount()<<"\n";
                 }
            	 if(Object *data = resolveNextObjectExpression(currentFrame, varName))
                 {

                	 startOfparams = currentFrame->exp[varName.end+1] =='('?
                     varName.end+1:currentFrame->exp.size();
                     uint32_t endOfParams = data->setParams(currentFrame->exp, startOfparams);
                     uint32_t startOfEnd = data->params.size()==0?varName.end+1:varName.start+varName.data.size()-1+endOfParams;
                    // std::cout<<"parsed obj: "<<data->toString()<<"\nsoe: "<<startOfEnd<<"\n";
                 //creates a new set of stack frames, one for the function/var and one for resolution,
                 //and calculation of each of it's parameters
                 //also returns function/var's value to this function's initialOperands stack
                	 //std::cout<<"\nexiisting obj loaded: "<<data->id<<"\n";
                	 createFrame(executionStack, currentFrame,
                                data,startOfEnd,hashFunctionCall(data->id));
                	 goto new_frame_execution;
                 }
                 else if(currentFrame->getParams()->getUseCount() < currentFrame->getParams()->size())
                 {

                	 if(currentFrame->exp[varName.end+1] == '.' || currentFrame->exp[varName.end+1] == '[')
                	 {
                		 std::stringstream error;
                		 error<<"Error while attempting to resolve parameter "<<varName.data<<" at position "<<to_string(varName.start)<<
                				 "\nfunction parameters must be defined when passing a reference to an object as parameter.\nTry wrapping the function in (";

                		 for(int i = 0;  i < currentFrame->getParams()->size();i++)
                			 error<<"<param"<<i+currentFrame->getParams()->getUseCount()+0<<">,";

                		 if(currentFrame->getParams()->size())
                			 error.seekp(-1,error.cur);
                		 error<<"){<original function body>}";
                		 error<<" to declare the function parameters";
                		 throw error.str();
                	 }
                     Object *paramVar = (*currentFrame->getParams())[currentFrame->getParams()->size() - 1 - currentFrame->getParams()->getUseCount()];
                     ++(*currentFrame->getParams());


                     //paramVar->id = varName.data;
                     Object *obj = ((*currentFrame->getParamMemory())[varName.data] = paramVar);
                     if(obj->id.size() == 0)
                     {
                    	 obj->id = varName.data;
                     }
                     {
                    //	 std::cout<<"\nnew param loaded as objid: "<<obj->id<<"\n";
                     }
                     uint32_t endOfParams = paramVar->setParams(currentFrame->exp, startOfparams);
                     uint32_t startOfEnd = paramVar->params.size()==0?varName.end+1:varName.start+varName.data.length()-1+endOfParams;
                     //std::cout<<"\nsoe: "<<startOfEnd<<" exp: "<<currentFrame->exp<<"\n";
                     createFrame(executionStack, currentFrame,
                    		 obj,startOfEnd,hashFunctionCall(paramVar->id));
                     goto new_frame_execution;
                 }
                 else
                 {
                    #if THROWERRORS == 1
                     if(cmpstr(varName.data,"inf"))
                         throw std::string("Arithmetic Overflow in column: "+to_string(i));
                     else if(cmpstr(varName.data,"nan"))
                         throw std::string("Error Undefined result calculated");
                     else
                         throw std::string("Invalid reference: "+varName.data+" in column: "+to_string(i));
                    #endif
                 }
             }

         }
         else
            if (currentChar==')')
            {
              currentFrame->initialOperators.top(peeker);
              while(!currentFrame->initialOperators.isEmpty() && peeker != '(')
              {
                  currentFrame->initialOperands.top(and1);
                  currentFrame->initialOperands.pop();
                processOperands.push(and1);
                processOperators.push(peeker);
                currentFrame->initialOperators.pop();
                currentFrame->initialOperators.top(peeker);
              }
              if(peeker != '(')
                  throw std::string("Error no opening parenthesis for closing ) at position: "+
                		  to_string(i+1));
              currentFrame->initialOperands.top(and2);
              currentFrame->initialOperands.pop();
              processOperands.push(and2);
              currentFrame->initialOperators.pop();
              //Send expression in parentheses to processStack for evaluation
              try{
                  and2 = processStack(processOperands,processOperators);
              }catch(std::string &error)
              {
                  throw std::string("In Expression: "+currentFrame->exp+" "+error);
              }

              currentFrame->initialOperands.push(and2);
            }
            //Section to parse numeric values from expression as a string to be inserted into
            //initialOperands stack
            else if(isNumeric(currentChar) ||
            (currentChar == '-' && isNumeric(currentFrame->exp[i+1]) &&
            		((i == 0 && !currentFrame->isAugmented()) ||
            				(i > 0 && (isNonParentheticalOperator(currentFrame->exp[i-1]) || currentFrame->exp[i-1] =='('))
            						)
            )
				|| currentChar == '.')
            {
                double nextDouble = getNextDouble(currentFrame->exp,i);
                currentFrame->initialOperands.push(nextDouble);
            }
            else if(isOperator(currentChar))
            {
                currentFrame->initialOperators.push(currentChar);
            }
            else if(currentChar == ';')
            {
            	lineCount++;
                currentFrame->clearStackIfAnotherStatementProceeds(isOperator);
            }


          }
        //Finally pop all values off initial stack onto final stacks for processing
        while(!currentFrame->initialOperands.isEmpty() || !currentFrame->initialOperators.isEmpty())
        {
          if(!currentFrame->initialOperands.isEmpty())
          {
              currentFrame->initialOperands.top(and1);
              processOperands.push(and1);
              currentFrame->initialOperands.pop();
          }
          if(!currentFrame->initialOperators.isEmpty())
          {
              currentFrame->initialOperators.top(peeker);
              processOperators.push(peeker);
              currentFrame->initialOperators.pop();
          }
        }
        //process values in stacks, and return final solution
        data = processStack(processOperands, processOperators);
        if(currentFrame->getReturnPointer())
        {
            if(currentFrame->isFunction())
            {
                if(*boolsettings["memoize"])
                {
                    memoPad[currentFrame->memoPointer] = data;
                }
                if(currentFrame != frame && *boolsettings["o"] && std::to_string(data).length() != MAX.length()){
                    std::cout<<"Returning value: "<<data<<'\n';
                }
            }
            currentFrame->returnResult(data, memory);
        }
        executionStack.pop();
        frame->returnPointer = rtnFrame;
        if(currentFrame->isDynamicAllocation())
        {
            delete currentFrame;
            deallocated += sizeofFrame;
        }
        currentFrame = nullptr;
    }

    }catch(std::string &error)
    {
    	clearStackOnError(true, error, executionStack, currentFrame, frame, rtnFrame);
        throw error;
    }
    catch(int exitCode)
    {
    	clearStackOnError(false, frame->exp, executionStack, currentFrame, frame, rtnFrame);
    	throw exitCode;
    }
    currentStack = nullptr;
    return data;
}
#include <sstream>
void clearStackOnError(bool printStack, std::string &error, linkedStack<AscalFrame<double>* > &executionStack, AscalFrame<double>* currentFrame, AscalFrame<double>* frame, AscalFrame<double>* rtnFrame)
{
	frame->returnPointer = rtnFrame;
	        if(printStack)
	        {
	        	std::stringstream data;
	            size_t errorStartMarker = error.find("\1\1\2\3");
	            if(errorStartMarker<0 || errorStartMarker > error.length())
	            	errorStartMarker = 0;
	            data<<error.substr(0, errorStartMarker);
	            while(!executionStack.isEmpty())
	            {
	                executionStack.top(currentFrame);
	                	data << currentFrame->getType() << "   ~:"<<currentFrame->exp<<'\n';
	                if(currentFrame->isDynamicAllocation()){
	                    delete currentFrame;
	                    deallocated += sizeofFrame;
	                }
	                executionStack.pop();
	            }
	            data << "\1\1\2\3";
	            data << error.substr(errorStartMarker);
	            error = data.str();
	        }
	        else
	        {
	            while(!executionStack.isEmpty())
	            {
	                executionStack.top(currentFrame);
	                if(currentFrame->isDynamicAllocation()){
	                    delete currentFrame;
	                    deallocated += sizeofFrame;
	                }
	                executionStack.pop();
	            }
	        }
}
static char periodCount;
static bool isADouble;
bool isDouble(std::string &exp)
{
    isADouble = true;
    periodCount = 0;
    for(uint16_t i = (exp[0] == '-'); isADouble && i < exp.length(); i++)
    {
        //to avoid branching I'm doing boolean arithmetic to determine if a string is a double
        periodCount += (exp[i] == '.');
        isADouble = ((exp[i] >= '0' && exp[i] <= '9' ) || (periodCount == 1 && exp[i] == '.') ||
                     (exp[i] == ';' && (exp[i+1] == 0 || exp[i+2] == 0|| exp[i+3] == 0) ));
    }
    return isADouble;
}
template <typename t>
void createFrame(linkedStack<AscalFrame<t>* > &executionStack, AscalFrame<t>* currentFrame, Object *obj, int i,uint64_t hash)
{
    //save index
    currentFrame->index = i;
    if(isDouble(obj->getInstructions()))
    {
        int ib = 0;
        currentFrame->initialOperands.push(getNextDoubleS(obj->getInstructions(), ib));
        varCount++;
        if(*boolsettings["o"])
        	std::cout<<"Reading variable: "<<obj->id<<" = "<<obj->getInstructions()<<'\n';
    }
    else
    {
        frameCount++;
        if(*boolsettings["o"])
        {
        	std::stringstream data;
        	for(SubStr &s:obj->params)
        	{
        		data << s.data << ",";
        	}
        	std::cout<<"Parsing params then executing: "<<obj->id<<'('<<data.str()<<')'<<'\n';
        }
        //create and set new frame expression
        FunctionFrame<t> *newFrame = new FunctionFrame<t>(nullptr,nullptr,nullptr);
        allocated += sizeofFrame;
        //newFrame->stackIndex = currentFrame->stackIndex+1;
        newFrame->exp = obj->getInstructions();
        if(memory.count(obj->id) == 0)
        	(*newFrame->getParamMemory())[obj->id] = obj;
        newFrame->memoPointer = hash;
        //Create new frame, and set return pointer
        newFrame->returnPointer = currentFrame;
        //Context object ptr for this keyword resolution
        newFrame->setContext(obj);
        //push new frame
        executionStack.push(newFrame);
        for(int i = 0; i < obj->params.size(); i++)
        {
        	uint8_t startingIndex = 0;
        	while(obj->params[i].data[startingIndex++] == ' '){};
            allocated += sizeofFrame;
            if(obj->params[i].data[--startingIndex] != '&')
            {
            	//std::cout<<"Loading param: "<<obj->params[i].data.substr(startingIndex)<<"\nfor obj:"<<obj->id<<"\n";
                //create and set new frame expression
                ParamFrame<t>* pFrame = new ParamFrame<t>(currentFrame->getParams(),currentFrame->getParamMemory(),currentFrame->getLocalMemory());
               // std::cout<<"Param exec frame mem: "<<pFrame->memToString();
                pFrame->exp = obj->params[i].data;
                //Create new frame, and set return pointer
                pFrame->returnPointer = newFrame;
                //Context object ptr for this keyword resolution
                pFrame->setContext(currentFrame->getContext());
                //push new frame
                executionStack.push(pFrame);
            }
            else
            {
            	//std::cout<<"Loading param ptr: "<<obj->params[i].data.substr(startingIndex)<<"\nfor obj:"<<obj->id<<"\n";
                ParamFrameFunctionPointer<t>* pFrame = new ParamFrameFunctionPointer<t>(currentFrame->getParams(),currentFrame->getParamMemory(),currentFrame->getLocalMemory());
                uint32_t frameIndexBackup = currentFrame->index;
                SubStr startOfParam = getVarName(obj->params[i].data.substr(startingIndex), startingIndex);
                startOfParam.end = obj->params[i].start+startOfParam.data.size();
                //std::cout<<"pars: "<<startOfParam.data<<"\nstart: "<<startOfParam.start<<" end: "<<startOfParam.end<<"\n";
                pFrame->obj = resolveNextObjectExpression(currentFrame, startOfParam);
                //std::cout<<"Object getting ref passed to it: "<<obj->id<<"\nParam ref passed obj: "<<pFrame->obj->toString();
                currentFrame->index = frameIndexBackup;
                //Create new frame, and set return pointer
                pFrame->returnPointer = newFrame;
                //push new frame
                executionStack.push(pFrame);
            }
        }
    }
}
template <class t>
t processStack(stack<t> &operands,stack<char> &operators)
{

#if THROWERRORS == 1
    if(operands.isEmpty() && operators.isEmpty()){}
    else if(operators.size() < operands.size()-1 ||
       operators.size() >= operands.size())
    {
        std::string error;
        if(operators.size() >= operands.size())
        {
            error = std::string("Error, too many operators.");
        }
        else
        {
            error = std::string("Error, too many operands.");
        }
        std::cerr<<"Operands ";
        printStack(operands);
        std::cerr<<"Operators ";
        printStack(operators);
        throw error;
    }
#endif
      t result = 0, and1 = 0, and2 = 0;
      char firstOperator, nextOperator;
      //loop to process expression saved in stack
     while(!operators.isEmpty() && operands.length()>1)
     {
    //initialize values so we can make sure no garbage, or previous values is in the variables
    //so top function can work properly
         nextOperator = 'a';
         firstOperator = 'a';
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
      static Calculator<t> c;
      result = c.calc(firstOperator,and1,and2);
      operands.push(result);
      if(*boolsettings["o"])
        std::cout<<and1<<firstOperator<<and2<<" = "<<result<<'\n';
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

    //get result from processing expression
    operands.top(result);
    operands.pop();
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
  int priority = -1;
  switch (ator){
  case '(':
	  priority = 0;
	  break;
  case ')':
	  priority = 0;
	  break;
  case '^':
	  priority = 90;
	  break;
  case '*':
	  priority = 80;
	  break;
  case '/':
	  priority = 80;
	  break;
  case '%':
	  priority = 80;
	  break;
  case '$':
	  priority = 80;
	  break;
  case 'P':
	  priority = 80;
	  break;
  case 'C':
	  priority = 80;
	  break;
  case '+':
	  priority = 70;
	  break;
  case '-':
	  priority = 70;
	  break;
  case '=':
	  priority = 60;
	  break;
  case '>':
	  priority = 60;
	  break;
  case '<':
	  priority = 60;
	  break;

  }
  return priority;
}
template <class t>
t doubleModulus(t &and1,t &and2)
{
    t result;
      if(and2 != 0)
      {
        	  double quotient = and1/and2;
        	  double flooredQuotient;
        	  modf(quotient,&flooredQuotient);
        	  result = and1 - and2*flooredQuotient;

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
          throw std::string("Error modulus by zero.");
      }
      return result;
}
template <class t>
void initOperations()
{
    operations['+'] = "Adds left operand to right";
    operations['-'] = "Subtracts right operand from left";
    operations['*'] = "";
    operations['/'] = "";
    operations['%'] = "";
    operations['^'] = "";
    operations['P'] = "";
    operations['C'] = "";
    operations['@'] = "";
    operations['$'] = "";
    operations['='] = "";
    operations['<'] = "";
    operations['>'] = "";
}

bool isOperator(char s)
{
  return  isNonParentheticalOperator(s) || s == '(' || s == ')';
}
bool isNonParentheticalOperator(char s)
{
    return operations.count(s);
}
bool isNumeric(char c)
{
  return (c >= 48 && c<58);
}
