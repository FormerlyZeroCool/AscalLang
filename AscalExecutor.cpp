

/*
 * AscalExecutor.cpp
 *
 *  Created on: Jan 11, 2021
 *      Author: andrewrubinstein
 */

#include "AscalExecutor.hpp"
#include "AscalFrame.hpp"
#include "Keyword.hpp"

/*
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
}*/
/*
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
}*/

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
    inputMapper.insert(key->getName(),key);
}
void AscalExecutor::updateBoolSetting(std::string &&name)
    {
        if(!lastExp.isEmpty())
            lastExp.pop();
        setting<bool> &set = boolsettings[name];
        //inverts setting value via operator overloads of = and *
        bool data = !*set;
        set = data;
        if(*boolsettings["o"])
        {
            std::cout<<set.getName()<<" Status changed to: "<<(data?"On":"Off")<<"\n";
        }
    }
Object& AscalExecutor::loadFn(Object function)
{
    Object &inserted = loadUserDefinedFn(function, memory);
    return inserted;
}

Object& AscalExecutor::loadUserDefinedFn(Object &&function, FlatMap<string_view, Object*> &mem)
{
    return this->loadUserDefinedFn(function, mem);
}
Object& AscalExecutor::loadUserDefinedFn(Object &function, FlatMap<string_view, Object*> &mem)
{
    const auto rec = mem.find(function.getId());
    if(rec == mem.end())
    {
        Object *obj = this->memMan.constructObj(function);
        return *(*mem.insert(obj->id, obj)).getValue();
    }
    else
    {
        this->memMan.obj_free((*rec).getValue());
        (*rec).getValue() = this->memMan.constructObj(function);
        return *(*rec).getValue();
    }
}
Object& AscalExecutor::loadUserDefinedFn(Object &function, MemoryMap &mem)
{
    return mem.insert(function);

}
Object& AscalExecutor::loadUserDefinedFn(Object &function, Map<string_view, Object*> &mem)
{
    return *mem.set(function.getId(), &function).data.second;
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
    loadFn(Object(memMan, "sinDeg","sin(toRad(x))",""));
    loadFn(Object(memMan, "csc","1/sin(x)",""));
    loadFn(Object(memMan, "cosDeg","cos(toRad(x))",""));
    loadFn(Object(memMan, "sec","1/cos(x)",""));
    //Helpful functions
    loadFn(Object(memMan, "fib","(x){loc counter = 0;loc first = 0;loc second = 1;loc int = 0;while counter<x{set int = second;set second = second+first;set first = int;set counter = counter+1;};first}",""));
    loadFn(Object(memMan, "fibr","(x){loc fr = (x){when x > 1 then fr(x-1)+fr(x-2) else x end;};loc z = 0;memoize 1;set z = fr(x);memoize 0;z}",""));
    loadFn(Object(memMan, "rfibr","(x){when x > 1 then rfibr(x-1)+rfibr(x-2) else x end;}",""));
    //loadFn(Object(memMan, "ack","when m=0 then n+1 when n=0 then ack(m-1,1) when  m+n > 0 then ack(m-1,ack(m,n-1)) else 0 end",""));
    //loadFn(Object(memMan, "fastAck","(m,n){loc z = 0;memoize 1;set z = ack(m,n);memoize 0;z;}",""));
    //loadFn(Object(memMan, "gcd","(a,b){when b=0 then a when a=0=0 then gcd(b,a%b) else b end}",""));
    /*loadFn(Object(memMan, "sumBetween","(numberzxa,numberzxb){"
            "when (numberzxb<numberzxa)+(numberzxb=numberzxa) then sumOneTo(numberzxa)-sumOneTo(numberzxb-1)"
            "else sumOneTo(numberzxb)-sumOneTo(numberzxa-1) end}"
            ,""));
            */
    loadFn(Object(memMan, "sumOneTo","(x*(x+1))/2",""));
    //factorial of >= 171 overflows double datatype
    loadFn(Object(memMan, "rfact",
            "when (x>1)*(x<171) then rfact(x-1)*x when not(x>1) then 1 else 0 end",""));
    loadFn(Object(memMan, "abs","when numberx<0 then numberx*-1 else numberx end",""));
    loadFn(Object(memMan, "ln","when argument>0 then e@argument else null end",""));
    loadFn(Object(memMan, "log","when argument>0 then 10@argument else null end",""));
    //loadFn(Object(memMan, "logbx","base@argument",""));
    loadFn(Object(memMan, "sqrt","radicand^0.5",""));
    loadFn(Object(memMan, "fact","when numberzxa<171 then numberzxaPnumberzxa when not(numberzxa<171) then 0 end",""));
    //loadFn(Object(memMan, "dist","sqrt((dx)^2+(dy)^2)",""));
    //loadFn(Object(memMan, "dist3d","sqrt((dx)^2+(dy)^2+(dz)^2)",""));
    loadFn(Object(memMan, "toDeg","rad*180/pi",""));
    loadFn(Object(memMan, "toRad","deg*pi/180",""));
    loadFn(Object(memMan, "println","(x){loc counter = 0;while counter<x{set counter = counter +1;print \"endl\";};null",""));
    loadFn(Object(memMan, "clear","println(150)",""));
    loadFn(Object(memMan, "ceiling","when x%1=0 then x else x+1-x%1 end",""));
    loadFn(Object(memMan, "round","when x%1<0.5 then floor(x) else ceiling(x) end",""));

    //Stats Functions
    //Object binProbDist(memMan, "binprob","(total C events) * probabilityOfSuccess^events * (1-probabilityOfSuccess)^(total-events)","");
    //loadFn(binProbDist);

    //Dietitian Functions
    //Object eKCal(memMan, "ekcal","(0*male*kg*cm*age*activity+ true(male)*5+not(male)*-161+(10*kg)+(6.25*cm)-(5*age))*activity","");
    //loadFn(eKCal);
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
    loadFn(isneg);

    //Constants Definition
    Object pi(memMan, "pi","","");
    //std::cout<<"Pi and e are broked fix them when you get a chance i ascalexecutor\n";
    
    Object e(memMan, "e","","");
    loadFn(Object(memMan, "null",MAX,""));
    loadFn(Object(memMan, "printf","print \"(x)endl\"",""));

    loadFn(Object(memMan, "floor","x-x%1",""));
    for(auto chunk:this->memory)
    {
        chunk.getValue()->compileInstructions(*this);
        const auto op = Global::returnAndPop;
        chunk.getValue()->append(op);
        chunk.getValue()->setObject();
    }
    loadFn(e).setDouble(2.718281828459045);
    loadFn(pi).setDouble(3.141592653589793238462643383279);
}
	AscalExecutor::~AscalExecutor()
	{
		for(auto &chunk : memory)
		{
			this->memMan.obj_free(chunk.getValue());
		}
        for(auto &chunk:inputMapper)
        {
            delete chunk.getValue();
        }
	}
