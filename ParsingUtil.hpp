/*
 * ParsingUtil.hpp
 *
 *  Created on: Jan 11, 2021
 *      Author: andrewrubinstein
 */

#ifndef PARSINGUTIL_HPP_
#define PARSINGUTIL_HPP_
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include "Calculator.hpp"
#include "SubStr.hpp"
#include "string_view.hpp"

class AscalExecutor;
static const std::string MAX = std::to_string(std::numeric_limits<double>::max());
template <typename string_type>
bool isObj(string_type &s);
class ParsingUtil {
private:
public:
	ParsingUtil();
	static char isalpha(char c) {
	    return ((unsigned char)(c | 32U) - 97U) < 26U;
	}
	static bool isNumeric(char c)
	{
	  return (c - 48U < 10U);
	}
	static char toLower(char c)
	{
		return c|32U;
	}
	template <typename string>
	static double getNextDoubleS(const string &data,int &index);
	template <typename string>
	static double getNextDouble(const string &data,int &index);
	static std::string to_string(double input);
	static void to_string(double input, std::string &data)
	{
	    int offsetFromEnd = 0;
	    bool brek = true;
	    data = std::to_string(input);
	    brek = (data[data.length()-offsetFromEnd-1] == '0');
	    data[data.length()-offsetFromEnd-1] *= brek == true;
	    offsetFromEnd += brek;
	    brek = brek && (data[data.length()-offsetFromEnd-1] == '0');
	    data[data.length()-offsetFromEnd-1] *= brek == true;
	    offsetFromEnd += brek;
	    brek = brek && (data[data.length()-offsetFromEnd-1] == '0');
	    data[data.length()-offsetFromEnd-1] *= brek == true;
	    offsetFromEnd += brek;
	    brek = brek && (data[data.length()-offsetFromEnd-1] == '0');
	    data[data.length()-offsetFromEnd-1] *= brek == true;
	    offsetFromEnd += brek;
	    brek = brek && (data[data.length()-offsetFromEnd-1] == '0');
	    data[data.length()-offsetFromEnd-1] *= brek == true;
	    offsetFromEnd += brek;
	    brek = brek && (data[data.length()-offsetFromEnd-1] == '0');
	    data[data.length()-offsetFromEnd-1] *= brek == true;
	    offsetFromEnd += brek;
	    offsetFromEnd += (data[data.length()-offsetFromEnd-1] == '.');
	    data[data.length()-offsetFromEnd-1] *= brek == true;
	}
	template <typename string>
	static SubStr getExpr(const string &data,int startingIndex, std::istream &ascal_cin, char opening = '{',char closing = '}',char lineBreak = ';');
	template <typename string>
	static SubStr getFollowingExpr(string &exp, uint32_t startIndex,const std::string &id, char start = '(', char end = ')');
	template <typename string>
	static SubStr getFollowingExpr(string &exp, uint32_t startIndex, const std::string &&id, char start = '(', char end = ')');
	template <typename string>
	static SubStr getExprInString(const string &data,int index,char opening,char closing,char lineBreak);
	template <typename string>
	static SubStrSV getExprInStringSV(const string &line,uint32_t index,char opening = '{',char closing = '}',char lineBreak = ';');
	static SubStrSV getFollowingExprSV(const string_view exp, uint32_t startIndex,const string_view id, char start = '(', char end = ')');
	static bool containsOperator(std::string s);
	template <typename string>
	static SubStr getCodeBlock(string &exp, int index, std::istream &in);
	template <typename string>
	static SubStr getVarName(string &s,int index);
	template <typename string>
	static SubStr getVarName(const string &s,int index);
	static SubStrSV getVarNameSV(const std::string &s,uint32_t &index);
	static SubStrSV getVarNameSV(string_view s,uint32_t &index);
	static SubStr getNewVarName(const std::string &data);
	//Generic string stuff
	template <typename string1, typename string2>
	static bool cmpstr(const string1 &s1,const string2 &s2);
	template <typename string1, typename string2>
	static bool cmpstr(const string1 &s1,const string2 &&s2);
	template <typename string>
	static bool hasChar(const string &dat,const char &c);
	template <typename string>
	static bool isDouble(string &exp);
	template <typename string_type>
	static bool isObj(string_type &s)
	{
		uint32_t i = 0;
		while(s[i] && s[++i] == ' '){}
		bool obj = isalpha(s[--i]);
		i++;
		for(;obj && i < s.size(); i++)
		{
			obj = (isalpha(s[i]) || isNumeric(s[i]) || s[i] == '.' ||
					s[i] == '[' || s[i] == ']' || s[i] == '"' || s[i] == ';');
		}
		return obj;
	}
	static std::vector<std::string> split(std::string &s, std::string &&delimiter)
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
	template <typename in_string_type, typename out_string_type>
	static SubStr getVarNameOrNum(const in_string_type &s,int index)
	{
	    int begin = index;
	    int count = 0;
	    while(s[index] && !isalpha(s[index]) && !isNumeric(s[index]))
	    {
	        begin++;
	        index++;
	    }
	    while(s.length()>index && !Calculator<double>::isOperator(s[index]) && (isalpha(s[index]) || isNumeric(s[index])))
	    {
	        index++;
	        count++;
	    }
	    index--;
	    out_string_type result = s.substr(begin>s.length()?s.length():begin,count);
	    return SubStrSV(result,begin, begin+result.length()-1);
	}
	static bool firstChar(std::string &s, char c)
	{
		static uint32_t it;
		for(it = 0; s[it] == ' '; it++){}

		return s[it] == c;
	}
	static std::string ts(string_view s)
	{
		return s.str();
	}
	static std::string& ts(std::string &s)
	{
		return s;
	}
};

