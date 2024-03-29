/*
 * Object.cpp
 *
 *  Created on: Mar 15, 2020
 *      Author: andrew
 */

#include "Object.hpp"
#include "AscalExecutor.hpp"

const uint8_t Object::initialOffset = sizeof(double);
//sizeID codes
const uint32_t Object::SMALL_EXP = 32, Object::MEDIUM_EXP = 256, Object::LARGE_EXP = 4096, Object::VERYLARGE_EXP = 4096*16, //Object::MALLOC_EXP = -1,
Object::SMALL_ID = 16, Object::LARGE_ID = 64;//, Object::MALLOC_ID = -1;

    void Object::compileInstructions()
    {
        //this->instructions = this->compileInstructions(this->instructions, 0);
    }
    template <typename string>
    std::string Object::compileInstructions(string &s, uint32_t start)
    {/*
        uint32_t index = start;
        std::stringstream compiled;
        while(index < s.size())
        {
            if(isalpha(s[index]))
            {
                string_view token = ParsingUtil::getVarNameSV(s, index);
                if(token.find("if") != (uint32_t) -1)
                {
                    compileIf(s, compiled, index);
                }
                //else if(token.find("when") !=  (uint32_t) -1)
                //{
                //    compileWhen(compiled, index);
                //}
                else
                {
                    compiled<<token;
                }
            }
            else
            {
                compiled<<this->instructions[index++];
            }
        }
        //std::cout<<"Compiled str: "<<compiled.str()<<"\n";
        return compiled.str();
        **/
        return std::string(this->instructions.str());
    }
    struct codeRec {
        string_view boolExp;
        std::string codeBlock;
    };
    template <typename string>
    std::stringstream& Object::compileIf(string &s, std::stringstream &instStream, uint32_t &index)
    {
         std::vector<codeRec> records;
         bool elseIfFound = false, elseFound = false;
         do{
         elseIfFound = false;
         elseFound = false;
             while(s[index] == ' ')
             {
                index++;
             }

            uint32_t startOfBoolExp = index;
            uint32_t startOfCodeBlock = index;

            while(s[startOfCodeBlock] && s[startOfCodeBlock] != '{')
            {
                startOfCodeBlock++;
            }
            index = startOfCodeBlock;
            string_view booleanExpression((char*) &this->instructions[0], startOfBoolExp, index);
            if(booleanExpression.size() == 0)
            {
                throw std::string("Error no boolean expression provided in if.\n");
            }
            string_view pcodeBlock = ParsingUtil::getExprInStringSV(s, startOfCodeBlock, '{', '}', ';');
            std::string codeBlock = pcodeBlock.str();
            index = startOfCodeBlock + codeBlock.size() - 1;
            if(index > this->instructions.size())
                index = this->instructions.size();

               index -=1;
            string_view nToken = ParsingUtil::getVarNameSV(s, index);
            if(nToken.find("else") != (uint32_t)  -1)
            {
                elseFound = true;
                while(this->instructions[++index] == '{') {std::cout<<instructions[index]<<"\n";}
                index--;
                uint32_t indexbkp = index;
                nToken = ParsingUtil::getVarNameSV(s, index);
                if(nToken.find("if") != (uint32_t) -1)
                {
                    elseIfFound = true;
                    //load code for else if
                    //instStream<<" ("<<booleanExpression<<") evalSetZF jumpZF("<<(codeBlock.size()+1)<<") "<<codeBlock<<" ";
                    codeRec rec;
                    rec.boolExp = booleanExpression;
                    rec.codeBlock = codeBlock;
                    records.push_back(rec);
                }
                else
                {
                    //just else no proceeding if
                    string_view pelseCodeBlock = ParsingUtil::getExprInStringSV(s, indexbkp, '{', '}', ';');
                    std::string elseCodeBlock = compileInstructions(pelseCodeBlock, 0);

                    //instStream<<" ("<<booleanExpression<<") evalSetZF jumpZF("<<(codeBlock.size()+1)<<") "<<codeBlock<<
                    //        " jumpNZF("<<(elseCodeBlock.size()+1)<<") "<<elseCodeBlock<<" ";
                    codeRec rec;
                    rec.boolExp = booleanExpression;
                    rec.codeBlock = codeBlock;
                    records.push_back(rec);
                    codeRec elseRec;
                    elseRec.codeBlock = elseCodeBlock;
                    records.push_back(elseRec);
                }
            }
            else
            {
                codeRec rec;
                rec.boolExp = booleanExpression;
                rec.codeBlock = codeBlock;
                records.push_back(rec);
            }

         }    while (elseIfFound);
         uint32_t cbcount = 0, becount = 0, sumOfLogsOfJumps = 0, sumOfcbs = 0, sumOfbes = 0;
         for(codeRec &rec : records)
         {
             becount += rec.boolExp.length()>0;
             cbcount += rec.codeBlock.length()>0;
             sumOfLogsOfJumps += std::floor(std::log(rec.codeBlock.length()+1)/std::log(10));
             sumOfcbs += rec.codeBlock.length();
             sumOfbes += rec.boolExp.length()>0;
         }
         uint32_t endIndex = sumOfcbs + sumOfbes + sumOfLogsOfJumps + becount*(31) + 13*(elseFound);
         uint32_t logEndIndex = std::log(endIndex)/std::log(10);
         //for jumpNZ params to skip else case
         endIndex += logEndIndex * (cbcount+elseFound);
         for(codeRec &rec : records)
         {
             if(rec.boolExp.size())
             {
                 instStream<<"("<<rec.boolExp<<") evalSetZF jumpZF("
                         <<(rec.codeBlock.size()+10+logEndIndex+(logEndIndex-std::floor(std::log(rec.codeBlock.size())/std::log(10))))<<") "
                         <<rec.codeBlock
                         <<" jumpNZ("<<((endIndex-instStream.tellp())>=0?(endIndex-instStream.tellp()):0)
                         <<") ";

             }
             else
             {
                 instStream<<" jumpNZ("<<(endIndex-instStream.tellp())<<") "<<rec.codeBlock<<" ";
             }
         }
         index = this->instructions.size();
        return instStream;
    }
    template <typename string>
    std::stringstream& Object::compileWhen(string &s, std::stringstream &instStream, uint32_t &index)
    {

        return instStream;
    }

