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


#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <unordered_map>
#include <map>
#include <chrono>
#include <ctime>
#include <cmath>
#include <algorithm>
#include <string>
#include <iostream>
#include <fstream>

#include "AscalFrame.hpp"
#include "Object.hpp"
#include "setting.hpp"
#include "stack.hpp"
#include "Vect2D.hpp"

#define DEBUG 1
#define THROWERRORS 1

static const std::string VERSION = "2.01";
struct SubStr{
    std::string data;
    int start,end;
    SubStr(std::string data,int start,int end):data(data),start(start),end(end){}
};
static uint32_t frameCount = 1;
static uint32_t varCount = 0;

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
std::string (*)(AscalFrame<double>*,bool)> inputMapper;

template <class t>
static std::map<char,t (*)(t&,t&)> operations;
static size_t rememberedFromMemoTableCount;
//End Program Global Memory Declaration
/////////////////////////////





#if defined(WIN32) || defined(_WIN32)
#define PATH_SEPARATOR '\\'
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
//Parallel reimann sums
//thread count = number of partitions/20 + 1
//dynamic array as long as thread count passed to each thread, along with thread index number to save result of calculation
//when all threads are finished running sum up result in array, maybe use simd if

std::string to_string(double input);
const std::string MAX = std::to_string(std::numeric_limits<double>::max());
//////////////////////////////////////////////////////////
//Start Ascal System Defined  Keyword functionality Executed by lookup in inputMapper unordered_map

template <class t>
t callOnFrame(AscalFrame<t>* callingFrame,std::string subExp);
template <class t>
t doubleModulus(t &and1,t &and2);
void printVar(const std::string &expr,bool saveLast);
void printHelpMessage(const std::string &expr);
void updateBoolSetting(AscalFrame<double>* frame);

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
        while(delta > 0.000001 && count < 10000)
        {
            num = pow(result,p) - base;
            den = pow(result,newPow)*p;
            prev = result;
            result = result - num/den;
            delta = result - prev;
            if(delta < 0)
                delta *= -1;

            count++;
        }
    }
    else if(b == 0)
        result = 0;
    else
        throw std::string("Error cannot compute complex radicals, radicand should be a positive number.");
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

int lineCount = 1;
void get_line(std::istream &in,std::string&data)
{
    lineCount++;
    getline(in,data);
}

