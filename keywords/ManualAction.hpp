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
class ManualAction: public Keyword {
public:
	ManualAction(AscalExecutor *runtime, std::unordered_map<std::string,Object> *memory, std::map<std::string,setting<bool> > *boolsettings):
	Keyword(runtime, memory, boolsettings)
	{
		this->keyWord = "man";
	}
	std::string action(AscalFrame<double>* frame) override
	{
		uint32_t startIndex = 0;
	    while(frame->exp[startIndex] == ' ' || frame->exp[startIndex] == ':')
	        startIndex++;
	    std::string keyWordName = frame->exp.substr(startIndex,frame->exp.find(';',startIndex)-startIndex);
		std::stringstream fullPath;
		fullPath<<runtime->commandLineParams.argv[0]<<PATH_SEPARATOR<<"KeywordManuals"<<PATH_SEPARATOR<<keyWordName<<".txt";
		if(*(*boolsettings)["o"])
			std::cout<<"Searching for man page at path: "<<fullPath.str()<<"\n";
	    std::ifstream inputFile;
	    inputFile.open(fullPath.str());
	    if(!inputFile)
	    {
	    	std::stringstream error;
	    	error<<"Could not locate keyword man page for keyword: "<<keyWordName<<"\n";
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
	    return MAX;
	}
};

#endif /* KEYWORDS_MANUALACTION_HPP_ */
