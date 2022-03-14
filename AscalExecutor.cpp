/*
 * AscalExecutor.cpp
 *
 *  Created on: Jan 11, 2021
 *      Author: andrewrubinstein
 */

#include "AscalExecutor.hpp"
#include "AscalFrame.hpp"
#include "Keyword.hpp"


static uint64_t hashfnv(const char *cptr, size_t len)
{
    static const uint64_t prime = 0x100000001b3, offset = 0xcbf29ce484222325;
    size_t hash = offset;
    for(uint32_t i = 0; i < len; i++)
    {
        hash ^= (cptr[i]) | ((unsigned int)cptr[i]<<8L);
        hash *= prime;
    }
    return hash;
}
template <typename string_type>
static uint64_t hashFunctionCall(string_type &exp)
{
    return hashfnv(exp.c_str(), exp.size());
}
static uint64_t hashFunctionCall(uint64_t hash,AscalParameters& params)
{
    static const uint64_t prime = 0x100000001b3;
    for(auto *s:params)
    {
        hash ^= hashfnv(s->getInstructions().c_str(), s->getInstructions().size());
        hash *= prime;
    }
    return hash;
}

template <class t>
static void printStack(stack<t> &operands)
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

void AscalExecutor::addKeyWord(Keyword *key)
{
    inputMapper[key->getName()] = key;
}
void AscalExecutor::updateBoolSetting(AscalFrame<double>* frame)
    {
        if(!lastExp.isEmpty())
            lastExp.pop();
        SubStr id = ParsingUtil::getVarName(frame->exp,frame->index);
        int endOfId = id.start+id.data.length();
        while(frame->exp[endOfId] == ' ')
            endOfId++;
        SubStr expression = ParsingUtil::getVarNameOrNum<string_view, string_view>(frame->exp,endOfId);
        short value = expression.data.length()>0?callOnFrame(frame, expression.data):-1;
        setting<bool> set = boolsettings[id.data];
        //inverts setting value via operator overloads of = and *
        bool data = value<0?!*set:value;
        setting<bool> newSetting(set.getName(),set.getCommand(),data);
        boolsettings[set.getCommand()] = newSetting;
        if(*boolsettings["o"])
        {
            std::cout<<set.getName()<<" Status changed to: "<<(data?"On":"Off")<<"\n";
        }
    }
AscalExecutor::~AscalExecutor() {
    for(auto &[key,value]:inputMapper)
    {
        delete value;
    }
}
void AscalExecutor::loadFn(Object function)
{
        memory.insert(function);
        systemDefinedFunctions.push_back(function);
}

