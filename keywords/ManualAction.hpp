/*
 * ManualAction.hpp
 *
 *  Created on: Jan 21, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_MANUALACTION_HPP_
#define KEYWORDS_MANUALACTION_HPP_

#include "../Keyword.hpp"
#include "../CrossPlatform.hpp"
#include <sstream>
#include <iostream>
#include <fstream>
class ManualAction: public StKeyword {
public:
	ManualAction(AscalExecutor &runtime):
		StKeyword(runtime)
	{
		this->keyWord = "man";
	}
	void action(AscalFrame<double>* frame) override
	{
		uint32_t startIndex = this->keyWord.size();
	    while(frame->exp[startIndex] == ' ' || frame->exp[startIndex] == ':')
	        startIndex++;
	    string_view keyWordName = frame->exp.substr(startIndex,frame->exp.find(';',startIndex)-startIndex);
		std::stringstream fullPath;
		fullPath<<runtime.commandLineParams.argv[0]<<PATH_SEPARATOR<<"KeywordManuals"<<PATH_SEPARATOR<<keyWordName<<".txt";
	    if(*runtime.boolsettings["o"])
			std::cout<<"Searching for man page at path: "<<fullPath.str()<<"\n";
	    std::ifstream inputFile;
	    inputFile.open(fullPath.str());
	    if(!inputFile)
	    {
	    	std::stringstream error;
	    	error<<"Error Could not locate keyword man page for keyword: "<<keyWordName;
	    	throw error.str();
	    }
	    while(true)
	    {
		    std::string line;
	    	getline(inputFile , line);
	    	if(!inputFile)
	    	    break;
	    	std::cout<<line<<'\n';
	    }
	}
};

#endif /* KEYWORDS_MANUALACTION_HPP_ */
