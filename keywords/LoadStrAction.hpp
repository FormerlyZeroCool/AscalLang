/*
 * LoadStrAction.hpp
 *
 *  Created on: Jan 15, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_LOADSTRACTION_HPP_
#define KEYWORDS_LOADSTRACTION_HPP_

#include "../Keyword.hpp"
class LoadStrAction: public StKeyword {
private:
    ParsedStatementList params;
public:
    LoadStrAction(AscalExecutor &runtime):
    StKeyword(runtime)
    {
        this->keyWord = "loadStr";
    }
    void action(AscalFrame<double>* frame) override
    {
        SubStr exp = ParsingUtil::getFollowingExpr(frame->exp, frame->index, keyWord);
        ParsingUtil::ParseStatementList(exp.data,0,params);
        if(params.statements.size() < 2)
            throw std::string("loadStr (<Object name>, <\"string in quotes\">");
        SubStr vns = ParsingUtil::getVarName(frame->exp, frame->index+keyWord.size()+params.statements[0].start);
        Object *obj = runtime.resolveNextExprSafe(frame, vns);
        obj->clearList();
        //parse only text between quotes
        size_t strStart = 0;
        while(params.statements[1].data.size() > strStart && params.statements[1].data[strStart++] != '\"'){}
        size_t strEnd = params.statements[1].data.length()>0?params.statements[1].data.length()-1:0;
        while(params.statements[1].data.size() > strEnd && params.statements[1].data[strEnd--] != '\"'){}
        //at this point str start, and end point to the appropriate places in the parameter
        string_view s((char*)&params.statements[1].data[0]+strStart, strEnd);
        obj->loadString(s);

        if(*runtime.boolsettings["o"])
        {
            std::cout<<"loadstr("<<s<<") = "<<'\n';
        }
    }
};

#endif /* KEYWORDS_LOADSTRACTION_HPP_ */