void newLine(std::stringstream &data,int indentationLevel)
{
    data<<('\n');
    for(int i = 0;i < indentationLevel;i++)
    {
        data<<(' ');
        data<<(' ');
    }
}

std::string Object::instructionsToFormattedString() const
{
    return instructionsToFormattedString(1);
}
std::string Object::instructionsToFormattedString(uint16_t indentationLevel) const
{
    std::stringstream data;

    //newLine(data,indentationLevel);
    //indentationLevel++;
    if(this->instructions[0] != 5 && this->listSize == 0)
        for(size_t i = 0; i < instructions.size(); i++)
        {
            const char &c = this->instructions[i];
            if(c == '{'){
                indentationLevel++;
            }
            else if(c == '}')
            {
                indentationLevel--;
                newLine(data,indentationLevel);
            }

            if(c == ';')
            {
                data<<(c);
                newLine(data,indentationLevel);
            }
            else if(c == 1)
            {
                i += sizeof(double);
                double d;
                memcpy(&d, &this->instructions[Object::initialOffset], sizeof(double));
                data << ParsingUtil::to_string(d);
            }
            else
                data<<(c);
            if(c == '{')
                newLine(data,indentationLevel);
        }
    else {
        data<<"[]";
        newLine(data,indentationLevel);
    }
        newLine(data,indentationLevel);

    return data.str();
}

