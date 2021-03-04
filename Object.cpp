/*
 * Object.cpp
 *
 *  Created on: Mar 15, 2020
 *      Author: andrew
 */

#include "Object.hpp"
#include "AscalExecutor.hpp"


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
				//	compileWhen(compiled, index);
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
		return this->instructions;
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
	        string_view booleanExpression(this->instructions, startOfBoolExp, index);
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
				    //		" jumpNZF("<<(elseCodeBlock.size()+1)<<") "<<elseCodeBlock<<" ";
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

		 }	while (elseIfFound);
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
				long double *d = (long double*) (&this->instructions[i+1]);
				i += sizeof(long double);
				data << ParsingUtil::to_string(*d);
			}
			else
				data<<(c);
			if(c == '{')
				newLine(data,indentationLevel);
		}
		newLine(data,indentationLevel);

	return data.str();
}

Object& Object::operator=(const Object& o)
{
	if(this->id.size() == o.id.size())
	{
		for(uint32_t i = 0; i < this->id.size(); i++)
		{
			this->id[i] = o.id[i];
		}
	}
	else
		this->id = o.id;
	if(this->instructions.size() == o.instructions.size())
	{
		for(uint32_t i = 0; i < this->instructions.size(); i++)
		{
			this->instructions[i] = o.instructions[i];
		}
	}
	else
		this->instructions = o.instructions;
	this->objectList = o.objectList;
	this->objectMap = o.objectMap;
	this->parent = o.parent;
	this->params = o.params;
	return *this;
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
	if(this->objectList.size())
	{
		depth++;
		newLine(s, depth);
		s<<"List len: "<<this->getListSize();
		s<<" data: [";
		depth++;
		for(auto &obj:this->objectList)
		{
			newLine(s, depth);
			s<<obj.toString(depth);
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
std::string& Object::getInstructions()
{
	return  instructions;
}

void Object::addInstruction(std::string expression)
{
	instructions = expression;
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
	bool equal = id.compare(o.id) == 0;
	return equal;
}
bool Object::operator==(Object &o) const
{
	bool equal = id.compare(o.id) == 0;
	return equal;
}

bool Object::isList()
{
	return this->objectList.size();
}
size_t Object::getListSize()
{
	return this->objectList.size();
}
Object& Object::getListElement(size_t index, MemoryMap &memory)
{
	if(index < this->getListSize())
		return this->objectList[index];
	return memory[string_view("null")];
}
void Object::pushList(Object &data)
{
	data.id = std::to_string(this->objectList.size())+this->id;
	data.parent = this;
	this->objectList.push_back(data);
}
void Object::pushList(Object &&data)
{
	data.id = std::to_string(this->objectList.size())+this->id;
	data.parent = this;
	this->objectList.push_back(data);
}
Object& Object::setList(Object &data, size_t index)
{
	if(this->objectList.size() < index)
		throw std::string("Out of range in list: "+this->id);
	else if(this->objectList.size() < index)
		this->pushList(data);
	else
		this->objectList[index] = data;
	return data;
}

void Object::clearList()
{
	this->objectList.clear();
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
		throw std::string("Error invalid field ("+id.str()+") in object "+this->id);
}
Object& Object::operator[](size_t index)
{
	if(this->objectList.size() > index)
		return this->objectList[index];
	else
		throw std::string("Invalid index: "+std::to_string(index)+" in object: "+this->toString());
}
std::string Object::listToString(MemoryMap &memory)
{
	std::stringstream str;
    for(size_t i = 0; i <= this->getListSize(); i++)
    {
    	char letter = (char) stoi(this->getListElement(i, memory).getInstructions().substr(0, 4));
    	if(isprint(letter))
    		str<<letter;
    }
    return str.str();
}
void Object::printList(MemoryMap &memory)
{
    for(size_t i = 0; i <= this->getListSize(); i++)
    {
    		std::cout<<(char) stoi(this->getListElement(i, memory).getInstructions().substr(0, 4));
    }
}
void Object::loadString(string_view s)
{
	clearList();
	uint16_t last = s[0],cur = s[0];
    for(size_t i = 1; i < s.size(); i++)
    {
    	last = cur;
    	cur = s[i];
    	if(last == '\\' && cur == 'n'){
    		this->pushList(Object(objectMap.getMemMan(), "","10",""));
    		if(i+1<s.size())
    			cur = s[++i];
    	}
    	else
    		this->pushList(Object(objectMap.getMemMan(), "", std::to_string(last), ""));
    }
    if(cur != '\"' && (last != '\\' || cur != 'n'))
		this->pushList(Object(objectMap.getMemMan(), "", std::to_string(cur), ""));

}
Object Object::splitString(string_view filter, MemoryMap &memory)
{
	Object splitArr(memory.getMemMan(), id+"Split","","");
	size_t start = 0, end = 0;
	std::stringstream data;
	for(size_t i = 0; i < this->getListSize(); i++)
	{
		data<<(char) stoi(this->getListElement(i, memory).instructions);
	}
	std::string datastr = data.str();
	string_view dataAsSV(datastr);
	// search for regex
	// set end to index returned -1
	// load string
	while(end < datastr.size())
	{
		end = dataAsSV.find(filter, start)-1;
		end = end<=datastr.size()?end:datastr.size();
		Object element(memory.getMemMan(), "","","");
		element.loadString(string_view((char*)datastr.c_str()+start, end));
		splitArr.pushList(element);
		start = end+filter.size();
		end = start;
	}
	return splitArr;
}

void Object::setDouble(long double d)
{
	if(this->instructions.size() < 14)
	{
		this->instructions = "01234567890123";
	}
	char *c = (char*) this->instructions.c_str();
	c[0] = 1;
	long double *mem = (long double *) &c[1];
	*mem = d;

}
long double Object::getDouble() const
{
	return *(long double*)(this->instructions.c_str()+1);
}
bool Object::isDouble()
{
	return this->instructions[0] == 1;
}
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
}
Object& Object::loadChild(Object &data, AscalExecutor &runtime)
{
	data.parent = this;
	runtime.loadUserDefinedFn(data, this->objectMap);
	return data;
}

Object* Object::getThis()
{
	return this->parent;
}
std::string Object::toString()
{
	return toString(0);
}
Object::Object(MemoryManager<Object> &memMan, std::string &id):id(id), objectMap(memMan)
{
	instructions = "*12456789";
}
Object::Object(MemoryManager<Object> &memMan, std::string &&id):id(id), objectMap(memMan)
{
	instructions = "*12456789";
}
Object::Object(MemoryManager<Object> &memMan, std::string &id, std::string &param):id(id), objectMap(memMan)
{
	instructions = "*12456789";
	setParams(param);
}
Object::Object(MemoryManager<Object> &memMan, std::string &&id,std::string &&expression,std::string &param):id(id), objectMap(memMan)
{
	setParams(param);
	instructions = expression;
}
Object::Object(MemoryManager<Object> &memMan, std::string &&id,const std::string &expression,std::string &&param):id(id), objectMap(memMan)
{
	setParams(param);
	instructions = expression;
}
Object::Object(MemoryManager<Object> &memMan, std::string &id,std::string &expression,std::string &param):id(id), objectMap(memMan)
{
	setParams(param);
	instructions = expression;
}
Object::Object(MemoryManager<Object> &memMan, std::string &id,std::string &expression,std::string &&param):id(id), objectMap(memMan)
{
	setParams(param);
	instructions = expression;
}
Object::Object(MemoryManager<Object> &memMan, std::string &&id,std::string &&expression,std::string &&param):id(id), objectMap(memMan)
{
	setParams(param);
	instructions = expression;
	//instructions = compileInstructions(0);
}
Object::Object(MemoryManager<Object> &memMan, std::string &id,std::string &&expression,std::string &&param):id(id), objectMap(memMan)
{
	setParams(param);
	instructions = expression;
}
Object::Object(MemoryManager<Object> &memMan): objectMap(memMan)
{
	this->id = "";
}
Object::~Object() {
	// TODO Auto-generated destructor stub
}