template <typename string>
SubStr ParsingUtil::getExpr(const string &data,int index, std::istream &ascal_cin,char opening,char closing,char lineBreak)
{
    int count = 0;
    int openingCount = 0;
    int maxOpeningCount = 0;
    SubStr block("",0,0);
    std::vector<char> result;
    if(data.length()<1024)
        result.reserve(1024);
    else
        result.reserve(data.length());
    string_view sline(data);
    std::string line = sline.str();

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
	block.end = result.size();
    return block;
}






template <typename string>
SubStr ParsingUtil::getVarName(string &s,int index)
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
    string_view result(s.substr(begin>s.length()?s.length():begin,index - begin));
    index--;
    return SubStr(result,begin,begin + result.length()-1);
}
template <typename string>
SubStr ParsingUtil::getVarName(const string &s,int index)
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
    std::string result = ts(s.substr(begin>s.length()?s.length():begin,index - begin));
    index--;
    return SubStr(result,begin,begin + result.length()-1);
}
template <typename string>
SubStr ParsingUtil::getFollowingExpr(string &exp, uint32_t index,const std::string &&id, char start, char end)
{
	return ParsingUtil::getFollowingExpr(exp, index, id, start, end);
}
template <typename string>
SubStr ParsingUtil::getFollowingExpr(string &exp, uint32_t startIndex,const std::string &id, char start, char end)
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

template <typename string>
SubStr ParsingUtil::getExprInString(const string &data,int index,char opening,char closing,char lineBreak)
{
    int count = 0;
    int openingCount = 0;
    int maxOpeningCount = 0;
    std::vector<char> result;
    if(data.length()<1024)
        result.reserve(1024);
    else
        result.reserve(data.length());
    std::string line = string_view(data).str();

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

template <typename string>
SubStrSV ParsingUtil::getExprInStringSV(const string &line,uint32_t index,char opening,char closing,char lineBreak)
{
    uint32_t i = index;
    uint8_t subLevel = 0;
    while(i < line.size() && line[i] != lineBreak && line[i] != opening)
    {
        i++;
    }
    subLevel += (line[i]==opening);
    i += subLevel;
    const bool isBlock = subLevel;
    while(i < line.size() && (subLevel != 0 || (!isBlock && line[i] != lineBreak && line[i] != '\n')))
    {
        subLevel += (line[i]==opening) - (line[i] == closing);
        ++i;

    }
    if(line.size() == i && subLevel != 0){
    	std::stringstream s;
    	s<<"Error no closing: "<<closing;
    	throw s.str();
    }
    return SubStrSV(string_view(line, index, i), index, i);
}

static char periodCount;
static bool isADouble;
template <typename string>
bool ParsingUtil::isDouble(string &exp)
{
    isADouble = true;
    periodCount = 0;
    for(uint16_t i = (exp[0] == '-'); isADouble && i < exp.size(); i++)
    {
        //to avoid branching I'm doing boolean arithmetic to determine if a string is a double
        periodCount += (exp[i] == '.');
        isADouble = ((exp[i] >= '0' && exp[i] <= '9' ) || (periodCount == 1 && exp[i] == '.') ||
                     (exp[i] == ';' && (exp[i+1] == 0 || exp[i+2] == 0|| exp[i+3] == 0) ));
    }
    return isADouble;
}

template <typename string1, typename string2>
bool ParsingUtil::cmpstr(const string1 &s1,const string2 &&s2)
{
	return cmpstr(s1, s2);
}

template <typename string1, typename string2>
bool ParsingUtil::cmpstr(const string1 &s1,const string2 &s2)
{
    bool isEqual = true;
    int i = 0;
    while(isEqual && i < s1.size() && i < s2.size())
    {
        if(s1[i] != s2[i])
        {
            isEqual = false;
        }
        i++;
    }

    //if(s1[i] != s2[i])
    //{
    //    isEqual = false;
    //}
    return isEqual;
}

template <typename string>
SubStr ParsingUtil::getCodeBlock(string &exp, int index, std::istream &cin_ascal)
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

template <typename string>
double ParsingUtil::getNextDoubleS(const string &data,int &index)
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
template <typename string>
double ParsingUtil::getNextDouble(const string &data,int &index)
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
#endif /* PARSINGUTIL_HPP_ */