std::string Object::toString(uint16_t depth)
{
    std::stringstream s;
    newLine(s, depth);
    s<<"{";
    depth++;
    newLine(s, depth);
    s<<"Obj Name: ("<<this->id<<")"<<this;
    newLine(s, depth);
    s<<"Instructions: "<<this->instructionsToFormattedString(depth);
    if(this->getListSize())
    {
        depth++;
        newLine(s, depth);
        s<<"List len: "<<this->getListSize();
        s<<" data: [";
        depth++;
        for(size_t i = 0; i < this->getListSize(); i++)
        {
            newLine(s, depth);
            if(this->isObjList()){
                auto &obj = this->getObjectAtIndex(i);
                s<<obj.toString(depth);
            }
            else
            {
                s<<this->getDoubleAtIndex(i);
            }
        }
        depth--;
        newLine(s, depth);
        s<<"]";
        depth--;
        newLine(s, depth);
    }
    if(this->objectMap.size()>0)
    {
        depth++;
        newLine(s, depth);
        s<<"Mapped data: {";
        depth++;
        for(auto &value:this->objectMap)
        {
            newLine(s, depth);
            s<<value.toString(depth);
        }
        depth--;
        newLine(s, depth);
        s<<"}";
        depth--;
        newLine(s, depth);
    }
    depth--;
    newLine(s, depth);
    s<<"}";
    return s.str();
}
bool cmpVector(const std::vector<std::string>& a,const std::vector<std::string>& b)
{
    bool equal = true;
    if(a.size() != b.size())
        equal = false;
    else
    {
        for(int i = 0; i < a.size() && equal;i++)
        {
            if(a[i].compare(b[i]) != 0)
                equal = false;
        }
    }
    return equal;
}
bool Object::operator==(const Object &o) const
{
    bool equal = (id == o.id);
    return equal;
}
bool Object::operator==(Object &o) const
{
    bool equal = (id == o.id);
    return equal;
}

bool Object::isList()
{
    return this->listSize;
}

bool Object::isObjList()
{
    return this->instructions[0] == 5;
}
size_t Object::getListSize() const
{
    return this->listSize;
}
Object* Object::getListElement(size_t index, FlatMap<string_view, Object*> &memory)
{
    if(index < this->getListSize())
        return &(*this)[index];
    return memory[string_view("null")];
}
Object& Object::operator[](size_t index)
{
    if(this->listSize > index){
        return getObjectAtIndex(index);
    }
    else
        throw std::string("Invalid index: "+std::to_string(index)+" in object: "+this->toString());
}

void Object::pushList(Object &&data)
{
    this->pushList(data);
}
Object& Object::setList(Object &data, size_t index)
{
    if(this->listSize < index)
        throw std::string("Out of range in list: "+this->id.str());
    else if(this->listSize == index)
        this->pushList(data);
    else
        (*this)[index] = data;
    return data;
}


Object& Object::getMapUnsafe(string_view id)
{
    return this->objectMap[id];
}
Object& Object::operator[](string_view id)
{
    if(this->objectMap.count(id) > 0)
        return this->objectMap[id];
    else
        throw std::string("Error invalid field ("+id.str()+") in object "+this->id.str());
}
std::string Object::listToString(FlatMap<string_view, Object*> &memory)
{
    
    if(this->isList())
    {
        std::string str;
        for(size_t i = 0; i < this->getListSize(); i++)
        {
            char letter = (char) this->getDoubleAtIndex(i);
            if(isprint(letter))
                str += letter;
        }
        return str;
    }
    else if(this->isDouble())
    {
        return ParsingUtil::to_string(this->getDouble());
    }
    else if(this->isDoubleList())
    {
        std::string str;
        for(int i = 0; i < this->getListSize(); i++)
        {
            str += this->getDoubleAtIndex(i);
        }
        return str;
    }
    else
    {
        std::string str;
        for(int i = 0; i < this->getListSize(); i++)
        {
            str += this->getObjectAtIndex(i).listToString(memory);
        }
        return str;
    }
}
void Object::printList(FlatMap<string_view, Object*> &memory)
{
    for(size_t i = 0; i < this->getListSize(); i++)
    {
        std::cout<<(char) this->getDoubleAtIndex(i);
    }
}
void Object::loadString(string_view s)
{
    clearList();
    uint16_t last = s[0],cur = s[0];
    for(uint32_t i = 1; i < s.size(); i++)
    {
        last = cur;
        cur = s[i];
        if(last == '\\' && cur == 'n'){
            this->pushList((double) '\n');
            if(i+1<s.size())
                cur = s[++i];
        }
        else
            this->pushList((double) last);
    }
    if(cur != '\"' && (last != '\\' || cur != 'n'))
        this->pushList((double) cur);

}
    