Object& AscalExecutor::loadUserDefinedFn(Object &function, MemoryMap &mem)
{
    return mem.insert(function);

}
Object& AscalExecutor::loadUserDefinedFn(Object &function, std::map<string_view, Object*> &mem)
{
    mem[function.id] = &function;
    return function;

}
void AscalExecutor::loadInitialFunctions()
{

    //Unary boolean Operations
    loadFn(Object(memMan, "not","value=0",""));
    loadFn(Object(memMan, "true","value=0=0",""));
    loadFn(Object(memMan, "True","1",""));
    loadFn(Object(memMan, "False","0",""));

    //Calculus Functions
    loadFn(Object(memMan, "fprime","x*0+(f(x+0.000001)-f(x))/0.000001",""));

    //Trig Functions
    loadFn(Object(memMan, "sinDeg","sin(toRad(theta))",""));
    loadFn(Object(memMan, "csc","1/sin(theta)",""));
    loadFn(Object(memMan, "cosDeg","cos(toRad(theta))",""));
    loadFn(Object(memMan, "sec","1/cos(theta)",""));
    //Helpful functions
    loadFn(Object(memMan, "fib","(x){loc counter = 0;loc first = 0;loc second = 1;loc int = 0;while counter<x{set int = second;set second = second+first;set first = int;set counter = counter+1;};first}",""));
    loadFn(Object(memMan, "fibr","(x){loc fr = (x){when x > 1 then fr(x-1)+fr(x-2) else x end;};loc z = 0;memoize 1;set z = fr(x);memoize 0;z}",""));
    loadFn(Object(memMan, "rfibr","(x){when x > 1 then rfibr(x-1)+rfibr(x-2) else x end;}",""));
    loadFn(Object(memMan, "ack","when m=0 + n*0  then n+1 when n=0 then ack(m-1,1) when  m+n > 0 then ack(m-1,ack(m,n-1)) else 0 end",""));
    loadFn(Object(memMan, "fastAck","(m,n){loc z = 0;memoize 1;set z = ack(m,n);memoize 0;z;}",""));
    loadFn(Object(memMan, "gcd","(a,b){when b=0 then a when a=0=0 then gcd(b,a%b) else b end}",""));
    loadFn(Object(memMan, "sumBetween","(numberzxa,numberzxb){"
            "when (numberzxb<numberzxa)+(numberzxb=numberzxa) then sumOneTo(numberzxa)-sumOneTo(numberzxb-1)"
            "else sumOneTo(numberzxb)-sumOneTo(numberzxa-1) end}"
            ,""));
    loadFn(Object(memMan, "sumOneTo","(numberzxa*(numberzxa+1))/2",""));
    //factorial of >= 171 overflows double datatype
    loadFn(Object(memMan, "rfact",
            "when (i>1)*(i<171) then rfact(i-1)*i when not(i>1) then 1 else 0 end",""));
    loadFn(Object(memMan, "abs","when numberx<0 then numberx*-1 else numberx end",""));
    loadFn(Object(memMan, "ln","when argument>0 then e@argument else null end",""));
    loadFn(Object(memMan, "log","when argument>0 then 10@argument else null end",""));
    loadFn(Object(memMan, "logbx","base@argument",""));
    loadFn(Object(memMan, "sqrt","radicand^0.5",""));
    loadFn(Object(memMan, "fact","when numberzxa<171 then numberzxaPnumberzxa when not(numberzxa<171) then 0 end",""));
    loadFn(Object(memMan, "dist","sqrt((dx)^2+(dy)^2)",""));
    loadFn(Object(memMan, "dist3d","sqrt((dx)^2+(dy)^2+(dz)^2)",""));
    loadFn(Object(memMan, "toDeg","rad*180/pi",""));
    loadFn(Object(memMan, "toRad","deg*pi/180",""));
    loadFn(Object(memMan, "println","(x){loc counter = 0;while counter<x{set counter = counter +1;print \"endl\";};null",""));
    loadFn(Object(memMan, "clear","println(150)",""));
    loadFn(Object(memMan, "floor","x-x%1",""));
    loadFn(Object(memMan, "ceiling","when x%1=0 then x else x+1-x%1 end",""));
    loadFn(Object(memMan, "round","when x%1<0.5 then floor(x) else ceiling(x) end",""));

    //Stats Functions
    Object binProbDist(memMan, "binprob","(total C events) * probabilityOfSuccess^events * (1-probabilityOfSuccess)^(total-events)","");
    loadFn(binProbDist);

    //Dietitian Functions
    Object eKCal(memMan, "ekcal","(0*male*kg*cm*age*activity+ true(male)*5+not(male)*-161+(10*kg)+(6.25*cm)-(5*age))*activity","");
    loadFn(eKCal);
;
loadFn(Object(memMan, "println","(x){loc counter = 0;while counter<x{set counter = counter +1;print \"endl\";};null",""));
loadFn(Object(memMan, "clear","println(150)",""));
    Object isneg(memMan, "isNeg","{ "
"if x<0{ "
"print \"The number is negative (-1*x)endl\";"
"};"
//"else if(x>0)"
//"{"
//"print \"The number is positive (x)endl\";"
//"};"
"else {"
            "print \"hehheh0endl\";"
"};"
"}}","");
    isneg.compileInstructions();
    loadFn(isneg);

    //Constants Definition
    Object pi(memMan, "pi","         ","");
    //std::cout<<"Pi and e are broked fix them when you get a chance i ascalexecutor\n";
    pi.setDouble(3.141592653589793238462643383279L);
    loadFn(pi);
    Object e(memMan, "e","           ","");
    e.setDouble(2.718281828459045L);
    loadFn(e);
    loadFn(Object(memMan, "null",MAX,""));
    loadFn(Object(memMan, "printf","print \"(x)endl\"",""));

}
AscalExecutor::AscalExecutor(char** argv, int argc, int index, std::streambuf* streambuf): memory(memMan), rememberedFromMemoTableCount(0), stream_buffer_cin(streambuf), ascal_cin(streambuf)
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

