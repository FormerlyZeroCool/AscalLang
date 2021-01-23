/*
 * Object.cpp
 *
 *  Created on: Mar 15, 2020
 *      Author: andrew
 */

#include "Object.hpp"


	std::string Object::compileInstructions(uint32_t start)
	{
		uint32_t index = start;
		std::stringstream compiled;
		while(index < this->instructions.size())
		{
			if(isalpha(this->instructions[index]))
			{
				SubStr token = ParsingUtil::getVarName(this->instructions, index);
				if(token.data.compare("if") == 0)
				{
					compileIf(compiled, index);
				}
				else if(token.data.compare("when") == 0)
				{
					compileWhen(compiled, index);
				}
				else
				{
					compiled<<token.data;
					index += token.data.size();
				}
			}
			else
				compiled<<this->instructions[index];
		}
		return compiled.str();
	}
	std::stringstream& Object::compileIf(std::stringstream &instStream, uint32_t &index)
	{

		 index += 2;
	    while(this->instructions[index] == ' ')
	    {
	            index++;
	    }

	        uint32_t startOfBoolExp = index;
	        uint32_t startOfCodeBlock = index;
	        SubStr codeBlock("",0,0);

	        while(this->instructions[startOfCodeBlock] && this->instructions[startOfCodeBlock] != '{')
	        {
	            startOfCodeBlock++;
	        }
	        index = startOfCodeBlock;
	        const std::string booleanExpression = this->instructions.substr(startOfBoolExp,index-startOfBoolExp);
	        if(booleanExpression.size() == 0)
	        {
	            throw std::string("Error no boolean expression provided in if.\n");
	        }

	        //codeBlock = ParsingUtil::getCodeBlock(frame, startOfCodeBlock, runtime->ascal_cin);

	        index = startOfCodeBlock + codeBlock.end - 1;
	        if(index > this->instructions.size())
	            index = this->instructions.size();


	       index -=1;

		return instStream;
	}
	std::stringstream& Object::compileWhen(std::stringstream &instStream, uint32_t &index)
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

std::string Object::instructionsToFormattedString()
{
	return instructionsToFormattedString(1);
}
std::string Object::instructionsToFormattedString(uint16_t indentationLevel)
{
	std::stringstream data;

	//newLine(data,indentationLevel);
	//indentationLevel++;

		for(char &c:instructions)
		{
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
			else
				data<<(c);
			if(c == '{')
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
		for(auto &[key,value]:this->objectMap)
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
std::string Object::instructionsToString()
{
	/*std::vector<char> data;
		data.reserve(128);
		for(int i = 0;i < instructions.size();i++)
		{
			for(char &c:instructions[i])
			{
				data.push_back(c);
			}
			data.push_back('\n');
		}
		return std::string(data.begin(),data.end()-1);
		*/

	return instructions;
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
bool Object::operator==(Object o)
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
Object& Object::getListElement(size_t index,std::unordered_map<std::string,Object> &memory)
{
	if(index < this->getListSize())
		return this->objectList[index];
	return memory["null"];
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

Object& Object::getMapUnsafe(std::string &id)
{
	return this->objectMap[id];
}
Object& Object::operator[](std::string &id)
{
	if(this->objectMap.count(id) > 0)
		return this->objectMap[id];
	else
		throw std::string("Error invalid field ("+id+") in object "+this->id);
}
Object& Object::operator[](std::string &&id)
{
	if(this->objectMap.count(id) > 0)
		return this->objectMap[id];
	else
		throw std::string("Error invalid field ("+id+") in object "+this->id);
}
Object& Object::operator[](size_t index)
{
	if(this->objectList.size() > index)
		return this->objectList[index];
	else
		throw std::string("Invalid index: "+std::to_string(index)+" in object: "+this->toString());
}
std::string Object::listToString(std::unordered_map<std::string,Object> &memory)
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
void Object::printList(std::unordered_map<std::string,Object> &memory)
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
    		this->pushList(Object("","10",""));
    		if(i+1<s.size())
    			cur = s[++i];
    	}
    	else
    		this->pushList(Object("", std::to_string(last), ""));
    }
    if(cur != '\"' && (last != '\\' || cur != 'n'))
		this->pushList(Object("", std::to_string(cur), ""));

}
Object Object::splitString(string_view filter, std::unordered_map<std::string,Object> &memory)
{
	Object splitArr(id+"Split","","");
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
		Object element("","","");
		element.loadString(string_view((char*)datastr.c_str()+start, end));
		splitArr.pushList(element);
		start = end+filter.size();
		end = start;
	}
	return splitArr;
}

int Object::setParams(std::string &param, uint32_t start)
{
    params.clear();
    uint32_t end = start,startBackup = start;
	while(param.size() > start && param[start] != '(' && !isalpha(param[start]) && param[start] != '-' && param[start] != '&' && !(param[start] < 58 && param[start]> 47))
	{
		++start;
		++end;
	}

    start += param[start] != 0;
    end += param[start] != 0;
	int pCount = 1;
	bool foundClosing = false;
	while(!foundClosing && param.size() > end)
	{
        pCount += (param[end] == '(') - (param[end] == ')');
        if(pCount == 0)
        {
            foundClosing = true;
        }
		if((param[end] == ',' || foundClosing) && end > start && (pCount == 1 || (pCount == 0 && foundClosing)))
		{
			params.push_back(SubStr(param.substr(start,end-start),start, end));
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
Object& Object::loadChild(Object &data)
{
	data.parent = this;
	this->objectMap[data.id] = data;
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
Object::Object(std::string &&id,std::string &&expression,std::string &param):id(id)
{
	setParams(param);
	instructions = expression;
}
Object::Object(std::string &&id,const std::string &expression,std::string &&param):id(id)
{
	setParams(param);
	instructions = expression;
}
Object::Object(std::string &id,std::string &expression,std::string &param):id(id)
{
	setParams(param);
	instructions = expression;
}
Object::Object(std::string &id,std::string &expression,std::string &&param):id(id)
{
	setParams(param);
	instructions = expression;
}
Object::Object(std::string &&id,std::string &&expression,std::string &&param):id(id)
{
	setParams(param);
	instructions = expression;
}
Object::Object(std::string &id,std::string &&expression,std::string &&param):id(id)
{
	setParams(param);
	instructions = expression;
}
Object::Object()
{
	this->id = "";
}
Object::~Object() {
	// TODO Auto-generated destructor stub
}

