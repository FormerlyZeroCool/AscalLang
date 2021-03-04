/*
 * InputAction.hpp
 *
 *  Created on: Jan 18, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_INPUTACTION_HPP_
#define KEYWORDS_INPUTACTION_HPP_

#include "../Keyword.hpp"
class InputAction: public OpKeyword {
public:
	InputAction(AscalExecutor &runtime):
	OpKeyword(runtime)
	{
		this->keyWord = "input";
	}
	void action(AscalFrame<double>* frame) override
	{
	        const int startOfPrint = frame->exp.find("\"")+1;
	        int endOfPrint = frame->exp.find("\"",startOfPrint);

	        std::string usrPrompt;
	        if(endOfPrint != -1)
	        {
	            usrPrompt = frame->exp.substr(startOfPrint,endOfPrint-startOfPrint).str();
	            int index = 0;
	            SubStr subexp("",0,0);
	            while(usrPrompt[index] && usrPrompt[index] != '\"')
	            {
	                if(usrPrompt[index] == '(')
	                {
	                    SubStr subexp = ParsingUtil::getExpr(usrPrompt,index,runtime.ascal_cin,'(',')',';');
	                    std::string value = ParsingUtil::to_string(runtime.callOnFrame(frame,subexp.data));
	                    std::string first = usrPrompt.substr(0,index);
	                    std::string last = usrPrompt.substr(index+subexp.end-3,frame->exp.size());
	                    usrPrompt = first+value+last;
	                    index = first.size()+value.size()-1;
	                }
	                else if(usrPrompt[index] == 'e' && usrPrompt.size()-index>3 &&
	                        usrPrompt[index+1] == 'n' && usrPrompt[index+2] == 'd' && usrPrompt[index+3] == 'l')
	                {
	                    usrPrompt = usrPrompt.substr(0,index)+'\n'+usrPrompt.substr(index+4,usrPrompt.size());
	                }
	                index++;
	            }
	            index++;
	        }
	        else
	        {
	            endOfPrint = frame->index+4;
	        }
	        std::string input;
	        if(runtime.commandLineParams.index+1 < runtime.commandLineParams.argc)
	        {
	            input = std::string(runtime.commandLineParams.argv[++runtime.commandLineParams.index]);
	        }
	        else
	        {
	            std::cout<<usrPrompt;
	        	std::streambuf* currentBuffer = runtime.ascal_cin.rdbuf();
	        	runtime.ascal_cin.rdbuf(runtime.stream_buffer_cin);
	            getline(runtime.ascal_cin,input);
	            runtime.ascal_cin.rdbuf(currentBuffer);
	            if(input.size()==0)
	            	throw 0;
	        }
	        frame->initialOperands.push(runtime.callOnFrame(frame,input));
		    if(*runtime.boolsettings["o"])
	          std::cout<<"Received "<<input<<" as input.\n";
	        frame->index = endOfPrint+1;

	}
};

#endif /* KEYWORDS_INPUTACTION_HPP_ */
