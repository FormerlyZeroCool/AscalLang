/*
 * Keyword.cpp
 *
 *  Created on: Dec 9, 2020
 *      Author: andrewrubinstein
 */

#include "Keyword.hpp"
#include "AscalExecutor.hpp"

    Keyword::Keyword(AscalExecutor &runtime):
    runtime(runtime), operation(noop){}


	void Keyword::compile(CompilationContext &ctx)
    {
        ctx.target.append(this->operation);
        ctx.src_index += this->keyWord.size();
    }

	void OpKeyword::compile(CompilationContext &ctx)
    {
	    SubStrSV exp = ParsingUtil::getFollowingExprSV(ctx.source, ctx.src_index, keyWord);
            std::cout<<"param op: "<<exp.data<<"\n";
	    ParsingUtil::ParseStatementList(exp.data,0,params);
        for(uint32_t i = 0; i < params.statements.size(); i++)
        {
            ctx.target.compileParams(params.statements[i].data, runtime, ctx);
        }
        Keyword::compile(ctx);
        ctx.src_index = params.end;
    }


    const std::string Keyword::MAX = std::to_string(std::numeric_limits<double>::max());
    Keyword::~Keyword() {}
