/*
 * Object.cpp
 *
 *  Created on: Mar 15, 2020
 *      Author: andrew
 */

#include "Object.h"

std::string Object::instructionsToString()
{
	std::vector<char> data;
	data.reserve(128);
	for(int i = 0;i < instructions.size();i++)
	{
		for(char &c:instructions[i])
		{
			data.push_back(c);
		}
		data.push_back('\n');
	}
	return std::string(data.begin(),data.end());
}

std::vector<std::string>& Object::getInstructions()
{
	return  instructions;
}

void Object::addInstruction(std::string expression)
{
	instructions.push_back(expression);
}
bool cmpVector(const std::vector<std::string>& a,const std::vector<std::string>& b)
{
	bool equal = true;
	if(a.size() != b.size())
		equal = false;
	else
	{
		for(int i = 0; i < a.size();i++)
		{
			if(a[i].compare(b[i]) != 0)
				equal = false;
		}
	}
	return equal;
}
bool Object::operator==(Object o)
{
	bool equal = true;
	if(id.compare(o.id) != 0)
		equal = false;
	if(equal && cmpVector(instructions,o.instructions))
		equal = false;
	return equal;
}

int Object::setParams(std::string param)
{
	//std::cout<<"Expression:"<<expression<<" setParam in Object class input: "<<param<<std::endl;
	params.resize(0);
	int start = 0,end = 0;
	while(param[start] && !isalpha(param[start]) && param[start] != '-' && !(param[start] < 58 && param[start]> 47))
	{
		//std::cout<<"setParam in Object class in first while: "<<param.substr(start,param.length())<<std::endl;
		start++;
		end++;
	}
	//std::cout<<"setParam in Object class after first while: "<<param.substr(start,param.length())<<std::endl;
	int pCount = 0;
	bool foundClosing = false;
	while(!foundClosing && param[end])
	{
		if((param[end] == ',' || param[end] == ')') && end > start && pCount == 0)
		{
			params.push_back(param.substr(start,end-start));
			//std::cout<<*params.rbegin()<<std::endl;
			start = end;
			while(param[start] == ',' || (!isalpha(param[start]) && param[start] != '-' && !(param[start] < 58 && param[start > 47])))
			{
				start++;
				end++;
			}
		}

		if(pCount == 0 && param[end] == ')')
		{
			foundClosing = true;
		}
		else if(param[end] == '(')
			pCount++;
		else if(param[end] == ')')
			pCount--;
		end++;
	}
	return ++end;
}

Object::Object(std::string id,std::string expression,std::string param):id(id)
{
	setParams(param);
	int start = 0;

	expression = expression + '\n';
	//std::cout<<expression<<std::endl;
	for(int i = 0;i < expression.length();i++)
	{
		if(expression[i] == '\n')
		{
			instructions.push_back(expression.substr(start,i-start));
			start = ++i;
		}
	}
}
Object::Object()
{
	this->id = "";
}
Object::~Object() {
	// TODO Auto-generated destructor stub
}