void printLoadedMemMessage(Object function)
{
    std::cout<<"Loaded Function: "<<function.id<<"\nexpression: "<<function.instructionsToFormattedString()<<std::endl<<std::endl;
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
        std::cerr<<"Error loading "<<function.id<<" due to the use of an operator in its name"<<std::endl;
        std::cerr<<"Operators: "<< '=' <<','<< '>' << ',' << '<' <<','<< '$' <<','<<
                'P' <<','<< '@' <<','<< '+' <<','<< '-' <<','<<
                '*'<<','<< '/' <<','<< '^' <<','<< '%' <<','<< 'C';
        throw std::string("Error Operator in Variable Name");
    }
    if(*boolsettings["p"])
        printLoadedMemMessage(function);
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
    loadFn(Object("fibr","(x){loc fr = (x){when x > 1 then fr(x-1)+fr(x-2) else x end;};memoize 1;fr(x);memoize 0;}",""));
    loadFn(Object("rfibr","(x){when x > 1 then rfibr(x-1)+rfibr(x-2) else x end;}",""));
    loadFn(Object("ack","when m=0 + n*0  then n+1 when n=0 then ack(m-1,1) when  m+n > 0 then ack(m-1,ack(m,n-1)) else 0 end",""));
    loadFn(Object("fastAck","(m,n){memoize 1;ack(m,n);memoize 0;}",""));
    loadFn(Object("gcd","a*b*0+ when b=0 then a when a=0=0 then gcd(b,a%b) end",""));
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
    loadFn(Object("println","(x){loc counter = 0;while counter<x{set counter = counter +1;printStr \"endl\";};null",""));
    loadFn(Object("clear","println(150)",""));
    loadFn(Object("floor","x-x%1",""));
    loadFn(Object("ceiling","when x%1=0 then x else x+1-x%1 end",""));
    loadFn(Object("round","when x%1<0.5 then floor(x) else ceiling(x) end",""));

    //Stats Functions
    Object binProbDist("binprob","(total C events) * probabilityOfSuccess^events * (1-probabilityOfSuccess)^(total-events)","");
    loadFn(binProbDist);

    //Dietician Functions
    Object eKCal("ekcal","(0*male*kg*cm*age*activity+ true(male)*5+not(male)*-161+(10*kg)+(6.25*cm)-(5*age))*activity","");
    loadFn(eKCal);
    //Object desWeight("desw","(percent/100)*(usual-desired)+desired","");
    //loadFn(desWeight);

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
        std::cout<<std::endl<<"Function Name: "<<key<<"\nFunction Expression: "<<value.instructionsToFormattedString()<<std::endl;
    }
    std::cout<<std::endl<<"End of All Functions in Memory."<<std::endl;
}
void printAllUDF()
{
    std::cout<<"User Defined Functions:"<<std::endl;
    for(Object & data:userDefinedFunctions)
    {
        std::cout<<std::endl<<"Function Name: "<<data.id<<std::endl<<"Function Expression: "<<data.instructionsToFormattedString()<<std::endl;
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
    inputMapper["print"] = printDefaultAction;
    inputMapper["printStr"] = printStringAction;
    inputMapper["input"] = inputAction;
    inputMapper["import"] = importAction;
    inputMapper["while"] = whileAction;
    inputMapper["when"] = whenAction;
    inputMapper["plot"] = plotAction;
    inputMapper["quit"] = quitAction;
    inputMapper["printc"] = printCommand;
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

    for(int i = 1;i<argc;i++)
    {
        std::map<std::string,Object> localMemory;
        std::map<std::string,Object> paramMemory;
        arg = argv[i];
        if(arg.size() > 4)
        {
            if(arg[arg.size()-4] == '.' && toLower(arg[arg.size()-3]) == 'a' &&
                    toLower(arg[arg.size()-2]) == 's' && toLower(arg[arg.size()-1]) == 'l')
            {
                std::cout<<"Loading file: "<<arg<<std::endl;
                loadFile(arg,0);
                continue;
            }
        }
        try{
            FunctionFrame<double>* frame = new FunctionFrame<double>(nullptr,nullptr,nullptr);
            frame->exp = arg;
            interpretParam(frame,true);
    }
    catch(std::string &exception)
    {
        std::cerr<<exception<<std::endl;
        std::cerr<<"Failed to exec: "<<arg<<std::endl;
    }
    }
  }
  //End of section interpreting program parameters from command line
  //beginning of main program loop, if only one parameter is given in the
  //command line by the user loop will not run, unless that param was the loop command
  //by default the loop runs

  while(*boolsettings["l"])
  {

          //Interpreter prompt to let user know program is expecting a command/expression
          FunctionFrame<double>* frame = new FunctionFrame<double>(nullptr,nullptr,nullptr);
          std::cout<<std::endl<<">>";
          get_line(std::cin, arg);
          arg = getExpr(arg,0).data;
          frame->exp = arg;
            std::map<std::string,Object> localMemory;
            std::map<std::string,Object> paramMemory;
            try{

                interpretParam(frame, true);
        }
        catch(std::string &exception)
        {
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
      std::cerr<<"exit code: "<<exitCode<<std::endl;
      return exitCode;
  }
  return 0;
}
double floor(double input)
{
    double one = 1;
    return input - doubleModulus(input,one);
}
/*#include <math.h>
std::string to_string(double input)
{

    std::vector<char> output;
    if(input > 0)
    {
        double integerPart;
        double fractionalPart = modf(input,&integerPart);
        double ten = 10;
        while(integerPart >= 1)
        {
            //std::cout<<"Char Pushed: "<<48+floor(doubleModulus(integerPart,ten))<<std::endl;
            output.insert(output.begin(),48+floor(doubleModulus(integerPart,ten)));
            integerPart /= ten;
        }
        if(integerPart == 0)
        {
            output.push_back('0');
        }
        output.push_back('.');
        if(fractionalPart == 0)
        {
            output.push_back('0');
        }
        int precisionCounter = 0;
        while(fractionalPart > 0 && precisionCounter < ten)
        {
            fractionalPart *= ten;
            int newData = floor(fractionalPart);
            output.push_back(48+newData);
            fractionalPart -= newData;
            precisionCounter++;
        }

    }
    else
        output.push_back(48);
    return std::string(output.begin(),output.end());
//    return std::to_string(input);
}*/
std::string to_string(double input)
{
    int offsetFromEnd = 0;
    bool brek = true;
    std::string data = std::to_string(input);
    /*while(data[data.length()-offsetFromEnd-1] == '0')
        offsetFromEnd++;*/
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
/*std::string testRuntimeAction(AscalFrame<double>* frame,bool s)
{
    std::string variableTest,functionNoParamsTest,functionOneParamTest,functionTwoParamTest,

}*/
std::string printStringAction(AscalFrame<double>* frame,bool s)
{
        //int startingIndex = expr.find("input");
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
            //std::cout<<"input In While: "<<std::endl;
                if(result[index] == '(')
                {
                    //std::cout<<"input In if: "<<std::endl;
                    SubStr subexp = getExpr(result,index,'(',')',';');
                    std::string value = to_string(callOnFrame(frame, subexp.data));
                    std::string first = result.substr(0,index);
                    std::string last = result.substr(index+subexp.end-3,frame->exp.size());

                //std::cout<<"inputAction first: "<<first<<" value: "<<value<<"\nLast: "<<last<<std::endl;

                    result = first+value+last;
                    index = first.size()+value.size()-1;
                //std::cout<<"inputActie"<<result.substr(index,result.length());
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
        //int startingIndex = expr.find("input");
        const int startOfPrint = frame->exp.find("\"")+1;
        int endOfPrint = frame->exp.find("\"",startOfPrint);

        std::string result;
        if(endOfPrint != -1)
        {
            result = frame->exp.substr(startOfPrint,endOfPrint-startOfPrint);
            int index = 0;
            SubStr subexp("",0,0);
            while(result[index] && result[index] != '\"')
            {
            //std::cout<<"input In While: "<<std::endl;
                if(result[index] == '(')
                {
                    //std::cout<<"input In if: "<<std::endl;
                    SubStr subexp = getExpr(result,index,'(',')',';');
                    std::string value = to_string(callOnFrame(frame,subexp.data));
                    std::string first = result.substr(0,index);
                    std::string last = result.substr(index+subexp.end-3,frame->exp.size());

                //std::cout<<"inputAction first: "<<first<<" value: "<<value<<"\nLast: "<<last<<std::endl;

                    result = first+value+last;
                    index = first.size()+value.size()-1;
                //std::cout<<"inputActie"<<result.substr(index,result.length());
                }
                else if(result[index] == 'e' && result.size()-index>3 &&
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
            //result = "number: ";
            endOfPrint = frame->index+4;
        }
        std::cout<<result;
        std::string input;
        get_line(std::cin,input);
        std::string inputReduced = to_string(callOnFrame(frame,input));
    return "a"+inputReduced+frame->exp.substr(endOfPrint+1,frame->exp.size());

}
void loadFile(const std::string & expr,int startIndex)
{
    std::ifstream inputFile;
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
        int count = 0;
        std::vector<char> data;
        for(char &c:filePath)
        {

            if(c == '.' && (count < filePath.size() - 4 || toLower(filePath[filePath.size()-3]) != 'a' ||
                    toLower(filePath[filePath.size()-2]) != 's' || toLower(filePath[filePath.size()-1]) != 'l'))
                c = (PATH_SEPARATOR);
            count++;
        }
        inputFile.open(filePath);
        if(!inputFile)
        {
            inputFile.open(filePath+".asl");
        }

    }
    if(!inputFile)
    {
        throw std::string("Malformed path: "+filePath);
    }
    std::cin.rdbuf(inputFile.rdbuf());
    int locLineCount = lineCount;
    while(inputFile)
    {
        FunctionFrame<double>* calledFunctionMemory = new FunctionFrame<double>(nullptr,nullptr,nullptr);
        get_line(inputFile, calledFunctionMemory->exp);
        try{

            calculateExpression<double>(calledFunctionMemory);
        }
        catch(std::string &exception)
        {
            std::cerr<<exception<<std::endl;
            std::cerr<<"On Line: "<<(lineCount - locLineCount)<<std::endl;
        }
    }
    std::cin.rdbuf(stream_buffer_cin);
}
std::string importAction(AscalFrame<double>* frame,bool s)
{
    bool print = *boolsettings["p"];
    *boolsettings["p"] = false;
    bool timeCalc = *boolsettings["t"];
    *boolsettings["t"] = false;
    try{
        loadFile(frame->exp,7);
    }catch(std::string &exception)
    {
        *boolsettings["p"] = print;
        *boolsettings["t"] = timeCalc;
        throw std::string("while importing file\n"+exception);
    }
    *boolsettings["p"] = print;
    *boolsettings["t"] = timeCalc;
    return MAX;
}
std::string runAction(AscalFrame<double>* frame,bool s)
{
    try{
        loadFile(frame->exp,4);
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
    if(*boolsettings["p"])
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
        value = callOnFrame(frame,last);
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
        value = callOnFrame(frame,last);
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
            std::cout<<"Erased "<<name<<" from memory."<<std::endl;
        }
        else if(frame->getLocalMemory()->count(name) != 0)
        {
            frame->getLocalMemory()->erase(name);
        }
        else
        {
            std::cout<<"    Error could not find "<<name<<" in memory to delete."<<std::endl;
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
std::string letNewVar(AscalFrame<double>* frame,bool saveLast)
{
    //find index of first char that is alpha find index of last alpha before space or =

                //ignore everything till the =

                //use expr.substr(firstIndex,LastIndex) as var name and id and key in hash map use everything after = as expression

                //set expr = expression part (everything after = ignoring spaces) of object and run calculation

                SubStr exPart = getExpr(frame->exp,frame->exp.find('=',frame->index)+1);
                SubStr newVarPart = getNewVarName(frame->exp);
                Object var(newVarPart.data,exPart.data,frame->exp.substr(newVarPart.end + 1,exPart.start - 1 ));

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
std::string constNewVar(AscalFrame<double>* frame,bool saveLast)
{

    SubStr exPart = getExpr(frame->exp,frame->exp.find('=',frame->index)+1);
    SubStr newVarPart = getVarName(frame->exp,frame->index+4);
    bool print = *boolsettings["p"];
    frame->exp = exPart.data;
    std::string value = to_string(calcWithOptions(frame));
    *boolsettings["p"] = print;
    Object var(newVarPart.data,value,"");


    std::vector<Object>::iterator position = std::find(userDefinedFunctions.begin(), userDefinedFunctions.end(), memory[var.id]);
                            if(position != userDefinedFunctions.end())
                                userDefinedFunctions.erase(position);

    //set var defined's value in hash map
    loadUserDefinedFn(var);
    return value;
}
std::string locNewVar(AscalFrame<double>* frame,bool saveLast)
{
    SubStr localName = getVarName(frame->exp,frame->exp.find("loc",frame->index)+4);
    SubStr subexp = getExpr(frame->exp,frame->exp.find('=')+1);

    Object newLocalVar(localName.data,subexp.data,"");
    if(*boolsettings["o"])
    {
        std::cout<<std::endl<<"Local var name: "<<localName.data<< " exp: "<<newLocalVar.instructionsToFormattedString()<<std::endl;
    }
    (*frame->getLocalMemory())[newLocalVar.id] = newLocalVar;
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
        std::cout<<std::endl<<"Const local var name: "<<localName.data<< " exp: "<<value<<std::endl;
    }
    (*frame->getLocalMemory())[newLocalVar.id] = newLocalVar;
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

std::string plotAction(AscalFrame<double>* frame,bool saveLast)
{
    const int plotKeyWordIndex = frame->exp.find("plot",frame->index);
    const int endOfFun = frame->exp.find(",",frame->index);
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
    for(int j = 0;j<functions.size();j++)
    {
        std::string function = functions[j];
        if(*boolsettings["o"])
            std::cout<<"\nProcessing: "<<function<<"\n";
        for(int i = 0;i<tableWidth;i++)
        {
            xi = xMin+dx*(i);
            FunctionFrame<double>* calledFunction = new FunctionFrame<double>(nullptr,nullptr,nullptr);
            calledFunction->exp = function+'('+to_string(xi)+')';
            outPuts.push_back(
                    calculateExpression<double>(calledFunction));
            sumArea[j] += outPuts.get(i,j)*dx;
        }
    }

    double yClosestToZero = yMax;
    for(int y = tableHeight+1;y>=0;y--)
    {
        if(abs(yMin+dy*y)<yClosestToZero)
            yClosestToZero = yMin+dy*y;
    }
    double xClosestToZero = xMax;
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
    const double xStepSize = (xMax-xMin)/(tableWidth>0?tableWidth:1);
    double dx = (xMax-xMin)/tableWidth;
    double xi;
    double thisIndex = 0;
    double lastIndex = 0;
    for(int j = 0;j<functions.size();j++)
    {
        std::string function = functions[j];
        for(int i = 0;i<tableWidth;i++)
        {
            xi = xMin+dx*(i+1);
            FunctionFrame<double>* calledFunction = new FunctionFrame<double>(nullptr,nullptr,nullptr);
            calledFunction->exp = function+'('+to_string(xi)+')';
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

    return MAX;
}
std::string existsAction(AscalFrame<double>* frame,bool saveLast)
{
    int index = frame->exp.find("exists",frame->index)+6;
    while(frame->exp[index] == ' ')
        index++;
    SubStr varName = getVarName(frame->exp,index);
        index += varName.data.length();

    if(frame->getLocalMemory()->count(varName.data) || frame->getParamMemory()->count(varName.data) || memory.count(varName.data))
    {
        return "a1;"+frame->exp.substr(index,frame->exp.size());
    }
    return "a0;"+frame->exp.substr(index,frame->exp.size());
}
std::string sinAction(AscalFrame<double>* frame,bool saveLast)
{
    int index = frame->exp.find("sin",frame->index)+3;
    while(frame->exp[index] == ' ')
        index++;
    SubStr exp = getExpr(frame->exp,index, '(', ')','\0');
        index += exp.data.length()-3;
    double input = callOnFrame(frame,exp.data);
    frame->initialOperands.push(sin(input));
    if(*boolsettings["o"])
    {
        std::cout<<"sin("<<input<<") = "<<sin(input)<<'\n';
    }
    return 'a'+frame->exp.substr(index,frame->exp.size());
}
std::string cosAction(AscalFrame<double>* frame,bool saveLast)
{
    int index = frame->exp.find("cos",frame->index)+3;
    while(frame->exp[index] == ' ')
        index++;
    SubStr exp = getExpr(frame->exp,index, '(', ')','\0');
        index += exp.data.length()-3;
    double input = callOnFrame(frame,exp.data);
    frame->initialOperands.push(cos(input));
    if(*boolsettings["o"])
    {
        std::cout<<"cos("<<input<<") = "<<cos(input)<<'\n';
    }
    return 'a'+frame->exp.substr(index,frame->exp.size());
}
std::string tanAction(AscalFrame<double>* frame,bool saveLast)
{
    int index = frame->exp.find("tan",frame->index)+3;
    while(frame->exp[index] == ' ')
        index++;
    SubStr exp = getExpr(frame->exp,index, '(', ')','\0');
        index += exp.data.length()-3;
    double input = callOnFrame(frame,exp.data);
    frame->initialOperands.push(tan(input));
    if(*boolsettings["o"])
    {
        std::cout<<"tan("<<input<<") = "<<tan(input)<<'\n';
    }
    return 'a'+frame->exp.substr(index,frame->exp.size());
}
std::string arctanAction(AscalFrame<double>* frame,bool saveLast)
{
    int index = frame->exp.find("arctan",frame->index)+6;
    while(frame->exp[index] == ' ')
        index++;
    SubStr exp = getExpr(frame->exp,index, '(', ')','\0');
        index += exp.data.length()-3;
    double input = callOnFrame(frame,exp.data);
    frame->initialOperands.push(atan(input));
    if(*boolsettings["o"])
    {
        std::cout<<"arctan("<<input<<") = "<<atan(input)<<'\n';
    }
    return 'a'+frame->exp.substr(index,frame->exp.size());
}
std::string arcsinAction(AscalFrame<double>* frame,bool saveLast)
{
    int index = frame->exp.find("arcsin",frame->index)+6;
    while(frame->exp[index] == ' ')
        index++;
    SubStr exp = getExpr(frame->exp,index, '(', ')','\0');
        index += exp.data.length()-3;
    double input = callOnFrame(frame,exp.data);
    frame->initialOperands.push(asin(input));
    if(*boolsettings["o"])
    {
        std::cout<<"arcsin("<<input<<") = "<<asin(input)<<'\n';
    }
    return 'a'+frame->exp.substr(index,frame->exp.size());
}
std::string arccosAction(AscalFrame<double>* frame,bool saveLast)
{
    int index = frame->exp.find("arccos",frame->index)+6;
    while(frame->exp[index] == ' ')
        index++;
    SubStr exp = getExpr(frame->exp,index, '(', ')','\0');
        index += exp.data.length()-3;
    double input = callOnFrame(frame,exp.data);
    frame->initialOperands.push(acos(input));
    if(*boolsettings["o"])
    {
        std::cout<<"arccos("<<input<<") = "<<acos(input)<<'\n';
    }
    return 'a'+frame->exp.substr(index,frame->exp.size());
}
std::string setAction(AscalFrame<double>* frame,bool saveLast)
{
    SubStr varName = getVarName(frame->exp,frame->exp.find("set",frame->index)+4);
    int startIndex = frame->exp.find("=",varName.end)+1;
    while(frame->exp[startIndex] && frame->exp[startIndex] == ' ')
        startIndex++;
    SubStr subexp = getExpr(frame->exp,startIndex);
    std::string value = to_string(callOnFrame(frame,subexp.data));
    if(frame->getLocalMemory()->count(varName.data))
    {
        (*frame->getLocalMemory())[varName.data] = Object(varName.data,value,"");
        if(*boolsettings["o"])
            std::cout<<"Local Var: "<<varName.data<<" set to: "<<value<<"\n\n";
    }
    else if(frame->getParamMemory()->count(varName.data))
    {
        (*frame->getParamMemory())[varName.data] = Object(varName.data,value,"");
        if(*boolsettings["o"])
            std::cout<<"Parameter: "<<varName.data<<" set to: "<<value<<"\n\n";
    }
    else if(memory.count(varName.data))
    {
        memory[varName.data] = Object(varName.data,value,"");
        if(*boolsettings["o"])
            std::cout<<"Global Var: "<<varName.data<<" set to: "<<value<<"\n\n";
    }
    else
    {
        throw std::string("Error invalid variable name "+varName.data);
    }
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
    bool printTime = *boolsettings["t"];
    *boolsettings["t"] = false;
    while(frame->exp[index] == ' ')
        index++;
    //std::cout<<"\nin Else ifFlag: "<<ifFlag<<" result flag for if: "<<ifResultFlag<<std::endl;
    //std::cout<<"comingfromElse in Else: "<<comingfromElse<<std::endl;
    SubStr codeBlock = getExpr(frame->exp,index);
    //std::cout<<"else Originial Str: "<<expr.substr(index,expr.length())<<std::endl;
    //std::cout<<"else Code Block: "<<codeBlock.data<<std::endl;
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
            //std::cout<<" yigi: "<<expr.substr(index,expr.length())<<std::endl;
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
                *boolsettings["t"] = printTime;
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
    }
    frame->setIfFlag(false);
    frame->index = 0;
    *boolsettings["t"] = printTime;
    return "a"+frame->exp.substr(index,frame->exp.size());
}
std::string ifAction(AscalFrame<double>* frame,bool saveLast)
{

    std::string expBkp = frame->exp;
    //std::cout<<"exp in if: "<<expr<<std::endl;
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
        //std::cout<<"BoolExpression: "<<booleanExpression;
        bool printTime = *boolsettings["t"];
        if(!*boolsettings["p"])
        {
            *boolsettings["t"] = false;
        }
        if(*boolsettings["o"])
        {
            std::cout<<"Executing boolean expression in if: "<<booleanExpression<<'\n';
        }
        try{
            boolExpValue = callOnFrame(frame,booleanExpression);
        }
        catch(std::string &exception)
        {
            *boolsettings["t"] = printTime;
            throw std::string(exception + "\nIn if boolean exp: " + booleanExpression);
        }

        if(*boolsettings["o"])
        {
            std::cout<<"Boolean expression evaluation complete result: "<<(boolExpValue?"true":"false")<<'\n';
        }
        codeBlock = getExpr(frame->exp,startOfCodeBlock);
        //std::cout<<"CodeBlock in if: "<<codeBlock.data<<std::endl;
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
                *boolsettings["t"] = printTime;
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
        *boolsettings["t"] = printTime;
        frame->exp = expBkp;
        frame->index = 0;
        return "a"+frame->exp.substr(index-2,frame->exp.size());
}
template <class t>
t callOnFrame(AscalFrame<t>* callingFrame,std::string subExp)
{
    ParamFrame<t> executionFrame(callingFrame->getParams(),callingFrame->getParamMemory(),callingFrame->getLocalMemory());
    executionFrame.exp = subExp;
    executionFrame.setIsDynamicAllocation(false);
    t data = calculateExpression<t>(&executionFrame);

    return data;
}
template <class t>
t callOnFrameFormatted(AscalFrame<t>* callingFrame,std::string subExp)
{
    ParamFrame<t> executionFrame(callingFrame->getParams(),callingFrame->getParamMemory(),callingFrame->getLocalMemory());
    executionFrame.exp = subExp;
    executionFrame.setIsDynamicAllocation(false);
    t data = calcWithOptions(&executionFrame);

    return data;
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
    bool printTime = *boolsettings["t"];
    //*boolsettings["p"] = false;
    if(!*boolsettings["p"])
    {
        *boolsettings["t"] = false;
    }
    //std::cout<<"bool expression Being Exec: "<<booleanExpression<<std::endl;
    try{
        if(*boolsettings["o"])
        {
            std::cout<<"Executing While Boolean Expression: "<<booleanExpression<<"\n";
        }
        boolExpValue = callOnFrame(frame,booleanExpression);
    }
    catch(std::string &exception)
    {
        *boolsettings["t"] = printTime;
        throw std::string(exception +"\nIn While Boolean Expression");
    }
    if(*boolsettings["o"])
    {
        std::cout<<"Execution Complete. "<<(boolExpValue?"true":"false")<<"\n\n";
    }
    codeBlock = getExpr(expbkp,index);
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
                *boolsettings["t"] = printTime;
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
            *boolsettings["t"] = printTime;
            throw std::string(exception  + "\nIn While Boolean Expression");
        }
            //std::cout<<" value: "<<boolExpValue<<std::endl;
        }
    }
    *boolsettings["t"] = printTime;
    index = codeBlock.end + startOfCodeBlock-2;
    frame->index = 0;
    while(frame->exp[index] == ';' || frame->exp[index] == ' ' || frame->exp[index] == '}')
        index++;
    //std::cout<<"newExpIndeces length: "<<expr.size()<<" new start: "<<index<<" New Exp After While: "<<expr.substr(index,expr.size())<<std::endl;
    return "a"+expbkp.substr(index-2,frame->exp.size());
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

    std::string expbkp = frame->exp;
    frame->index = 0;
    const int startIndex = frame->exp.find("when",frame->index)<1000?frame->exp.find("when",frame->index):0;
    const int endIndex = frame->exp.find("end",frame->index)<1000?frame->exp.find("end",frame->index):0;
    //std::string startOfExp = expr.substr(0,startIndex);
    std::string endOfExp = frame->exp.substr(endIndex+3,frame->exp.length());
    std::string value;
    std::string extractedBranch;
    int index = startIndex + 4;
    std::vector<char> exp;
    //should always start after when is finished to build boolean expression

    int thenIndex;
    int whenIndex = startIndex;
    double boolExpValue;
    int elseIndex = frame->exp.find("else",frame->index);
    elseIndex = elseIndex==-1?1000000:elseIndex;
    int lastThen = 0;
    //std::cout<<"start: "<<startOfExp<<" End: "<<endOfExp<<"\n";
    do {
        thenIndex = frame->exp.find("then",index);
        //std::cout<<"Starting of exp: "<<index<<":"<<expr[index]<<" thenIndex: "<<thenIndex<<":"<<expr[thenIndex]
        //        <<" endIndex: "<<endIndex<<":"<<expr[endIndex]<<"\n";
        while(frame->exp[index] == ' ' && index < endIndex && index < thenIndex)
            index++;
        while(frame->exp[index] && index < endIndex && index < thenIndex)
        {
            exp.push_back(frame->exp[index]);
            index++;
        }
        //std::cout<<"\n";
        std::string booleanExpression(exp.begin(),exp.end());
        if(booleanExpression.size() == 0)
        {
            throw std::string("Error no boolean expression provided in when.\n");
        }
        exp.clear();
        //bool showOp = *boolsettings["o"];
        //*boolsettings["o"] = false;

        if(*boolsettings["o"])
        {
            std::cout<<"Executing Boolean Expression:\n";
        }
        try{
            boolExpValue = callOnFrame(frame,booleanExpression);
        }
        catch(std::string &exception)
        {
            throw std::string("In When Action Calculating BooleanExpression: "+booleanExpression+"\n"+exception);
        }
        //*boolsettings["o"] = showOp;
    //false case simply set the index to the next instance of when+4
    //and repeat until true, or at end of case when
        if(boolExpValue == 0)
        {
            index = frame->exp.find("when",index) + 5;
            whenIndex = index - 5;
            if(whenIndex == -1 && elseIndex != 1000000)
            {
                value = getExpr(frame->exp.substr(elseIndex+4,endIndex-(elseIndex+4)),0).data;
                extractedBranch = value;
                index = endIndex;
                value = //startOfExp+
                        value.substr(0,value.length()-1)+endOfExp;
                //std::cout<<"In Else Case rtn val: "<<value<<"\n";
            }

        }
    //true case get sub expression associated with this when return value exit loop
        else
        {
            index += 5;
            thenIndex = frame->exp.find("when",index);
            thenIndex = thenIndex==-1?endIndex+1:thenIndex;

            value = getExpr(frame->exp.substr(index,std::min(std::min(endIndex,thenIndex),elseIndex)-index),0).data;
            extractedBranch = value;

            value = //startOfExp+
            value.substr(0,value.length()-1)+endOfExp;
        }
        lastThen = thenIndex;
    } while(frame->exp[index] && boolExpValue == 0 && index < endIndex);
    if(*boolsettings["o"])
    {
        std::cout<<"Executing Branch: "<<extractedBranch<<" Params: "<<printMemory(*frame->getParamMemory()," = ",false,"|");
        std::cout<<"\n";
    }
    frame->index = 0;
    return "a"+value;
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
void printVar(const std::string &expr,bool saveLast)
{
    std::cout<<getVarName(expr,10).data<<" = "<<memory[getVarName(expr,10).data].instructionsToFormattedString()<<"\n";
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
                printAllUDF();
            }
            else if(cmpstr(frame->exp.substr(start,3),"sdf"))
            {
                printAllSDF();
            }
            else if(cmpstr(frame->exp.substr(start,3),"var"))
            {
                printVar(frame->exp,saveLast);
            }
            else
            {
                bool print = *boolsettings["p"];
                printCalculation(frame,saveLast);
                *boolsettings["p"] = print;
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
        std::cout<<set.getName()<<" Status: "<<data<<"\n";
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

        bool print = *boolsettings["p"];

        if(saveLast && !cmpstr(firstWord.data,"u") && !cmpstr(firstWord.data,"r"))
            lastExp.push(frame->exp);

        value = calcWithOptions(frame);
        if(firstWord.data.size() != 1 && firstWord.data[0] != 'p')
        *boolsettings["p"] = print;

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
    double result = calculateExpression<double>(frame);
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
                        /*<<"Values used from memo tables: "<<rememberedFromMemoTableCount<<'\n'
            <<"Currently active allocations: "<<allocatedCount-freed<<" Total Allocated: "<<allocatedCount<<" Total freed: "<<freed<<'\n'
            <<"Memory used this call: "<<allocated-lastRunAllocatedTotal<<'\n'*/
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
            get_line(std::cin, line);
            index = 0;
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
    int index = data.find("let")<500 || data.find("const")<500?data.find("t ")+1:0;
    return getVarName(data,index);
}

SubStr getVarName(const std::string &s,int index)
{
    int begin = index;
    int count = 0;
    while(s[index] && !isalpha(s[index]))
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

#include "AscalFrame.hpp"
template <typename t>
void createFrame(linkedStack<AscalFrame<t>* > &executionStack, AscalFrame<t>* currentFrame, AscalParameters &params,std::string &exp,std::string &varName, int i,uint64_t hash);

static const std::hash<std::string> hashfn;
uint64_t hashFunctionCall(std::string &exp)
{
    return hashfn(exp);
}
uint64_t hashFunctionCall(uint64_t hash,AscalParameters& params)
{
    for(std::string &s:params)
    {
        hash ^= hash<<5;
        hash ^= hash<<12;
        hash ^= hash<<18;
        hash ^= hash<<21;
        hash ^= hash<<26;
        hash ^= hash<<29;
        hash += hashfn(s);
    }
    return hash;
}
uint64_t hash(AscalFrame<double>* currentFrame)
{
    uint64_t frameptr = (uint64_t) currentFrame;
    uint64_t rtnptr = (uint64_t) currentFrame->returnPointer;
    uint64_t paramsptr = (uint64_t) currentFrame->getParamMemory();
    uint64_t locvarptr = (uint64_t) currentFrame->getLocalMemory();
    uint64_t hash = ((frameptr<<(1 + (15&rtnptr)))^currentFrame->memoPointer^paramsptr^locvarptr);
    hash |= paramsptr;
    hash |= paramsptr<<16;
    hash ^= rtnptr<<6;
    hash ^= rtnptr<<26;
    hash |= currentFrame->memoPointer<<3;
    hash ^= currentFrame->memoPointer<<20;
    hash ^= hash<<5;
    hash ^= hash<<10;
    hash ^= hash<<15;
    hash ^= hash<<20;
    hash ^= hash<<28;
    hash += frameptr<<8;
    hash += currentFrame->memoPointer;
    return hash;
}
void clearStackOnError(bool printStack, linkedStack<AscalFrame<double>* > &executionStack, AscalFrame<double>* currentFrame, AscalFrame<double>* frame, AscalFrame<double>* rtnFrame);

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
                    if(std::to_string(data).length() != MAX.length()){
                        std::cout<<"retrieving return value from memo tables\n";
                        std::cout<<"Returning value: "<<data<<'\n';
                    }
                }
            }
            }
            if(*boolsettings["d"])
               {
                std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
                std::time_t end_time = std::chrono::system_clock::to_time_t(end);
                   std::cout<<"Beginning execution of frame id: ";
                   printf("%p\n",(void*) hash(currentFrame));
                   std::cout<<std::ctime(&end_time);
               }

        }
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
         else if(isalpha(currentChar) && !isOperator(currentChar))
         {
             //This needs to be updated, and simplified it makes conditional jumps very expensive
             SubStr varName(getVarName(currentFrame->exp,i));

             //Keyword handling only one keyword at the beginning of each statement allowed,
             //including statements defined in variables
             if(inputMapper.count(varName.data) != 0)
             {

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
                     //std::cout<<"Expression after statement altering keyword: "<<exp<<std::endl;
                     //std::cout<<"expression returned from when action: "<<exp<<std::endl;

                     i = -1;
                     //currentChar = exp[i];
                     currentFrame->index = 0;
                     continue;
                 }
                 else
                 {
                     currentFrame->exp = currentFrame->exp.substr(i,currentFrame->exp.length());
                 }
                 //std::cout<<"New Expression after inputMapper: "<<exp<<std::endl;
                 i = -1;
                 currentFrame->index = i;
             }
             else
             {
                 Object data = memory.count(varName.data)!=0?memory[varName.data]:Object();
                 Object localData = currentFrame->getLocalMemory()->count(varName.data)!=0?(*currentFrame->getLocalMemory())[varName.data]:Object();

                 //Variable handling section
                 if(localData.id.length() != 0)
                 {
                     std::string params = currentFrame->exp[varName.end+1] == '('?currentFrame->exp.substr(varName.end+1):"";
                     int endOfParams = localData.setParams(params);
                     int startOfEnd = localData.params.size()==0?varName.end+1:varName.start+varName.data.length()-1+endOfParams;


                     i = endOfParams;
                     createFrame(executionStack, currentFrame,
                            localData.params,localData.getInstructions(),localData.id,startOfEnd,hashFunctionCall(localData.id));
                     goto new_frame_execution;
                 }
                 else if(data.id.length() != 0)
                 {
                              //*boolsettings["p"]  = false;
                     std::string params = currentFrame->exp[varName.end+1] =='('?
                     currentFrame->exp.substr(varName.end+1):"";
                     int endOfParams = data.setParams(params);
                     int startOfEnd = data.params.size()==0?varName.end+1:varName.start+varName.data.length()-1+endOfParams;

                     //creates a new set of stack frames, one for the function/var and one for resolution,
                     //and calculation of each of it's parameters
                     //also returns function/var's value to this function's initialOperands stack
                     createFrame(executionStack, currentFrame,
                                    data.params,data.getInstructions(),data.id,startOfEnd,hashFunctionCall(data.id));
                     goto new_frame_execution;

                  }
                 else if(currentFrame->getParamMemory()->count(varName.data))
                 {
                     Object paramData =
                             currentFrame->getParamMemory()->count(varName.data)?(*currentFrame->getParamMemory())[varName.data]
                                                                                                            :Object();
                     std::string params = currentFrame->exp[varName.end+1] == '('
                     ?currentFrame->exp.substr(varName.end+1)
                     :"";
                     int endOfParams = paramData.setParams(params);
                     int startOfEnd = data.params.size()==0?varName.end+1:varName.start+varName.data.length()-1+endOfParams;

                     //creates a new set of stack frames, one for the function/var and one for resolution,
                     //and calculation of each of it's parameters
                     //also returns function/var's value to this function's initialOperands stack

                     createFrame(executionStack, currentFrame,
                                 paramData.params,paramData.getInstructions(),varName.data,startOfEnd,hashFunctionCall(paramData.id));
                                     goto new_frame_execution;
                 }
                 else if(currentFrame->getParams()->getUseCount() < currentFrame->getParams()->size())
                 {
                     Object localVar(varName.data,(*currentFrame->getParams())[currentFrame->getParams()->size() - 1 - currentFrame->getParams()->getUseCount()],"");
                                      ++(*currentFrame->getParams());
                     (*currentFrame->getParamMemory())[localVar.id] = localVar;
                     std::string params = currentFrame->exp[varName.end+1] == '('
                     ?currentFrame->exp.substr(varName.end+1)
                     :"";
                     int endOfParams = localVar.setParams(params);
                     int startOfEnd = localVar.params.size()==0?varName.end+1:varName.start+varName.data.length()-1+endOfParams;

                     createFrame(executionStack, currentFrame,
                            localVar.params,localVar.getInstructions(),localVar.id,startOfEnd,hashFunctionCall(localVar.id));
                     goto new_frame_execution;
                 }
                 else
                 {
                    #if THROWERRORS == 1
                     if(cmpstr(varName.data,"inf"))
                         throw std::string("Arithmetic Overflow in column: "+to_string(i));
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
              if(isNumeric(currentFrame->exp[i+1]) || currentFrame->exp[i+1] == '(')
              {
                  currentFrame->initialOperators.push('*');
              }
            }
            //Section to parse numeric values from expression as a string to be inserted into
            //initialOperands stack
            else if(isNumeric(currentChar) ||
            ((currentChar == '-' || currentChar == '.')&& (i == 0 || isNonParentheticalOperator(currentFrame->exp[i-1]) || currentFrame->exp[i-1] =='(')
            ))
            {
                //This comment block must be enabled to use Integer instead of long
                //as the number's datatype
                double nextDouble = getNextDouble(currentFrame->exp,i);
                currentFrame->initialOperands.push(nextDouble);
            }
            else if(isOperator(currentChar))
            {
                currentFrame->initialOperators.push(currentChar);
            }
            //This is to support multiplication when expressions look like 2(4)
            else if(currentChar == '(' && i > 0 && isNumeric(currentFrame->exp[i-1]))

            {
                currentFrame->initialOperators.push('*');
                currentFrame->initialOperators.push(currentChar);
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
        if(currentFrame->isFunction() && *boolsettings["d"])
        {
            std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
            std::time_t end_time = std::chrono::system_clock::to_time_t(end);
               std::cout<<"Finished execution of frame id: ";
               printf("%p\n",(void*) hash(currentFrame));
               std::cout<<std::ctime(&end_time);
        }
        if(currentFrame->getReturnPointer())
        {
            if(currentFrame->isFunction())
            {
                if(*boolsettings["memoize"])
                {
                    memoPad[currentFrame->memoPointer] = data;
                }
                if(*boolsettings["o"] && std::to_string(data).length() != MAX.length()){
                    //include unix time finished
                    std::cout<<"Returning value: "<<data<<'\n';
                }
            }
            currentFrame->returnResult(data, memory);
        }
        executionStack.pop();
        if(currentFrame->isDynamicAllocation())
        {
            delete currentFrame;
        }
        else
        {
            frame->returnPointer = rtnFrame;
        }
        currentFrame = nullptr;
    }

    }catch(std::string &error)
    {
        clearStackOnError(frame->isDynamicAllocation(), executionStack, currentFrame, frame, rtnFrame);
        throw error;
    }
    catch(int exitCode)
    {
        clearStackOnError(false, executionStack, currentFrame, frame, rtnFrame);
        throw exitCode;
    }
    return data;
}
void clearStackOnError(bool printStack, linkedStack<AscalFrame<double>* > &executionStack, AscalFrame<double>* currentFrame, AscalFrame<double>* frame, AscalFrame<double>* rtnFrame)
{
    frame->returnPointer = rtnFrame;
            if(printStack)
            {
                std::cerr<<"\nFunction call stack trace:\n";
                while(!executionStack.isEmpty())
                {
                    executionStack.top(currentFrame);
                    std::cerr<<"   ~:"<<currentFrame->exp<<'\n';
                    if(currentFrame->isDynamicAllocation())
                        delete currentFrame;
                    executionStack.pop();
                }
                std::cerr<<"End of function call stack trace.\n";
            }
            else
            {
                while(!executionStack.isEmpty())
                {
                    executionStack.top(currentFrame);
                    if(currentFrame->isDynamicAllocation())
                        delete currentFrame;
                    executionStack.pop();
                }
            }
}
bool isDouble(std::string &exp)
{
    bool isADouble = true;
    char periodCount = 0;
    for(int i = exp[0] == '-'; isADouble && exp[i] && i < exp.length(); i++)
    {
        //to avoid branching I'm doing boolean arithmetic to determine if a string is a double
        periodCount += (exp[i] == '.');
        isADouble = (isNumeric(exp[i]) || (periodCount <= 1 && exp[i] == '.') ||
                     (exp[i] == ';' && exp[i+1] == 0));
    }
    return isADouble;
}
template <typename t>
void createFrame(linkedStack<AscalFrame<t>* > &executionStack, AscalFrame<t>* currentFrame, AscalParameters &params,std::string &exp,std::string &varName, int i, uint64_t hash)
{
    //save index
    currentFrame->index = i;
    if(isDouble(exp))
    {
        int i = 0;
        currentFrame->initialOperands.push(getNextDoubleS(exp, i));
        varCount++;
    }
    else
    {
        frameCount++;
        //create and set new frame expression
        FunctionFrame<t> *newFrame = new FunctionFrame<t>(nullptr,nullptr,nullptr);
        //newFrame->stackIndex = currentFrame->stackIndex+1;
        newFrame->exp = exp;
        if(memory.count(varName) == 0)
            (*newFrame->getLocalMemory())[varName] = Object(varName,exp,"");
        newFrame->memoPointer = hash;
        //Create new frame, and set return pointer
        newFrame->returnPointer = currentFrame;
        //push new frame
        executionStack.push(newFrame);
        for(int i = 0; i < params.size(); i++)
        {
            if(params[i][0] != '&')
            {
                //create and set new frame expression
                ParamFrame<t>* pFrame = new ParamFrame<t>(currentFrame->getParams(),currentFrame->getParamMemory(),currentFrame->getLocalMemory());
                pFrame->exp = params[i];
                //pFrame->stackIndex = currentFrame->stackIndex+2+i;
                //Create new frame, and set return pointer
                pFrame->returnPointer = newFrame;
                //push new frame
                executionStack.push(pFrame);
            }
            else
            {
                ParamFrameFunctionPointer<t>* pFrame = new ParamFrameFunctionPointer<t>(currentFrame->getParams(),currentFrame->getParamMemory(),currentFrame->getLocalMemory());
                pFrame->functionName = params[i].substr(1);
                //pFrame->stackIndex = currentFrame->stackIndex+2+i;
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
t add(t &and1,t &and2){    return and1+and2;}
template <class t>
t subtract(t &and1,t &and2){return and1-and2;}
template <class t>
t multiply(t &and1,t &and2){return and1*and2;}
template <class t>
t divide(t &and1,t &and2){return and2!=0?and1/and2:throw std::string("Error division by zero.");}
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
t log(t &and1,t &and2){return log(and2)/log(and1);}
template <class t>
t rootOp(t &and1,t &and2){return root(and2,and1);}
template <class t>
t equals(t &and1,t &and2){return and1==and2;}
template <class t>
t lessThan(t &and1,t &and2){return and1<and2;}
template <class t>
t greaterThan(t &and1,t &and2){    return and1>and2;}

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
        result = operations<t>[op](and1,and2);

    /*switch (op) {
    case '+':
        result = add<t>(and1,and2);
        break;
    case '-':
            result = subtract<t>(and1,and2);
            break;
    case '*':
            result = multiply<t>(and1,and2);
            break;
    case '/':
            result = divide<t>(and1,and2);
            break;
    case '%':
            result = doubleModulus<t>(and1,and2);
            break;
    case '^':
            result = exponentiate<t>(and1,and2);
            break;
    case 'P':
            result = permute<t>(and1,and2);
            break;
    case 'C':
            result = combinations<t>(and1,and2);
            break;
    case '@':
            result = log<t>(and1,and2);
            break;
    case '$':
            result = rootOp<t>(and1,and2);
            break;

    case '=':
            result = equals<t>(and1,and2);
            break;

    case '>':
            result = lessThan<t>(and1,and2);
            break;

    case '<':
            result = greaterThan<t>(and1,and2);
            break;

    }*/
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
