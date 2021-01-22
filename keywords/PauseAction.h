/*
 * PauseAction.h
 *
 *  Created on: Jan 16, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_PAUSEACTION_H_
#define KEYWORDS_PAUSEACTION_H_

#include "../Keyword.hpp"
class PauseAction: public Keyword {
public:
	PauseAction(AscalExecutor *runtime, std::unordered_map<std::string,Object> *memory, std::map<std::string,setting<bool> > *boolsettings):
	Keyword(runtime, memory, boolsettings)
	{
		this->keyWord = "pause";
	}
	std::string action(AscalFrame<double>* frame) override
	{
	    static std::string s;
	    std::cout<<"Paused press enter to continue.\n";

		std::streambuf* currentBuffer = std::cin.rdbuf();
	    std::cin.rdbuf(runtime->stream_buffer_cin);
	    std::getline(std::cin,s);
	    std::cin.rdbuf(currentBuffer);
	    return 'a'+frame->exp.substr(frame->index+5,frame->exp.size());
	}
};

#endif /* KEYWORDS_PAUSEACTION_H_ */