Object Object::subString(uint_fast64_t start, uint_fast64_t length, FlatMap<string_view, Object*> &)
{
    Object subString(this->objectMap.getMemMan(), id+"Sub","","");
    if(this->isDoubleList())
    {
        for(uint_fast64_t i = start; i < start+length; i++)
        {
            subString.pushList(this->getDoubleAtIndex(i));
        }
    }
    else if(this->isObjList())
    {
        for(uint_fast64_t i = start; i < start+length; i++)
        {
            subString.pushList(this->getObjectAtIndex(i));
        }
    }
    return subString;
}
Object Object::splitString(string_view filter, FlatMap<string_view, Object*> &memory)
{
    Object splitArr(this->objectMap.getMemMan(), id+"Split","","");
    size_t start = 0, end = 0;
    std::stringstream data;
    for(size_t i = 0; i < this->getListSize(); i++)
    {
        //just getting double will probably be a bug unless we make sure every variable even in lists is in double form
        //temporary fix to get compiling for testing other issues need to revise
        data<<(char) (this->getDoubleAtIndex(i));
    }
    std::string datastr = data.str();
    string_view dataAsSV(datastr);
    // search for pattern
    // set end to index returned -1
    // load string
    while(end < datastr.size())
    {
        end = dataAsSV.find(filter, start)-1;
        end = end<=datastr.size()?end:datastr.size();
        Object element(this->objectMap.getMemMan(), "","","");
        element.loadString(string_view((char*)datastr.c_str()+start, end));
        splitArr.pushList(element);
        start = end+filter.size();
        end = start;
    }
    return splitArr;
}
/*
int Object::setParams(string_view param, uint32_t start)
{
    params.clear();
    uint32_t end = start,startBackup = start;
    while(param.size() > start && param[start] != '(')// && !isalpha(param[start]) && param[start] != '-' && param[start] != '&' && !(param[start] < 58 && param[start]> 47))
    {
        ++start;
        ++end;
    }

    start += param.size() > start;
    end += param.size() > start;
    int pCount = param.size() > start;
    bool foundClosing = !pCount;
    while(!foundClosing && param.size() > end)
    {
        pCount += (param[end] == '(') - (param[end] == ')');
        if(pCount == 0)
        {
            foundClosing = true;
        }
        if((param[end] == ',' || foundClosing) && end > start && (pCount == 1 || (pCount == 0 && foundClosing)))
        {
            params.push_back(SubStrSV(param.substr(start,end-start),start, end));
                start = end;
                while(param[start] == ',' || (!isalpha(param[start]) && param[start] != '-' && !(param[start] < 58 && param[start > 47])))
                {
                    start++;
                    end++;
                }

        }

        end++;
    }
    return ++end-startBackup;
}*/
void Object::setParent(Object *p)
{
    this->parent = p;
}
Object& Object::loadChild(Object &data, AscalExecutor &runtime)
{
    data.parent = this;
    return runtime.loadUserDefinedFn(data, this->objectMap);
}

