/*
 * PrintStringAction.hpp
 *
 *  Created on: Jan 16, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_PRINTSTRINGACTION_HPP_
#define KEYWORDS_PRINTSTRINGACTION_HPP_

#include "../Keyword.hpp"
class PrintStringAction: public StKeyword {
public:
	PrintStringAction(AscalExecutor &runtime):
	StKeyword(runtime)
	{
		this->keyWord = "printTxt";
	}
	void action(AscalFrame<double>* frame) override
	{
	        const int startOfPrint = frame->exp.find("\"",frame->index+keyWord.length()-2)+1;
	        //std::cout<<"sop: "<<startOfPrint<<"\n";
	        const int endOfPrint = frame->exp.find("\"",startOfPrint);
	        //std::cout<<"eop: "<<endOfPrint<<"\n"<<frame->exp<<"\n";

	        if(endOfPrint <= startOfPrint)
	            throw std::string("Error, no terminating \" in print string command");

	        string_view working;
	        std::stringstream result;
            uint32_t index = 0;
	        if(endOfPrint != -1)
	        {
	            working = frame->exp.substr(startOfPrint,endOfPrint-startOfPrint);
	            SubStrSV subexp(string_view(),0,0);
	            while(index < working.length() && working[index] != '\"')
	            {
	                if(working[index] == '(')
	                {
	                    SubStrSV subexp = ParsingUtil::getExprInStringSV(working,index,'(',')',';');
	                    result << ParsingUtil::to_string(runtime.callOnFrame(frame, subexp.data));
	                    index += subexp.data.size();
	                }
	                else if(working.size()-index > 3 &&
	                		working[index] == 'e' && working[index+1] == 'n' && working[index+2] == 'd' && working[index+3] == 'l')
	                {
	                    result<<'\n';
	                    index += 4; //"endl".size()
	                }
	                else
	                {
	                	result<<working[index];
	                	index++;
	                }
	            }
	            index++;
	        }
	        else
	        {
	            working = "";
	        }
	        //frame->index = startOfPrint + index;
	        std::cout<<result.str();
	}
};

#endif /* KEYWORDS_PRINTSTRINGACTION_HPP_ */
