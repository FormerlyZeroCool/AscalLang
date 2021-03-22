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
SubStrSV ParsingUtil::getVarNameSV(const std::string &s,uint32_t &index)
{
    int begin = index;
    while(s.length() > index && !isalpha(s[index]))
    {
        begin++;
        index++;
    }
    while(s.length()>index && (isalpha(s[index]) || ParsingUtil::isNumeric(s[index])))
    {
    	index++;
    }
    index = index>s.length()?s.length():index-1;
    begin = begin>s.length()?s.length():begin;
    return SubStrSV(string_view(s, begin, index+1), begin, index);
}
SubStrSV ParsingUtil::getVarNameSV(string_view s,uint32_t &index)
{
    int begin = index;
    while(s.length() > index && !ParsingUtil::isalpha(s[index]))
    {
        begin++;
        index++;
    }
    while(s.length()>index && (ParsingUtil::isalpha(s[index]) || ParsingUtil::isNumeric(s[index])))
    {
    	index++;
    }
    index = index>s.length()?s.length():index-1;
    begin = begin>s.length()?s.length():begin;
    return SubStrSV(string_view(s,begin,index+1), begin, index);
}
SubStr ParsingUtil::getNewVarName(const std::string &data)
{
    int index = data.find("let")<5000 || data.find("const")<5000?data.find("t ")+1:0;
    return ParsingUtil::getVarName(data,index);
}
SubStrSV ParsingUtil::getFollowingExprSV(const string_view exp, uint32_t startIndex,const string_view id, char start, char end)
{
	int index = startIndex+id.length();
	while(exp[index] == ' ')
		index++;
	SubStrSV expr = ParsingUtil::getExprInStringSV(exp,index, start, end, ';');
    return expr;
}
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