void Object::eraseList(long index)
{
    if(index >= 0 && index < this->getListSize())
    {
        if(this->isDoubleList() || this->isObjList())
        {
            void* element = reinterpret_cast<void*>(&this->instructions[initialOffset + index * sizeof(uint64_t)]);
            void* finalElement = reinterpret_cast<void*>(&this->instructions[this->instructionBufferSizeId - 1 * sizeof(uint64_t)]);
            const long elementsToMove = reinterpret_cast<uint64_t*>(finalElement) - reinterpret_cast<uint64_t*>(element);

            if(this->isObjList())
            {
                this->objectMap.getMemMan().obj_free(reinterpret_cast<Object*>(element));
                memcpy(element, reinterpret_cast<Object*>(element) + 1, elementsToMove * sizeof(uint64_t));
            }
            else
                memcpy(element, reinterpret_cast<double*>(element) + 1, elementsToMove * sizeof(uint64_t));
            this->listSize--;
        }
    }
    else
        throw std::string("Error deleting element, no element at index: ") + ParsingUtil::to_string(index);
}
Object* Object::getThis()
{
    return this->parent;
}
std::string Object::toString()
{
    return toString(0);
}
void Object::deallocateId(void *ptr, const size_t bufSize)
{
    ptr = static_cast<void*>(static_cast<char*>(ptr));
    switch (bufSize){
        case (Object::SMALL_ID):
            this->objectMap.getMemMan().small_id_free(ptr);
            break;
        case (Object::LARGE_ID):
            this->objectMap.getMemMan().large_id_free(ptr);
            break;
        default:
            free(ptr);
    }
}
void Object::deallocateInstructions(void *ptr, const size_t bufSize)
{
    ptr = static_cast<void*>(static_cast<char*>(ptr));
    switch (bufSize) {
        case (Object::SMALL_EXP):
            this->objectMap.getMemMan().small_free(ptr);
            break;
        case (Object::MEDIUM_EXP):
            this->objectMap.getMemMan().medium_free(ptr);
            break;
        case (Object::LARGE_EXP):
            this->objectMap.getMemMan().large_free(ptr);
            break;
        case (Object::VERYLARGE_EXP):
            this->objectMap.getMemMan().vlarge_free(ptr);
            break;
        default:
            free(static_cast<void*>(ptr));
            break;
    };
}
void Object::deallocateMemory(void* ptr, const size_t bufSize, void *idptr, const uint32_t idbufSize)
{
    deallocateId(idptr, idbufSize);
    deallocateInstructions(ptr, bufSize);
}

