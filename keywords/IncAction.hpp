/*
 * SinAction.hpp
 *
 *  Created on: Jan 18, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_INCACTION_HPP_
#define KEYWORDS_INCACTION_HPP_

#include "../Keyword.hpp"
static inline void incAction(KeywordExecutionContext ctx) 
{
    ctx.index() += Keyword::opcodeSize();
    Object *obj = nullptr;
    ctx.getData(obj);
    ctx.index() += sizeof(Object*);
    obj->setDouble(obj->getDouble() + 1);
    //std::cout<<"Loading data for var id: "<<index<<"\n";
}
static inline void incLocalAction(KeywordExecutionContext ctx) 
{
    ctx.index() += Keyword::opcodeSize();
    uint64_t index = -1;
    ctx.getData(index);
    ctx.index() += sizeof(uint64_t);
    //std::cout<<"Loading data for var id: "<<index<<"\n";
    ctx.frame().localMemory[index].data.number++;
}
class IncAction: public OpKeyword {
public:
	IncAction(AscalExecutor &runtime):
	OpKeyword(runtime)
	{
		this->keyWord = "inc";
		this->operation = incAction;
	}
    void compile(CompilationContext &ctx) override
    {    
        ctx.src_index += keyWord.size();
	    SubStrSV varName = ParsingUtil::getVarNameSV(ctx.source, ++ctx.src_index);
        ctx.src_index = varName.end + 1;
        const auto localIt = ctx.localMemory.find(varName.data);
        if(localIt != ctx.localMemory.end())
        {
            CompilationContext::LocalRecord obj = (*localIt).getValue();
		    this->operation = incLocalAction;
            ctx.target.append(this->operation);
            ctx.target.append((uint64_t) obj.stack_index);
        }
        else
        {
            const auto globalIt = ctx.runtime.memory.find(varName.data);
            if(globalIt != ctx.runtime.memory.end())
            {
                const Object *obj = (*globalIt).getValue();
                std::cout<<"compiling inc global\n";
		        this->operation = incAction;
                ctx.target.append(this->operation);
                ctx.target.append(obj);
            }
        }
    }
};

#endif /* KEYWORDS_SINACTION_HPP_ */
