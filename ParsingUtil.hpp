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
#include "Calculator.hpp"
#include "AscalFrame.hpp"
#include "SubStr.hpp"

class AscalExecutor;
static const std::string MAX = std::to_string(std::numeric_limits<double>::max());
template <typename string_type>
bool isObj(string_type &s);
class ParsingUtil {
private:
public:
	ParsingUtil();
	static bool isNumeric(char c)
	{
	  return (c >= 48 && c<58);
	}
	static char toLower(char c)
	{
		return c|32;
	}
	static double getNextDouble(const std::string &data,int &index);
	static std::string to_string(double input);
	static SubStr getExpr(const std::string &data,int startingIndex, std::istream &ascal_cin, char opening = '{',char closing = '}',char lineBreak = ';');
	static SubStr getFollowingExpr(AscalFrame<double>* frame,const std::string &id, char start = '(', char end = ')');
	static SubStr getFollowingExpr(AscalFrame<double>* frame,const std::string &&id, char start = '(', char end = ')');
	static SubStr getExprInString(const std::string &data,int index,char opening,char closing,char lineBreak);
	static bool containsOperator(std::string s);
	static SubStr getCodeBlock(AscalFrame<double> *frame, int index, std::istream &cin_ascal);
	static SubStr getVarName(const std::string &s,int index);
	static SubStr getNewVarName(const std::string &data);
	//Generic string stuff
	static bool cmpstr(const std::string &s1,const std::string &s2);
	static bool cmpstr(const std::string &s1,const std::string &&s2);
	static bool hasChar(const std::string &dat,const char &c);
	static bool isDouble(std::string &exp);
	template <typename string_type>
	static bool isObj(string_type &s)
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
	static SubStr getVarNameOrNum(const std::string &s,int index)
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
	    std::string result = s.substr(begin>s.length()?s.length():begin,count);
	    return SubStr(result,begin,begin + result.length()-1);
	}

	static bool firstChar(std::string &s, char c)
	{
		static uint32_t it;
		for(it = 0; s[it] == ' '; it++){}

		return s[it] == c;
	}
	static std::string printMemory(std::map<std::string,Object*> &memory,std::string delimiter,bool justKey = true,
	        std::string secondDelimiter = "\n")
	{
	    std::string s;
	    if(justKey)
	        for(auto &[key,value]:memory)
	            s+=key+delimiter;
	    else
	        for(auto &[key,value]:memory)
	            s+=key+delimiter+value->instructionsToString()+secondDelimiter;
	    return s.substr(0,s.size()-secondDelimiter.size());
	}
	static std::string printMemory(std::unordered_map<std::string,Object> &memory,std::string delimiter,bool justKey = true,
	        std::string secondDelimiter = "\n")
	{
	    std::string s;
	    if(justKey)
	        for(auto &[key,value]:memory)
	            s+=key+delimiter;
	    else
	        for(auto &[key,value]:memory){
	            std::string instr = value.instructionsToString();
	            s+=key+delimiter+instr+secondDelimiter;
	        }
	    return s.substr(0,s.size()-secondDelimiter.size());
	}
};

#endif /* PARSINGUTIL_HPP_ */