bool Object::isDoubleList()
{
    return this->instructions[0] == 6;
}
void Object::pushList(double data)
{
    static const uint16_t objSize = sizeof(double);
    if(initialOffset+this->listSize*(objSize) < this->instructionBufferSizeId)
    {
        this->instructions[0] = 6;
        memcpy(&this->instructions[initialOffset+this->listSize*(objSize)], &data, objSize);
        listSize++;
    }
    else
    {
        void *ptr = this->inp;
        const uint32_t bufSize = this->instructionBufferSizeId;
        //sets the id pointer to be the same as the front of the newly allocated block
        this->resizeInstructions(this->instructionBufferSizeId*2);
        memcpy(&this->instructions[0], ptr, bufSize);
        this->deallocateInstructions(ptr, bufSize);
        pushList(data);
    }/*
    else
    {
        Object obj(this->objectMap.getMemMan(), std::to_string(listSize)+this->id.str());
        obj.setDouble(data);
        this->pushList(obj);
    }*/
}
void Object::pushList(Object &data)
{
    static const uint16_t objSize = sizeof(Object*);
    if(initialOffset+this->listSize*(objSize) < this->instructionBufferSizeId)
    {
        this->instructions[0] = 5;
        data.id = std::to_string(listSize)+this->id.str();
        data.parent = this;
        Object *obj = this->objectMap.getMemMan().constructObj(data);
        memcpy(&this->instructions[initialOffset+this->listSize*(objSize)], &obj, objSize);
        listSize++;
    }
    else
    {
        if(this->instructionBufferSizeId < data.instructions.size() + this->instructions.size())
        {
            this->instructions.resize(this->instructions.size() + data.instructions.size());
        }
        else
        {
            void *ptr = this->inp;
            const uint32_t bufSize = this->instructionBufferSizeId;
            //sets the id pointer to be the same as the front of the newly allocated block
            this->resizeInstructions(this->instructions.size() + data.instructions.size());
            memcpy(&this->instructions[0], ptr, bufSize);
            this->deallocateInstructions(ptr, bufSize);
        }
        pushList(data);
    }
}
void Object::resizeInstructions(uint32_t newSize)
{
    if(newSize <= Object::MEDIUM_EXP)
    {
        this->instructionBufferSizeId = Object::MEDIUM_EXP;
        this->inp = this->objectMap.getMemMan().medium_alloc();
        this->instructions = string_view(static_cast<const char*>(this->inp), newSize);
    }
    else if(newSize <= Object::LARGE_EXP)
    {
        this->instructionBufferSizeId = Object::LARGE_EXP;
        this->inp = this->objectMap.getMemMan().large_alloc();
        this->instructions = string_view(static_cast<const char*>(this->inp), newSize);
    }
    else if(newSize <= Object::VERYLARGE_EXP)
    {
        this->instructionBufferSizeId = Object::VERYLARGE_EXP;
        this->inp = this->objectMap.getMemMan().vlarge_alloc();
        this->instructions = string_view(static_cast<const char*>(this->inp), newSize);
    }
    else
    {
        this->instructionBufferSizeId = newSize;
        this->inp = malloc(newSize);
        this->instructions = string_view(static_cast<const char*>(this->inp), newSize);
    }
}
Object& Object::copyExceptID(const Object& o)
{
    copyToInstructions(o.instructions);
    this->objectMap = o.objectMap;
    this->parent = o.parent;
    return *this;
}
void Object::copyToId(string_view id)
{
    if(id.size() <= this->IdBufferSizeId)
    {
        memcpy(&this->id[0], &id[0], id.size());
        this->id.resize(id.size());
    }
    else
    {
        void *ptr = this->idp;
        const uint32_t bufSize = this->IdBufferSizeId;
        this->loadId(id);
        this->deallocateId(ptr, bufSize);
    }
}
void Object::copyToInstructions(string_view ins)
{
    if(this->instructionBufferSizeId >= ins.size())
    {
        memcpy(&this->instructions[0], &ins[0], ins.size());
        this->instructions.resize(ins.size());
    }
    else
    {
       void *ptr = this->inp;
        const uint32_t bufSize = this->instructionBufferSizeId;
        this->loadInstructions(ins);
        this->deallocateInstructions(ptr, bufSize);
    }
}
void Object::clone(const Object &o)
{
    loadData(o.id, o.instructions);
    if(this->isObjList())
    {
        for(int_fast64_t i = 0; i < o.getListSize(); i++)
        {
            Object &toCopy = o.getObjectAtIndex(i);
            char *location = &this->instructions[initialOffset+i*(sizeof(uint64_t))];
            Object *newObj = this->objectMap.getMemMan().constructObj(toCopy);
            memcpy(&location, &newObj, sizeof(Object*));
        }
    }
    this->objectMap = o.objectMap;
    this->listSize = o.getListSize();
}
Object& Object::operator=(const Object &o)
{
    this->clone(o);
    return *this;
}
//need to make a copy constructor for the objectMap
Object::Object(const Object &o): objectMap(o.objectMap)
{
    this->clone(o);
}
Object::Object(MemoryManager &memMan, std::string &&id,std::string &&expression,std::string &param): objectMap(memMan)
{
    loadData(id, expression);
}
Object::Object(MemoryManager &memMan, std::string &&id,const std::string &expression,std::string &&param): objectMap(memMan)
{
    loadData(id, expression);
}
Object::Object(MemoryManager &memMan, std::string &id,std::string &expression,std::string &param): objectMap(memMan)
{
    loadData(id, expression);
}
Object::Object(MemoryManager &memMan, std::string &id,std::string &expression,std::string &&param): objectMap(memMan)
{
    loadData(id, expression);
}
Object::Object(MemoryManager &memMan, std::string &&id,std::string &&expression,std::string &&param): objectMap(memMan)
{
    loadData(id, expression);
    //instructions = compileInstructions(0);
}
Object::Object(MemoryManager &memMan, std::string &id,std::string &&expression,std::string &&param): objectMap(memMan)
{
    loadData(id, expression);
}
Object::Object(MemoryManager &memMan): objectMap(memMan)
{
    loadData(std::string("                "),std::string(""));
}
Object::Object(MemoryManager &memMan, string_view id): objectMap(memMan)
{
    char c = 0;
    string_view exp(&c, 0);
    loadData(id, exp);
}
Object::Object(MemoryManager &memMan, string_view id, string_view exp): objectMap(memMan)
{
    loadData(id, exp);
}
Object::~Object() {
    clearList();
    deallocateId(this->idp, this->IdBufferSizeId);
    deallocateInstructions(this->inp, this->instructionBufferSizeId);
    //deallocateMemory(this->inp, this->instructionBufferSizeId, this->idp, this->IdBufferSizeId);
}