double AscalExecutor::callOnFrame(AscalFrame<double>* callingFrame, const string_view subExp)
{
    ParamFrame<double> executionFrame(*this, callingFrame->getParams(),callingFrame->getParamMemory(),callingFrame->getLocalMemory());
    executionFrame.returnPointer = callingFrame->returnPointer;
    executionFrame.exp = subExp;
    executionFrame.setIsDynamicAllocation(false);
    executionFrame.setContext(callingFrame->getContext());
    const double data = this->calculateExpression(&executionFrame);
    return data;
}
double AscalExecutor::callOnFrame(AscalFrame<double>* callingFrame, const std::string &subExp)
{
    ParamFrame<double> executionFrame(*this, callingFrame->getParams(),callingFrame->getParamMemory(),callingFrame->getLocalMemory());
    executionFrame.returnPointer = callingFrame->returnPointer;
    executionFrame.exp = subExp;
    executionFrame.setIsDynamicAllocation(false);
    executionFrame.setContext(callingFrame->getContext());

    const uint32_t paramSize = callingFrame->getParams()->size();
    const double data = this->calculateExpression(&executionFrame);
    while(callingFrame->getParams()->size() > paramSize){
        callingFrame->getParams()->pop_back();
        std::cout<<"poppin params\n";
    }
    return data;
}
double AscalExecutor::callOnFrameFormatted(AscalFrame<double>* callingFrame,std::string subExp)
{
    ParamFrame<double> executionFrame(*this, callingFrame->getParams(),callingFrame->getParamMemory(),callingFrame->getLocalMemory());
    executionFrame.exp = subExp;
    executionFrame.setIsDynamicAllocation(false);
    double data = calcWithOptions(&executionFrame);
    return data;
}
void AscalExecutor::createFrame(StackSegment<AscalFrame<double>* > &executionStack, AscalFrame<double>* currentFrame, Object *obj, ParsedStatementList &params, int i,uint64_t hash)
{
    //save index
    currentFrame->index = i;
    if(obj->isList() | obj->isObjList()) {}
    else if(obj->isDouble())
    {
        currentFrame->initialOperands.push(obj->getDouble());
        varCount++;
        if(*boolsettings["o"])
            std::cout<<"QReading variable: "<<obj->getId()<<" = "<<obj->getDouble()<<'\n';
    }
    else if(ParsingUtil::isDouble(obj->getInstructions()))
    {
        int ib = 0;
        double value = ParsingUtil::getNextDouble(obj->getInstructions(), ib);
        currentFrame->initialOperands.push(value);
        obj->setDouble(value);
        varCount++;
        if(*boolsettings["o"])
            std::cout<<"Reading variable: "<<obj->getId()<<" = "<<obj->getDouble()<<'\n';
    }
    else
    {
        frameCount++;
        if(*boolsettings["o"])
        {
            std::stringstream data;
            for(SubStrSV s:params.statements)
            {
                data << s.data << ",";
            }
            std::cout<<"Parsing params then executing: "<<obj->getId()<<'('<<data.str()<<')'<<'\n';
        }
        //create and set new frame expression
        FunctionFrame<double> *newFrame = fFramePool.construct(*this, this->memMan);//new FunctionFrame<double>(*this, this->memMan);
        newFrame->exp = obj->getInstructions();
        if(memory.count(obj->id) == 0)
            this->loadUserDefinedFn(*obj, *newFrame->getParamMemory());
        newFrame->memoPointer = hash;
        //Create new frame, and set return pointer
        newFrame->returnPointer = currentFrame;
        //Context object ptr for this keyword resolution
        newFrame->setContext(obj);
        //push new frame
        executionStack.push(newFrame);
        for(int i = 0; i < params.statements.size(); i++)
        {
            uint8_t startingIndex = 0;
            while(params.statements[i].data[startingIndex++] == ' '){};
            if(params.statements[i].data[--startingIndex] != '&')
            {
                //create and set new frame expression
                ParamFrame<double>* pFrame = pFramePool.construct(*this, currentFrame);
                pFrame->exp = params.statements[i].data;
                //Create new frame, and set return pointer
                pFrame->returnPointer = newFrame;
                //Context object ptr for this keyword resolution
                pFrame->setContext(currentFrame->getContext());
                //push new frame
                executionStack.push(pFrame);
            }
            else
            {
                ParamFrameFunctionPointer<double>* pFrame = fpFramePool.construct(*this, currentFrame);
                uint32_t frameIndexBackup = currentFrame->index;
                SubStr startOfParam = ParsingUtil::getVarName(params.statements[i].data.substr(startingIndex), startingIndex);
                
                SubStrSV sopView(startOfParam);
                startOfParam.end = params.statements[i].start+startOfParam.data.size();
                pFrame->obj = resolveNextObjectExpression(currentFrame, sopView).data;
                currentFrame->index = frameIndexBackup;
                //Create new frame, and set return pointer
                pFrame->returnPointer = newFrame;
                //push new frame
                executionStack.push(pFrame);
            }
        }
    }
}
void AscalExecutor::deleteFrame(AscalFrame<double> *frame)
{
    switch (frame->getType()) {
        case 'f':
            this->fFramePool.destroy((FunctionFrame<double>*) frame);
            break;
        case 'p':
            this->pFramePool.destroy((ParamFrame<double>*) frame);
            break;
        case 'z':
            this->fpFramePool.destroy((ParamFrameFunctionPointer<double>*) frame);
            break;
        case 'c':
            this->cFramePool.destroy((ConditionalFrame<double>*) frame);
            break;
        case 's':
            this->sFramePool.destroy((FunctionSubFrame<double>*) frame);
            break;
        default:
            throw std::string("Error, no deallocation strategy for frame, this is a system bug, contact developer on github @ github.com/andrewrubinstein");
            break;
    }
}

