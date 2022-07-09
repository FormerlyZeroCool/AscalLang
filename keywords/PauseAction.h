/*
 * PauseAction.h
 *
 *  Created on: Jan 16, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_PAUSEACTION_H_
#define KEYWORDS_PAUSEACTION_H_

#include "../Keyword.hpp"
class PauseAction: public OpKeyword {
public:
	PauseAction(AscalExecutor &runtime):
	OpKeyword(runtime)
	{
		this->keyWord = "pause";
	}
	void action(AscalFrame<double>* frame) override
	{
	    static std::string s;
	    std::cout<<"Paused press enter to continue.\n";

		std::streambuf* currentBuffer = std::cin.rdbuf();
	    std::cin.rdbuf(runtime.stream_buffer_cin);
	    std::getline(std::cin,s);
	    std::cin.rdbuf(currentBuffer);
	    frame->index += this->keyWord.size();
	}
};

#endif /* KEYWORDS_PAUSEACTION_H_ */