//does not free old memory!!!
//only allocates appropriate sized buffer, then copies data supplied in params to new buffer
void Object::loadInstructions(string_view exp)
{
    if(exp.size() < Object::SMALL_EXP)
    {
        this->instructionBufferSizeId = Object::SMALL_EXP;
        this->inp = this->objectMap.getMemMan().small_alloc();
        this->instructions = string_view(static_cast<const char*>(this->inp), exp.size());
    }
    else if(exp.size() < Object::MEDIUM_EXP)
    {
        this->instructionBufferSizeId = Object::MEDIUM_EXP;
        this->inp = this->objectMap.getMemMan().medium_alloc();
        this->instructions = string_view(static_cast<const char*>(this->inp), exp.size());
    }
    else if(exp.size() < Object::LARGE_EXP)
    {
        this->instructionBufferSizeId = Object::LARGE_EXP;
        this->inp = this->objectMap.getMemMan().large_alloc();
        this->instructions = string_view(static_cast<const char*>(this->inp), exp.size());
    }
    else if(exp.size() < Object::VERYLARGE_EXP)
    {
        this->instructionBufferSizeId = Object::VERYLARGE_EXP;
        this->inp = this->objectMap.getMemMan().vlarge_alloc();
        this->instructions = string_view(static_cast<const char*>(this->inp), exp.size());
    }
    else
    {
        this->instructionBufferSizeId = exp.size();
        this->inp = malloc(exp.size());
        this->instructions = string_view(static_cast<const char*>(this->inp), exp.size());
    }
    memcpy((char*) this->instructions.c_str(), (char*) exp.c_str(), exp.size());
    this->instructions[instructions.size()] = '\0';
}
void Object::loadId(string_view id)
{
    if(id.size() <= Object::SMALL_ID-1)
    {
        this->IdBufferSizeId = Object::SMALL_ID;
        this->idp =this->objectMap.getMemMan().small_id_alloc();
        this->id = string_view(static_cast<const char*>(this->idp), id.size());
    }
    else if(id.size() <= Object::LARGE_ID-1)
    {
        this->IdBufferSizeId = Object::LARGE_ID;
        this->idp = this->objectMap.getMemMan().large_id_alloc();
        this->id = string_view(static_cast<const char*>(this->idp), id.size());
    }
    else
    {
        this->IdBufferSizeId = id.size();
        this->idp = malloc(id.size());
        this->id = string_view(static_cast<const char*>(this->idp), id.size());
    }
    memcpy((char*) this->id.c_str(), id.c_str(), id.size());
    this->id[id.size()] = 0;
}