void AscalExecutor::clearStackOnError(bool printStack, std::string &error, StackSegment<AscalFrame<double>* > &executionStack, AscalFrame<double>* currentFrame, AscalFrame<double>* frame)
{
     frame->returnPointer = cachedRtnObject;
            if(printStack)
            {
                std::stringstream data;
                size_t errorStartMarker = error.find("\1\1\2\2\3\3\4\4\1\1");
                if(errorStartMarker<0 || errorStartMarker > error.length())
                    errorStartMarker = 0;
                data<<error.substr(0, errorStartMarker);
                while(!executionStack.isEmpty())
                {
                    executionStack.top(currentFrame);
                        data << currentFrame->getType() << "   ~:"<<currentFrame->exp<<'\n';
                    if(currentFrame->isDynamicAllocation()){
                        deleteFrame(currentFrame);
                    }
                    executionStack.pop();
                }
                data << "\1\1\2\2\3\3\4\4\1\1";
                data << error.substr(errorStartMarker);
                error = data.str();
            }
            else
            {
                while(!executionStack.isEmpty())
                {
                    executionStack.top(currentFrame);
                    if(currentFrame->isDynamicAllocation()){
                        deleteFrame(currentFrame);
                    }
                    executionStack.pop();
                }
            }
            this->cachedRtnObject = nullptr;
}
stack<AscalFrame<double>*> frameStack;
double AscalExecutor::calculateExpression(AscalFrame<double>* frame)
{
    //variable that is always a copy
    char peeker;
    //variables to hold the operands
    double and1,and2;
    //c is a variable to store each of the characters in the expression
    //in the for loop
    char currentChar;

    double data = 0;
    AscalFrame<double>* rtnptr = frame->returnPointer;
    frame->returnPointer = nullptr;
    //This loop handles parsing the numbers, and adding the data from the expression
    //to the stacks
    StackSegment<AscalFrame<double>* > executionStack(frameStack);
    currentStack = &executionStack;
    AscalFrame<double>* currentFrame = frame;
    executionStack.push(currentFrame);
    try{
        check_stack:
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
                    executionStack.pop();//must happen before return for conditional frames
                    currentFrame->returnResult(data, memory, executionStack, *this);
                    ++rememberedFromMemoTableCount;

                    if(*boolsettings["o"])
                    {
                        if(std::to_string(data).length() != MAX.length())
                        {
                            std::cout<<"retrieving return value from memo tables\n";
                            std::cout<<"Returning value: "<<data<<'\n';
                        }
                    }
                    if(currentFrame->isDynamicAllocation())
                        deleteFrame(currentFrame);
                    goto check_stack;
                }
            }

        }
        else if(currentFrame->isReturnFlagSet())
        {
            if(currentFrame->isReturnObjectFlagSet())
            {
                Object *returnedObj = currentFrame->getReturnObject();
                //accessing field in returned object
                if(currentFrame->exp[currentFrame->index] == '.' || currentFrame->exp[currentFrame->index] == '[')
                {
                    SubStr ptr("",0,currentFrame->index-1);
                    SubStrSV svPtr(ptr);
                    bool adjust = currentFrame->exp[currentFrame->index] == '.' || currentFrame->exp[currentFrame->index+1] == '&';
                    Object *objectResolved = resolveNextObjectExpression(currentFrame, svPtr, returnedObj).data;
                    if(!returnedObj)
                        throw std::string("could not resolve field in returned object: "+returnedObj->getId().str());
                    else
                        returnedObj = objectResolved;
                    currentFrame->index -= adjust;
                }

                uint32_t startOfparams = currentFrame->exp[currentFrame->index] =='('?
                    currentFrame->index:currentFrame->exp.size();
                ParsingUtil::ParseStatementList(currentFrame->exp, startOfparams, this->paramsBuffer);
                uint32_t startOfEnd = this->paramsBuffer.statements.size()==0?currentFrame->index:currentFrame->index+this->paramsBuffer.end-1;
                //creates a new set of stack frames, one for the returned function/var and one for resolution,
                //and calculation of each of it's parameters
                //also returns function/var's value to this function's initialOperands stack
                createFrame(executionStack, currentFrame,
                            returnedObj,this->paramsBuffer,startOfEnd,hashFunctionCall(returnedObj->id));
                   goto new_frame_execution;

            }
            else
            {
                currentFrame->setReturnFlag(false);
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
        for(int i = currentFrame->index;i < currentFrame->exp.length();i++)
        {
         currentFrame->index = i;
         currentChar = currentFrame->exp[i];
         currentFrame->initialOperators.top(peeker);
         //anything used outside of the first { gets dropped from program stack, for param definition
         currentFrame->level += (currentChar == '{') - (currentChar == '}');
         //std::cout<<currentChar<<"\n";

         if(currentChar == '{' && currentFrame->level == 1)
         {
            while(!currentFrame->initialOperands.isEmpty())
                currentFrame->initialOperands.pop();
            while(!currentFrame->initialOperators.isEmpty())
                currentFrame->initialOperators.pop();
         }
         else if(ParsingUtil::isalpha(currentChar) && (ParsingUtil::isalpha(frame->exp[i+1]) || !Calculator<double>::isOperator(currentChar)))
         {
             //This needs to be updated, and simplified it makes conditional jumps very expensive
             uint32_t ind = i;
             SubStrSV varName = ParsingUtil::getVarNameSV(currentFrame->exp,ind);

             if(inputMapper.count(varName.data) != 0)
             {
                 Keyword* executingKeyword;
                 const int currentFrameCount = executionStack.length();
                 bool isDynamicBackup = currentFrame->isDynamicAllocation();
                 if(!currentFrame->returnPointer)
                     cachedRtnObject = rtnptr;
                 try{
                     executingKeyword = inputMapper.find(varName.data)->second;
                     executingKeyword->action(currentFrame);
                     currentFrame->setIsDynamicAllocation(isDynamicBackup);
                 }catch(std::string& s){
                     currentFrame->setIsDynamicAllocation(isDynamicBackup);
                     throw s;
                 }
                 cachedRtnObject = nullptr;

                 if(executingKeyword->getType() == 0)
                 {
                     uint8_t subLevel = 0;
                     while((subLevel != 0 && currentFrame->exp.size() < i) || (currentFrame->exp[i] && currentFrame->exp[i] != ';' && currentFrame->exp[i] != '\n'))
                     {
                         i++;
                         subLevel += (currentFrame->exp[i]=='{') - (currentFrame->exp[i] == '}');

                     }

                     if((i == currentFrame->exp.size() || 1+i == currentFrame->exp.size()))
                     {
                         if(currentFrame->initialOperands.size() != 0)
                         {
                             throw std::string("\nError called keyword: '"+varName.data.str()+"'\nwithout statement separator ';' before previous expression.");
                         }
                         i = currentFrame->exp.size();
                         currentFrame->initialOperands.push(std::stod(MAX));
                     }
                     continue;
                 }
                 else if(executingKeyword->getType() == 1 || executingKeyword->getType() == 3)
                 {
                     //for returning altered versions of the expression up till a statement separator like ;
                     //use cases: getting user input outside functions and variables
                     //when then when then end if else.
                     //while loops?
                     //just make sure first character in returned string is a throwaway alphabetical character
                     //it will not be parsed so ensure it is not meaningful code
                         i = currentFrame->index<currentFrame->exp.size()?currentFrame->index - 1:currentFrame->exp.size();
                     if(executionStack.length() != currentFrameCount)
                     {
                         goto new_frame_execution;
                     }
                          continue;

                 }
                 else
                 {
                     currentFrame->exp = currentFrame->exp.substr(i,currentFrame->exp.length());
                 }
                 i = -1;
                 currentFrame->index = i;
                 continue;
             }
             else
             {
                 uint32_t startOfparams = currentFrame->exp[varName.end+1] =='('?
                 varName.end+1:currentFrame->exp.size();
                 //Variable handling section
                 currentFrame->index = varName.start;
                 expressionResolution resolution = resolveNextObjectExpression(currentFrame, varName);
                 if(resolution.data)
                 {
                     varName.start = varName.end - varName.data.length() + 1;
                     startOfparams = currentFrame->exp[varName.end+1] =='('?
                     varName.end+1:currentFrame->exp.size();
                     uint32_t endOfParams =
                     ParsingUtil::ParseStatementList(currentFrame->exp, startOfparams, this->paramsBuffer).end;
                     uint32_t startOfEnd = varName.start+varName.data.size()-1+endOfParams;
                     //std::cout<<"soe: "<<startOfEnd<<" vnsize: "<<varName.data.size()<<"start: "<<varName.start<<" eop: "<<endOfParams<<"\nvarName: "<<varName.data<<"\n";
                 //creates a new set of stack frames, one for the function/var and one for resolution,
                 //and calculation of each of it's parameters
                 //also returns function/var's value to this function's initialOperands stack
                     createFrame(executionStack, currentFrame,
                                resolution.data, this->paramsBuffer,startOfEnd,hashFunctionCall(resolution.data->id));
                     goto new_frame_execution;
                 }
                 else if(currentFrame->getParams()->getUseCount() < currentFrame->getParams()->size())
                 {
                     
                         varName.start = varName.end - varName.data.length() + 1;
                     if(currentFrame->exp[varName.end+1] == '.' || currentFrame->exp[varName.end+1] == '[')
                     {
                         std::stringstream error;
                         error<<"Error while attempting to resolve parameter "<<varName.data<<" at position "<<ParsingUtil::to_string(varName.start)<<
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
                     if(paramVar->id[0] == 2)
                     {
                         paramVar->copyToId(varName.data.str());
                     }
                     (*currentFrame->getParamMemory())[varName.data] = (paramVar);
                     uint32_t endOfParams =
                     ParsingUtil::ParseStatementList(currentFrame->exp, startOfparams, this->paramsBuffer).end;
                     uint32_t startOfEnd = varName.start+varName.data.size()-1+endOfParams;
                     createFrame(executionStack, currentFrame,
                             paramVar, this->paramsBuffer,startOfEnd,hashFunctionCall(paramVar->id));
                     goto new_frame_execution;
                 }
                 else
                 {
                    #if THROWERRORS == 1
                     if(resolution.error)
                     {
                         if(ParsingUtil::cmpstr(varName.data.str(),std::string("inf")))
                             throw std::string("Arithmetic Overflow in column: "+ParsingUtil::to_string(i));
                         else if(ParsingUtil::cmpstr(varName.data.str(),std::string("nan")))
                             throw std::string("Error Undefined result calculated");
                         else
                             throw std::string("Invalid reference: "+varName.data.str()+" in column: "+ParsingUtil::to_string(i));
                     }
                     else
                    #endif
                     {
                         i = currentFrame->index;
                     }
                     
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
                          ParsingUtil::to_string(i+1));
              currentFrame->initialOperands.top(and2);
              currentFrame->initialOperands.pop();
              processOperands.push(and2);
              if(!currentFrame->initialOperators.isEmpty())
                    currentFrame->initialOperators.pop();
              //Send expression in parentheses to processStack for evaluation
              try{
                  and2 = processStack(processOperands,processOperators);
              }catch(std::string &error)
              {
                  throw std::string("In Expression: "+currentFrame->exp.str()+" "+error);
              }

              currentFrame->initialOperands.push(and2);
            }
            //Section to parse numeric values from expression as a string to be inserted into
            //initialOperands stack
            else if(ParsingUtil::isNumeric(currentChar) ||
            (currentChar == '-' && ParsingUtil::isNumeric(currentFrame->exp[i+1]) &&
                    ((currentFrame->initialOperands.isEmpty()) ||
                            (i > 0 && (Calculator<double>::isNonParentheticalOperator(currentFrame->exp[i-1]) || currentFrame->exp[i-1] =='('))
                            ))
                || currentChar == '.')
            {
                currentFrame->initialOperands.push(ParsingUtil::getNextDouble(currentFrame->exp,i));
            }
            else if(Calculator<double>::isOperator(currentChar))
            {
                currentFrame->initialOperators.push(currentChar);
            }
            else if(currentChar == ';')
            {
                currentFrame->clearStackIfAnotherStatementProceeds();
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
        if(currentFrame->getReturnPointer() && !currentFrame->getReturnPointer()->isReturnFlagSet())
        {
            if(currentFrame->isFunction())
            {
                if(*boolsettings["memoize"])
                {
                    memoPad[currentFrame->memoPointer] = data;
                }
                if(currentFrame->getContext() && *boolsettings["o"] && std::to_string(data).length() != MAX.length()){
                    std::cout<<"Returning value: "<<data<<" from function: "<<currentFrame->getContext()->getId()<<'\n';
                }
            }
            executionStack.pop();//must happen before return for conditional frames
            currentFrame->returnResult(data, memory, executionStack, *this);
        }
        else
            executionStack.pop();//must happen before return for conditional frames
        if(currentFrame->isDynamicAllocation())
        {
            deleteFrame(currentFrame);
        }
        cachedRtnObject = nullptr;
        currentFrame = nullptr;
    }

    }catch(std::string &error)
    {
        clearStackOnError(true, error, executionStack, currentFrame, frame);
        throw error;
    }
    catch(int exitCode)
    {
        std::string s = frame->exp.str();
        clearStackOnError(false, s, executionStack, currentFrame, frame);
        throw exitCode;
    }
    frame->returnPointer = rtnptr;
    currentStack = nullptr;
    return data;
}

template <typename t>
void AscalExecutor::cleanOnError(bool timeInstruction, t start, t end)
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
                        << "finished computation at " << std::ctime(&end_time)
                                              << "elapsed time: " << elapsed_seconds.count() << "s\n";
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

double AscalExecutor::calcWithOptions(AscalFrame<double>* frame)
{
    bool timeInstruction = *boolsettings["t"];

    std::chrono::system_clock::time_point start,end;
    if(timeInstruction){
        start = std::chrono::system_clock::now();
    }

    //alternate beginning of calculation for doubles
    //-------------------------
    double result;
    try{
        result = calculateExpression(frame);
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
            << "finished computation at " << std::ctime(&end_time)
                                  << "elapsed time: " << elapsed_seconds.count() << "s\n";

        }
        if(std::to_string(result).length() != MAX.length() && *boolsettings["p"])
        {
            if(*boolsettings["sci"])
                std::cout<<result<<std::endl;
            else
                std::cout<<ParsingUtil::to_string(result)<<std::endl;
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
double AscalExecutor::processStack(stack<double> &operands,stack<char> &operators)
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
      double result = 0, and1 = 0, and2 = 0;
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
          if(Calculator<double>::getPriority(nextOperator) > Calculator<double>::getPriority(firstOperator))
          {
              savedOperators.push(firstOperator);
              savedOperands.push(and1);
          }

      } while(Calculator<double>::getPriority(nextOperator)>Calculator<double>::getPriority(firstOperator));
      //perform found operation, and remove remaining operand from stack
      operands.pop();
      static Calculator<double> c;
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
    if(!operands.isEmpty())
        operands.pop();
      return result;
}

Object& AscalExecutor::getObject(AscalFrame<double>* frame, string_view functionName)
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
                    return (memory)[functionName];
                }
                else
                {
                    throw std::string("Error locating object "+functionName.str()+"\n");
                }
}
Object* AscalExecutor::getObjectNoError(AscalFrame<double>* frame, string_view functionName)
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
                    Object *obj = &(memory)[functionName];
                    return obj;
                }
                else
                {
                    return nullptr;
                }
}