AscalExecutor::AscalExecutor(char** argv, int argc, int index, std::streambuf* streambuf): 
    rememberedFromMemoTableCount(0), stream_buffer_cin(streambuf), ascal_cin(streambuf)
{

    ascal_cin.rdbuf(streambuf);
    
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

        boolsettings.insert(set.getCommand(), set);
        //auto alloced = boolsettings.allocatedCount();
        //std::cout<<"capacity, alloced, fill% "<<boolsettings.getCapacity()<<", "<<alloced<<", "<<((alloced*100.0)/boolsettings.getCapacity())<<"\n";
    

        set = setting<bool> (
                /*name*/
                    "Auto print results of calculations",
                /*command line command*/
                    "p",
                /*variable*/
                    true);

        boolsettings.insert(set.getCommand(), set);
        //alloced = boolsettings.allocatedCount();
        //std::cout<<"capacity, alloced, fill% "<<boolsettings.getCapacity()<<", "<<alloced<<", "<<((alloced*100.0)/boolsettings.getCapacity())<<"\n";
    

        set = setting<bool> (
                /*name*/
                    "Use scientific notation for output of numbers larger than 999,999",
                /*command line command*/
                    "sci",
                /*variable*/
                    true);

        boolsettings[set.getCommand()] = set;
        //alloced = boolsettings.allocatedCount();
        //std::cout<<"capacity, alloced, fill% "<<boolsettings.getCapacity()<<", "<<alloced<<", "<<((alloced*100.0)/boolsettings.getCapacity())<<"\n";
    
        set = setting<bool>(
            /*name*/
                "Auto Memoize all function calls to improve multiple recursive function performance,\nwill cause erroneous calculations if not using pure mathematical functions.",
            /*command line command*/
                "memoize",
            /*variable*/
                false);
        boolsettings.insert(set.getCommand(), set);
        //alloced = boolsettings.allocatedCount();
        //std::cout<<"capacity, alloced, fill% "<<boolsettings.getCapacity()<<", "<<alloced<<", "<<((alloced*100.0)/boolsettings.getCapacity())<<"\n";
    
        set = setting<bool>(
            /*name*/
                "Print time taken to run calculation",
            /*command line command*/
                "t",
            /*variable*/
                false);
        boolsettings.insert(set.getCommand(), set);
        //alloced = boolsettings.allocatedCount();
        //std::cout<<"capacity, alloced, fill% "<<boolsettings.getCapacity()<<", "<<alloced<<", "<<((alloced*100.0)/boolsettings.getCapacity())<<"\n";
    
        }//bracket to remove set variable from program memory
          /*
           * End of initialization values in settings hashmap
           * */
}

