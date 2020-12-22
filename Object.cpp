/*
 * Object.cpp
 *
 *  Created on: Mar 15, 2020
 *      Author: andrew
 */

#include "Object.hpp"

void newLine(std::vector<char> &data,int indentationLevel)
{
	data.push_back('\n');
	for(int i = 0;i < indentationLevel;i++)
	{
		data.push_back(' ');
		data.push_back(' ');
	}
}
std::string Object::instructionsToFormattedString()
{
	std::vector<char> data;
	data.reserve(256);

	int indentationLevel = 1;
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
				data.push_back(c);
				newLine(data,indentationLevel);
			}
			else
				data.push_back(c);
			if(c == '{')
				newLine(data,indentationLevel);
		}
		newLine(data,indentationLevel);

	return std::string(data.begin(),data.end());
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

Object Object::getChild(std::string &id)
{
	return this->objectMap.count(id)?this->objectMap[id]:throw std::string("Error finding "+this->id+"."+id);
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
	if(index < this->listSize)
		return this->objectList[index];
	return memory["null"];
}
void Object::pushList(Object &data)
{
	data.id = this->id+std::to_string(this->objectList.size());
	this->objectList.push_back(data);
}
void Object::pushList(Object &&data)
{
	data.id = this->id+std::to_string(this->objectList.size());
	this->objectList.push_back(data);
}
void Object::setList(Object &data, size_t index)
{
	if(this->objectList.size() < index)
		throw std::string("Out of range in list: "+this->id);
	else if(this->objectList.size() < index)
		this->pushList(data);
	else
		this->objectList[index] = data;
}

void Object::clearList()
{
	this->objectList.clear();
}
void Object::printList(std::unordered_map<std::string,Object> &memory)
{
    for(size_t i = 0; i <= this->getListSize(); i++)
    {
    		std::cout<<(char) stoi(this->getListElement(i, memory).getInstructions());
    }
}
void Object::loadString(std::string_view s)
{
	uint16_t last = s[0],cur = s[0];
    for(size_t i = 1; i <= s.size()+1; i++)
    {
    	last = cur;
    	cur = s[i];
    	if(last == '\\' && cur == 'n'){
    		this->pushList(Object("","10",""));
    		i++;
    	}
    	else
    		this->pushList(Object("", std::to_string(last), ""));
    }
}
Object Object::splitString(std::string_view filter, std::unordered_map<std::string,Object> &memory)
{
	Object splitArr(id+"Split","","");
	size_t start = 0, end = 0;
	std::stringstream data;
	for(size_t i = 0; i < this->getListSize(); i++)
	{
		data<<(char) stoi(this->getListElement(i, memory).instructions);
	}
	std::string datastr = data.str();
	// search for regex
	// set end to index returned -1
	// load string
	while(end < datastr.size())
	{
		end = datastr.find(filter, start)-1;
		end = end<=datastr.size()?end:datastr.size();
		Object element("","","");
		element.loadString(std::string_view(datastr.c_str()+start, end));
		splitArr.pushList(element);
		start = end+filter.size();
		end = start;
	}
	return splitArr;
}
int Object::setParams(std::string &param)
{
    params.resize(0);
	int start = 0,end = 0;
	while(param[start] && param[start] != '(' && !isalpha(param[start]) && param[start] != '-' && param[start] != '&' && !(param[start] < 58 && param[start]> 47))
	{
		++start;
		++end;
	}
    start += param[start] != 0;
    end += param[start] != 0;
	int pCount = 1;
	bool foundClosing = false;
	while(!foundClosing && param[end])
	{
        pCount += (param[end] == '(') - (param[end] == ')');
        if(pCount == 0)
        {
            foundClosing = true;
        }
		if((param[end] == ',' || foundClosing) && end > start && (pCount == 1 || (pCount == 0 && foundClosing)))
		{
			params.push_back(param.substr(start,end-start));
                start = end;
                while(param[start] == ',' || (!isalpha(param[start]) && param[start] != '-' && !(param[start] < 58 && param[start > 47])))
                {
                    start++;
                    end++;
                }

		}

		end++;
	}
	return ++end;
}

std::string Object::toString(uint16_t depth)
{
	std::stringstream s;
	for(int i = 0; i < depth; i++)
		s<<"    ";
	s<<"{\n";
	depth++;
	for(int i = 0; i < depth; i++)
		s<<"    ";
	s<<"Obj Name: "<<this->id<<"\n";
	for(int i = 0; i < depth; i++)
		s<<"    ";
	s<<"Instructions: "<<this->instructionsToFormattedString()<<"\n";
	if(this->objectMap.size()>0)
	{
		depth++;
		for(int i = 0; i < depth; i++)
			s<<"    ";
		s<<"data {\n";
		for(int i = 0; i < depth; i++)
			s<<"    ";

		for(auto &[key,value]:this->objectMap)
		{
			for(int i = 0; i < depth; i++)
				s<<"    ";
			s<<value.toString(depth)<<"\n";
		}
		s<<"\n";
		for(int i = 0; i < depth; i++)
			s<<"    ";
		s<<"}";
		s<<"\n";
		depth--;
	}
	for(int i = 0; i < depth; i++)
		s<<"    ";
	s<<"}";
	return s.str();
}
std::string Object::toString()
{
	return toString(0);
}
Object::Object(std::string id,std::string expression,std::string param):id(id)
{
	setParams(param);
	instructions = expression;
	//std::cout<<expression<<std::endl;
	/*for(int i = 0;i < expression.length();i++)
	{
		if(expression[i] == '\n')
		{
			instructions.push_back(expression.substr(start,i-start));
			start = ++i;
		}
	}*/

}
Object::Object()
{
	this->id = "";
}
Object::~Object() {
	// TODO Auto-generated destructor stub
}