Object* AscalExecutor::resolveNextExprSafe(AscalFrame<double>* frame, SubStrSV varName)
{
    Object* obj = AscalExecutor::resolveNextObjectExpression(frame, varName, nullptr).data;
    if(!obj)
    {
        throw std::string("Error Resolving object expression near index: "+ParsingUtil::to_string(varName.start)+" last parsed var name: "+varName.data.str());
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

expressionResolution AscalExecutor::resolveNextObjectExpression(AscalFrame<double>* frame, SubStrSV &varName, Object *obj)
{
    frame->index = varName.end+1;
    expressionResolution result;
    if(!obj)
    {
        obj = getObjectNoError(frame, varName.data);
    }
    if(frame->getContext() && !obj &&
            varName.data.size() == 4 &&
                    varName.data[0] == 't' && varName.data[1] == 'h' && varName.data[2] == 'i' && varName.data[3] == 's')
    {
        obj = frame->getContext()->getThis();
    }
    bool parsing;
    do {
        parsing = frame->exp[frame->index] == '.';
        while (obj && frame->exp.size() > frame->index && parsing)
        {
            varName = ParsingUtil::getVarNameSV(frame->exp, frame->index);
            parsing = frame->exp[varName.end+1] == '.';
            frame->index = varName.end+1 + parsing;
            result.parent = obj;
            obj = &(*obj)[varName.data];
        }
        if(result.parent)
            obj->setParent(result.parent);
        if(obj && frame->exp.size() > frame->index && frame->exp[frame->index] == '[')
        {
            //if index in array lookup
            if(ParsingUtil::isalpha(frame->exp[frame->index+1]) || (ParsingUtil::isNumeric(frame->exp[1+frame->index])))
            {
                int index = frame->index;
                SubStrSV lStr = ParsingUtil::getExprInStringSV(frame->exp, index, '[',']',';');
                varName.data = lStr.data;
                double lookup = this->callOnFrame(frame, lStr.data);
                if(obj->isObjList())
                {
                    result.parent = obj;
                    result.listIndex = lookup;
                    obj = &(*obj)[lookup];
                    frame->index = index+1;
                    varName.end = index+lStr.data.size();
                }
                else
                {
                    frame->initialOperands.push(obj->getDoubleAtIndex(lookup));
                    result.error = false;
                    result.parent = obj;
                    result.listIndex = lookup;
                    obj = nullptr;
                    frame->index = index+1;
                    varName.end = index+lStr.data.size();
                    varName.data.resize(0);
                }
            }
            //else dictionary based lookup by string
            else if(frame->exp.size() > frame->index)
            {
                //By variable name representing string to lookup
                if(frame->exp[frame->index+1] == '&')
                {
                    varName = ParsingUtil::getVarName(frame->exp, ++frame->index);
                    //get object data
                    frame->index = varName.start;
                    result.parent = obj;
                    obj = &(*obj)[AscalExecutor::resolveNextObjectExpression(frame, varName, nullptr).data->listToString(memory)];
                    varName.end++;
                }
                //By string literal
                else if(frame->exp[frame->index+1] == '\"')
                {
                    varName = ParsingUtil::getVarName(frame->exp, ++frame->index);
                    frame->index = varName.end+1;
                    while(frame->index < frame->exp.size() && frame->exp[frame->index++] == ']'){}
                    varName.end = frame->index;
                    result.parent = obj;
                    obj = &(*obj)[varName.data];
                }
                else
                {
                    throw std::string("Error invalid lookup parameters supplied to map: "+obj->getId().str());
                }
            }
        }
        parsing = (frame->exp[++frame->index] == '.' || frame->exp[frame->index] == '[');
    } while(obj && parsing);
    varName.start = varName.end;
    frame->index = varName.end;
    result.data = obj;
    return result;
}
Object* AscalExecutor::resolveNextObjectExpressionPartial(AscalFrame<double>* frame, SubStrSV &varName, Object *obj)
{
    //getvarname
    frame->index = varName.end+1;

    if(!obj && (frame->exp[varName.end+1] == '.' || frame->exp[varName.end+1] == '['))
        obj = getObjectNoError(frame, varName.data);
    if((!obj && frame->getContext() && (frame->exp[varName.end+1] == '.' || frame->exp[varName.end+1] == '[')) || (varName.data.size() == 4 && varName.data[0] == 't' && varName.data[1] == 'h' && varName.data[2] == 'i' && varName.data[3] == 's'))
        obj = frame->getContext()->getThis();
    bool parsing = frame->exp[frame->index] == '.';
    Object *possible = nullptr;
    do {
        parsing = frame->exp[frame->index] == '.';
        while (obj && frame->exp.size() > frame->index && parsing)
            {
                varName = ParsingUtil::getVarNameSV(frame->exp, frame->index);
                parsing = frame->exp[varName.end+1] == '.';
                frame->index = varName.end+1 + parsing;
                if(parsing || frame->exp[varName.end+1] == '[')
                    obj = &(*obj)[varName.data];
            }
            if(obj && frame->exp.size() > frame->index && frame->exp[frame->index] == '[')
            {
                //if index in array lookup
                if((ParsingUtil::isNumeric(frame->exp[++frame->index]) || isalpha(frame->exp[frame->index])) && frame->exp[frame->index])
                {
                    int index = frame->index-1;
                    varName = ParsingUtil::getExprInStringSV(frame->exp, index, '[',']',';');
                    varName.data = ParsingUtil::to_string(this->callOnFrame(frame, varName.data.str()));
                    frame->index = index+1;
                }
                //else dictionary based lookup by string
                else if(frame->exp.size() > frame->index)
                {
                    //By variable name representing string to lookup
                    if(frame->exp[frame->index] == '&')
                    {
                        varName = ParsingUtil::getVarNameSV(frame->exp, ++frame->index);
                        frame->index = varName.start;
                        varName.data = AscalExecutor::resolveNextObjectExpression(frame, varName, nullptr).data->listToString(memory).data();
                        frame->index = varName.end+1;
                        while(frame->index < frame->exp.size() && frame->exp[frame->index++]){}
                        possible = &obj->getMapUnsafe(varName.data);
                    }
                    //By string literal
                    else if(frame->exp[frame->index] == '\"')
                    {
                        varName = ParsingUtil::getVarNameSV(frame->exp, ++frame->index);
                        frame->index = varName.end+1;
                        while(frame->index < frame->exp.size() && frame->exp[frame->index++] == ']'){}
                        varName.end = frame->index - 1;
                        possible = &obj->getMapUnsafe(varName.data);
                    }
                    else
                    {
                        throw std::string("Error invalid lookup parameters supplied to map: "+obj->getId().str());
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

    frame->index = varName.end - varName.data.size() + 1;
    return obj;
}


