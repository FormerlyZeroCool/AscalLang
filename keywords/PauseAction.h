/*
 * PauseAction.h
 *
 *  Created on: Jan 16, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_PAUSEACTION_H_
#define KEYWORDS_PAUSEACTION_H_

#include "../Keyword.hpp"
static void pause(KeywordExecutionContext ctx)
{
	static std::string s;
	std::cout<<"Paused press enter to continue.\n";
	std::streambuf* currentBuffer = std::cin.rdbuf();
	std::cin.rdbuf(ctx.runtime().stream_buffer_cin);
	std::cin.get();
	std::cin.rdbuf(currentBuffer);
	ctx.frame().index += Keyword::opcodeSize();
}
class PauseAction: public StKeyword {
public:
	PauseAction(AscalExecutor &runtime):
	StKeyword(runtime)
	{
		this->keyWord = "pause";
		this->operation = pause;
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
