/*
 * WhenAction.hpp
 *
 *  Created on: Jan 18, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_WHENACTION_HPP_
#define KEYWORDS_WHENACTION_HPP_

#include "../Keyword.hpp"
class WhenAction: public OpKeyword {
	private:
	const uint32_t bufSize = 8192;
	char buffer[8192];
public:
	WhenAction(AscalExecutor &runtime):
	OpKeyword(runtime)
	{
		this->keyWord = "when";
	}
	void action(AscalFrame<double>* frame) override
	{
	    //parse expression, start from substr when go up until substr end is found
	    //extract substring, and save the other parts of the expression
	    //find when evaluate expression between it and then
	    //if the expression evaluates to anything other than 0
	    //then extract the expression proceeding the then statement
	    //frame->index = 0;
		static const int invalidIndex = 2000000000;
	    const int startIndex = frame->index;

        const string_view exp = string_view(frame->exp);
	    int endIndex = frame->index;
	    /*this block locates the end keyword for the ascal when block*/{
	    int i = endIndex;
	    uint8_t qCount = 0;
	    while(i < frame->exp.length()-2)
	    {
	    	qCount = ((frame->exp[i] == '\"') + qCount) & 1;
	    	if(frame->exp[i] == 'e' && frame->exp[i+1] == 'n' && frame->exp[i+2] == 'd' && qCount == 0)
	    	{
	    		endIndex = i;
	    		i = invalidIndex;
	    	}
	    	i++;
	    }
	    }
	    //string_view endOfExp = string_view(frame->exp).substr(endIndex+3,frame->exp.length());
	    string_view value;
	    int index = startIndex + 4;
	    //should always start after when is finished to build boolean expression

	    int thenIndex;
	    int whenIndex = startIndex;
	    double boolExpValue;
	    int elseIndex = frame->exp.find("else",frame->index);
	    elseIndex = elseIndex==-1?invalidIndex:elseIndex;
	    int lastThen = 0;
	    do {
	        thenIndex = frame->exp.find("then",index);
	        //Parsing boolean expression
	        while(frame->exp[index] == ' ' && index < endIndex && index < thenIndex)
	        {
	            index++;
	        }
	        uint32_t startOfBoolExp = index;
	        while(frame->exp.size() > index && index < endIndex && index < thenIndex)
	        {
	            index++;
	        }
	        string_view booleanExpression = exp.substr(startOfBoolExp,index-startOfBoolExp);
	        if(booleanExpression.size() == 0)
	        {
	            throw std::string("Error no boolean expression provided in when.\n");
	        }

		    if(*runtime.boolsettings["o"])
	        {
	            std::cout<<"Executing Boolean Expression: "<<booleanExpression<<'\n';
	        }
	        try{
	            boolExpValue = runtime.callOnFrame(frame,booleanExpression);
	        }
	        catch(std::string &exception)
	        {
	            throw std::string("In When Action Calculating BooleanExpression: "+booleanExpression.str()+"\n"+exception);
	        }
	    //false case simply set the index to the next instance of when+4
	    //and repeat until true, or at end of case when
	        if(boolExpValue == 0)
	        {
	            index = frame->exp.find("when",index) + 5;
	            whenIndex = index - 5;
	            if(whenIndex == -1 && elseIndex != invalidIndex)
	            {
	                value = ParsingUtil::getExprInStringSV(exp.substr(elseIndex+4,endIndex-(elseIndex+4)), 0, '{', '}', ';').data;
	                index = endIndex;
	            }

	        }
	    //true case get sub expression associated with this when return value exit loop
	        else
	        {
	            index += 5;
	            thenIndex = frame->exp.find("when",index);
	            thenIndex = thenIndex==-1?endIndex+1:thenIndex;
	            value = ParsingUtil::getExprInStringSV(exp.substr(index,std::min(std::min(endIndex,thenIndex),elseIndex)-index), 0, '{', '}', ';').data;
	        }
	        lastThen = thenIndex;
	    } while(frame->exp[index] && boolExpValue == 0 && index < endIndex);
	    if(*runtime.boolsettings["o"])
	    {
	        std::cout<<"Executing Branch: "<<value<<" Params: "<<AscalExecutor::printMemory(*frame->getParamMemory()," = ",false,"|");
	        std::cout<<"\n";
	    }
	    if(value.length()-1 > bufSize)
	    {
		    std::string s = (value).substr(0,value.length()-1).str();
		    index = endIndex+3-s.length();
		    frame->index = index;
		    uint32_t i = 0;
		    for(; index < endIndex+3; index++){
		    	frame->exp[index] = s[i++];
		    }
	    }
	    else
	    {

		    string_view s = (value).substr(0,value.length()-1);
		    index = endIndex+3-s.length();
		    frame->index = index;
		    uint32_t i = 0;
		    for(short j = 0; j < endIndex+3-index; j++)
		    	buffer[j] = s[j];
		    for(; index < endIndex+3; index++){
		    	frame->exp[index] = buffer[i++];
		    }
	    }
	}
};

#endif /* KEYWORDS_WHENACTION_HPP_ */