void AscalExecutor::createFrame(StackSegment<AscalFrame<double>* > &executionStack, AscalFrame<double>* currentFrame, Object *obj, ParsedStatementList &params, int i,uint64_t hash)
{/*
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
        if(memory.find(obj->id) == memory.end())
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
    }*/
}
void AscalExecutor::deleteFrame(AscalFrame<double> *frame)
{
    this->framePool.destroy(frame);    
}

void AscalExecutor::clearStackOnError(bool printStack, std::string &error, stack<AscalFrame<double>* > &executionStack, AscalFrame<double>* currentFrame, AscalFrame<double>* frame)
{
            if(printStack)
            {
                std::stringstream data;
                size_t errorStartMarker = error.find("\1\1\2\2\3\3\4\4\1\1");
                if(errorStartMarker<0 || errorStartMarker > error.length())
                    errorStartMarker = 0;
                data<<error.substr(0, errorStartMarker);
                while(executionStack.size() > 1)
                {
                    executionStack.top(currentFrame);
                        data << currentFrame->getType() << "   ~:"<<currentFrame->exp<<'\n';
                    //if(currentFrame->isDynamicAllocation())
                    {
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
                while(executionStack.size() > 1)
                {
                    executionStack.top(currentFrame);
                    //if(currentFrame->isDynamicAllocation())
                    {
                        deleteFrame(currentFrame);
                    }
                    executionStack.pop();
                }
            }
            this->cachedRtnObject = nullptr;
}
double AscalExecutor::calculateExpression(AscalFrame<double>* frame)
{
    frameStack.push(frame);
    operationType operation = nullptr;
    try{
        while(frame->index < frame->exp.length())
        {
            frame->getObjectAt(operation, frame->index);
            operation(KeywordExecutionContext(frame));
        }
        AscalExecutor::Operand data;
        if(!frame->initialOperands.isEmpty())
        {
            frame->initialOperands.top(data);
        }
        frameStack.pop();
        return data.number();
    }
    catch(std::string &error)
    {
        clearStackOnError(true, error, frameStack, frame, frame);
        throw error;
    }
    catch(int exitCode)
    {
        std::string s = frame->exp.str();
        clearStackOnError(false, s, frameStack, frame, frame);
        throw exitCode;
    }
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
    //memoPad.clear();
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
        if(*boolsettings["p"])
        {
            //if(*boolsettings["sci"])
                std::cout<<result<<std::endl;
            //else
              //  std::cout<<ParsingUtil::to_string(result)<<std::endl;
        }
    }
    frameCount = 1;
    varCount = 0;
    
    return result;

}

Object& AscalExecutor::getObject(AscalFrame<double>* frame, string_view functionName)
{
    const auto rec1 = memory.find(functionName);
    if(rec1 != memory.end())
    {
        return *(*rec1).getValue();
    }
    else
        throw std::string("Error locating object "+functionName.str()+"\n");

}
Object* AscalExecutor::getObjectNoError(AscalFrame<double>* frame, string_view functionName)
{
    const auto rec1 = memory.find(functionName);

    if(rec1 != memory.end())
    {
        return (*rec1).getValue();
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
                double lookup = 0;//this->callOnFrame(frame, lStr.data);
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
                    frame->index = index;
                    varName.end = index+lStr.data.size() - 1;
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
                    const auto list = AscalExecutor::resolveNextObjectExpression(frame, varName, nullptr);
                    if(list.data)
                        obj = &(*obj)[list.data->listToString(memory)];
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
                    //varName.data = ParsingUtil::to_string(this->callOnFrame(frame, varName.data.str()));
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

                        const auto list = AscalExecutor::resolveNextObjectExpression(frame, varName, nullptr);
                        if(list.data)
                        {
                            obj = &(*obj)[list.data->listToString(memory)];
                            varName.data = list.data->listToString(memory).data();
                            possible = &obj->getMapUnsafe(varName.data);
                        }
                        frame->index = varName.end+1;
                        while(frame->index < frame->exp.size() && frame->exp[frame->index++]){}
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


