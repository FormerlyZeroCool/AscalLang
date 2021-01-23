/*
 * ParsingUtil.cpp
 *
 *  Created on: Jan 11, 2021
 *      Author: andrewrubinstein
 */

#include "ParsingUtil.hpp"

ParsingUtil::ParsingUtil() {
	// TODO Auto-generated constructor stub

}


template <typename string_type>
bool isObj(string_type &s)
{
	uint32_t i = 0;
	while(s[i] && s[++i] == ' '){}
	bool obj = isalpha(s[i]);
	for(;obj && i < s.size(); i++)
	{
		obj = (isalpha(s[i]) || isNumeric(s[i]) || s[i] == '.' ||
				s[i] == '[' || s[i] == ']' || s[i] == '"' || s[i] == ';');
	}
	return obj;
}
std::string ParsingUtil::to_string(double input)
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
double ParsingUtil::getNextDouble(const std::string &data,int &index)
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
  if(data[index] == '-' && (Calculator<double>::isOperator(previous) || index == 0))
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
    else if(!Calculator<double>::isOperator(previous) && index != 0)
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
SubStr ParsingUtil::getVarName(const std::string &s,int index)
{
    int begin = index;
    while(s[index] && !isalpha(s[index]))
    {
        begin++;
        index++;
    }
    while(s.length()>index && (isalpha(s[index]) || ParsingUtil::isNumeric(s[index])))
    {
        index++;
    }
    std::string result = s.substr(begin>s.length()?s.length():begin,index - begin);
    index--;
    return SubStr(result,begin,begin + result.length()-1);
}
SubStr ParsingUtil::getNewVarName(const std::string &data)
{
    int index = data.find("let")<5000 || data.find("const")<5000?data.find("t ")+1:0;
    return ParsingUtil::getVarName(data,index);
}
SubStr ParsingUtil::getExpr(const std::string &data,int index, std::istream &ascal_cin,char opening,char closing,char lineBreak)
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
        while(line[index] && !ParsingUtil::isNumeric(line[index]) && !isalpha(line[index]) && !Calculator<double>::isOperator(line[index]) &&
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
        		getline(ascal_cin, line);
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
SubStr ParsingUtil::getFollowingExpr(std::string &exp, uint32_t index,const std::string &&id, char start, char end)
{
	return ParsingUtil::getFollowingExpr(exp, index, id, start, end);
}
SubStr ParsingUtil::getFollowingExpr(std::string &exp, uint32_t startIndex,const std::string &id, char start, char end)
{
	int index = startIndex+id.length();
	while(exp[index] == ' ')
		index++;
	SubStr expr = ParsingUtil::getExprInString(exp,index, start, end,'\1');
	index += expr.data.length()>3?expr.data.length()-3:0;
	expr.end = index;
    index = index+id.length();
    return expr;
}

SubStr ParsingUtil::getExprInString(const std::string &data,int index,char opening,char closing,char lineBreak)
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
        while(line[index] && !isNumeric(line[index]) && !isalpha(line[index]) && !Calculator<double>::isOperator(line[index]) &&
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

SubStr ParsingUtil::getCodeBlock(std::string &exp, int index, std::istream &cin_ascal)
{
    SubStr codeBlock = getExpr(exp,index, cin_ascal);
    if(codeBlock.data.size() == 0)
    {
        codeBlock.loadedNew = true;
    	std::string nextLine;
        while(cin_ascal && codeBlock.data.size() == 0)
        {
        	getline(cin_ascal,nextLine);
        	codeBlock = getExpr(nextLine, 0, cin_ascal);
        }
    }
    return codeBlock;
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

bool ParsingUtil::containsOperator(std::string s)
{
    bool contains = false;
    int i = 0;
    while(!contains && s[i])
    {
        contains = Calculator<double>::isOperator(s[i]);
        i++;
    }
    return contains;
}
static char periodCount;
static bool isADouble;
bool ParsingUtil::isDouble(std::string &exp)
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

bool ParsingUtil::cmpstr(const std::string &s1,const std::string &&s2)
{
	return cmpstr(s1, s2);
}

bool ParsingUtil::cmpstr(const std::string &s1,const std::string &s2)
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

