/*
 * PrintStringAction.hpp
 *
 *  Created on: Jan 16, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_PRINTSTRINGACTION_HPP_
#define KEYWORDS_PRINTSTRINGACTION_HPP_

#include "../Keyword.hpp"
class PrintStringAction: public Keyword {
public:
	PrintStringAction(AscalExecutor *runtime, std::unordered_map<std::string,Object> *memory, std::map<std::string,setting<bool> > *boolsettings):
	Keyword(runtime, memory, boolsettings)
	{
		this->keyWord = "printTxt";
	}
	std::string action(AscalFrame<double>* frame) override
	{
	        const int startOfPrint = frame->exp.find("\"",frame->index)+1;
	        const int endOfPrint = frame->exp.find("\"",startOfPrint);
	        if(endOfPrint <= startOfPrint)
	            throw std::string("Error, no terminating \" in print string command");

	        std::string result;
	        if(endOfPrint != -1)
	        {
	            result = frame->exp.substr(startOfPrint,endOfPrint-startOfPrint);
	            int index = 0;
	            SubStr subexp("",0,0);
	            while(result[index] && result[index] != '\"')
	            {
	                if(result[index] == '(')
	                {
	                    SubStr subexp = ParsingUtil::getExpr(result,index,runtime->ascal_cin,'(',')',';');
	                    std::string value = ParsingUtil::to_string(runtime->callOnFrame(frame, subexp.data));
	                    std::string first = result.substr(0,index);
	                    std::string last = result.substr(index+subexp.end-2,frame->exp.size());

	                    result = first+value+last;
	                    index = first.size()+value.size()-1;
	                }
	                else if(result.size()-index>3 && result[index] == 'e' &&
	                        result[index+1] == 'n' && result[index+2] == 'd' && result[index+3] == 'l')
	                {
	                    result = result.substr(0,index)+'\n'+result.substr(index+4,result.size());
	                }
	                index++;
	            }
	            index++;
	        }
	        else
	        {
	            result = "";
	        }
	        std::cout<<result;
	        return MAX;
	}
};

#endif /* KEYWORDS_PRINTSTRINGACTION_HPP_ */
