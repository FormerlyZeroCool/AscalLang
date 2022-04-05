/*
 * FileHandler.hpp
 *
 *  Created on: Jan 17, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_FILEHANDLER_HPP_
#define KEYWORDS_FILEHANDLER_HPP_

#include "../AscalExecutor.hpp"
#include "../ParsingUtil.hpp"
#include "../CrossPlatform.hpp"

class FileHandler {
private:
public:
	FileHandler();
	static void loadFile(AscalExecutor &runtime, string_view expr,int startIndex, std::istream &inputStream)
	{
	    std::ifstream inputFile;
	    while(expr[startIndex] == ' ' || expr[startIndex] == ':')
	        startIndex++;
		auto endIndex = expr.find(';');
		endIndex += (endIndex<0) * (expr.length() + 1);
	    std::string filePath = expr.substr(startIndex,endIndex-startIndex).str();
	    inputFile.open(filePath);
	    if(!inputFile)
	    {
	        inputFile.open(filePath+".asl");
	    }
	    if(!inputFile)
	    {
	        std::vector<char> data;
	        for(char &c:filePath)
	        {
	            if(c == '.')
	                c = (PATH_SEPARATOR);
	        }
	        if(filePath.size() > 4 &&
	        		filePath[filePath.size()-4] == PATH_SEPARATOR && ParsingUtil::toLower(filePath[filePath.size()-3]) == 'a' &&
					ParsingUtil::toLower(filePath[filePath.size()-2]) == 's' && ParsingUtil::toLower(filePath[filePath.size()-1]) == 'l')
				filePath[filePath.size()-4] = '.';
	        inputFile.open(filePath);
	        if(!inputFile)
	        {
	            inputFile.open(filePath+".asl");
	        }
	        if(!inputFile)
	        {
	        	inputFile.open(filePath.substr(0, filePath.size()-1)+".asl");
	        }
	    }
	    if(!inputFile)
	    {
	        throw std::string("Malformed path: "+filePath);
	    }
	    if(*runtime.boolsettings["o"])
	    {
	    	std::cout<<"Loading file: "<<filePath<<"\n";
	    }
	    std::streambuf* cinrdbuf = inputStream.rdbuf();
	    inputStream.rdbuf(inputFile.rdbuf());
	    FunctionFrame<double>* calledFunctionMemory = new FunctionFrame<double>(runtime, runtime.memMan);
	    calledFunctionMemory->setIsDynamicAllocation(false);
	    while(true)
	    {
	    	std::string line;
	        getline(inputFile, line);
	        calledFunctionMemory->exp = line;
	    	if(!inputFile)
	    		break;
	        //reset so it is like we are executing a new frame with shared memory, and if/else flag state
	        calledFunctionMemory->index = 0;
	        calledFunctionMemory->level = 0;
	        calledFunctionMemory->memoPointer = 0;
	        calledFunctionMemory->setIsFirstRun(true);
	        calledFunctionMemory->setAugmented(false);
	        //finish reset
	        try{
	        	uint32_t i = 0;
	        	while(calledFunctionMemory->exp[i] && calledFunctionMemory->exp[i] == ' ')
	        		i++;
	        	if(calledFunctionMemory->exp[i] != '#')
	        	{
	        		//evaluate statement as an expression
	        		runtime.calculateExpression(calledFunctionMemory);
	        	}
	        }
	        //catch only ascal runtime exceptions, not exit signal exceptions, or anything from C++
	        catch(std::string &exception)
	        {
	            std::cerr<<exception<<std::endl;
	        }
	    }
	    delete calledFunctionMemory;
	    inputFile.close();
	    inputStream.rdbuf(cinrdbuf);
	    if(*runtime.boolsettings["o"])
	    {
	    	std::cout<<"Finished loading file: "<<filePath<<"\n";
	    }
	}

};

#endif /* KEYWORDS_FILEHANDLER_HPP_ */
