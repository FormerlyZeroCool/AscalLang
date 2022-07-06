/*
 * WhenAction.hpp
 *
 *  Created on: Jan 18, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_WHENACTION_HPP_
#define KEYWORDS_WHENACTION_HPP_


#include "../Keyword.hpp"
class WhenAction: public OpKeyword {
	private:
    std::string thenText, elseText;
public:
	WhenAction(AscalExecutor &runtime):
	OpKeyword(runtime)
	{
		this->keyWord = "when";
        this->thenText = "then";
        this->elseText = "else";
	}
    void compileWhen(CompilationContext &ctx, string_view boolExp, uint32_t &jumpBackDistIndex, const uint32_t startOfBody, const uint32_t endOfBody) const 
    {
        operationType operation = nullptr;
        /*compile boolean expression and add jump instruction*/
        ctx.target.compileParams(boolExp, ctx.runtime, ctx);
        const double jumpDist = 0;
        operation = jumpIfFalseInlineAction;
        ctx.target.append(operation);
        const uint64_t jumpDistIndex = ctx.target.getInstructions().size();
        ctx.target.append(jumpDist);
        string_view codeblock = ctx.source.substr(startOfBody, endOfBody - startOfBody);

        //std::cout<<"compiling when bool exp: "<<boolExp<<"  body: "<<codeblock<<"\n";
        /*Compile expression to exec if true, and add jump past end instruction*/
        ctx.target.compileParams(codeblock, ctx.runtime, ctx);
        operation = jumpForwardInlineAction;
        ctx.target.append(operation);
        jumpBackDistIndex = ctx.target.getInstructions().size();
        ctx.target.append(jumpDist);
        //set jump if false jump dist to jump past codeblock to next when
        double jump = ctx.target.getInstructions().size() - jumpDistIndex;
        ctx.writeToTargetAtByte(jump, jumpDistIndex);
    }
	virtual void compile(CompilationContext &ctx) override {
        ctx.src_index += this->keyWord.size();
        uint32_t i = ctx.src_index;
        const uint32_t startOfBoolExp = i;
        i = ctx.source.find(this->thenText, i) + this->thenText.size();
        if(i == -1)
            throw std::string("Error malformed when statement, no proceeding then.");
        
        const uint32_t endOfBoolExp = i - 1;
        string_view boolExp(ctx.source.substr(startOfBoolExp, endOfBoolExp - startOfBoolExp));
        const uint32_t startOfBody = i + 1;

	    uint8_t qCount = 0;
	    uint8_t braceCount = 0;
        while(i + 2 < ctx.source.size())
        {
	        qCount = ((ctx.source[i] == '\"') + qCount) & 1;
	        braceCount += (ctx.source[i] == '{') - (ctx.source[i] == '}');
            if(braceCount == 0 && qCount == 0)
            {
                if(i + 3 < ctx.source.size())
                {
                    if(ctx.source[i] == 'w' && ctx.source[i+1] == 'h' && ctx.source[i+2] == 'e'  && ctx.source[i+3] == 'n')
                    {
                        uint32_t jumpBackDistIndex = 0;//will be assigned in compileWhen call
                        this->compileWhen(ctx, boolExp, jumpBackDistIndex, startOfBody, i);
                        ctx.src_index = i;
                        ctx.syncTokenIndexToSrcIndex();

                        /*Recursive call for compiling next when*/
                        std::cout<<"Recursive call\n";
                        this->compile(ctx);
                        std::cout<<"End recursive call\n";

                        /*Handle setting jump to end after successful execution of codeblock via jumpBackAction jump*/
                        double jump = (ctx.target.getInstructions().size() - jumpBackDistIndex);
                        ctx.writeToTargetAtByte(jump, jumpBackDistIndex);
                        break;
                    }
                    else if(ctx.source[i] == 'e' && ctx.source[i+1] == 'l' && ctx.source[i+2] == 's'  && ctx.source[i+3] == 'e')
                    {
                        uint32_t jumpBackDistIndex = 0;//will be assigned in compileWhen call
                        this->compileWhen(ctx, boolExp, jumpBackDistIndex, startOfBody, i);
                        const uint32_t startOfElseBlock = i + 5;
                        while(i + 2 < ctx.source.size())
                        {
	                        qCount = ((ctx.source[i] == '\"') + qCount) & 1;
	                        braceCount += (ctx.source[i] == '{') - (ctx.source[i] == '}');
                            if(!qCount && !braceCount && ctx.source[i] == 'e' && ctx.source[i+1] == 'n' && ctx.source[i+2] == 'd')
                            {
                                break;
                            }
                            i++;
                        }
                        const uint32_t endOfElseBlock = i - 1;
                        string_view elseBlock(ctx.source.substr(startOfElseBlock, endOfElseBlock - startOfElseBlock));
                        ctx.target.compileParams(elseBlock, ctx.runtime, ctx);
                        double jump = (ctx.target.getInstructions().size() - jumpBackDistIndex);
                        ctx.writeToTargetAtByte(jump, jumpBackDistIndex);
                        break;
                    }
                    else if(ctx.source[i] == 'e' && ctx.source[i+1] == 'n' && ctx.source[i+2] == 'd')
                    {
                        uint32_t jumpBackDistIndex = 0;//will be assigned in compileWhen call
                        this->compileWhen(ctx, boolExp, jumpBackDistIndex, startOfBody, i);
                        double jump = (ctx.target.getInstructions().size() - jumpBackDistIndex);
                        ctx.writeToTargetAtByte(jump, jumpBackDistIndex);
                        break;
                    }
                }
                else if(ctx.source[i] == 'e' && ctx.source[i+1] == 'n' && ctx.source[i+2] == 'd')
                {
                    uint32_t jumpBackDistIndex = 0;//will be assigned in compileWhen call
                    this->compileWhen(ctx, boolExp, jumpBackDistIndex, startOfBody, i);
                    double jump = (ctx.target.getInstructions().size() - jumpBackDistIndex);
                    ctx.writeToTargetAtByte(jump, jumpBackDistIndex);
                    break;
                }
            }
            i++;
        }
    }
    #ifdef crap
	virtual void compile_old(CompilationContext &ctx) {
        //parse expression, start from substr when go up until substr end is found
	    //extract substring, and save the other parts of the expression
	    //find when evaluate expression between it and then
	    //if the expression evaluates to anything other than 0
	    //then extract the expression proceeding the then statement
	    //frame->index = 0;
	   const int startIndex = ctx.src_index;

        const string_view exp = string_view(ctx.source);
	    uint32_t endIndex = ctx.src_index;
        
	    /*this block locates the end keyword for the ascal when block*/{
	    uint32_t i = endIndex;
	    uint8_t qCount = 0;
	    while(i < ctx.source.length()-2)
	    {
	    	qCount = ((ctx.source[i] == '\"') + qCount) & 1;
	    	if(ctx.source[i] == 'e' && ctx.source[i+1] == 'n' && ctx.source[i+2] == 'd' && qCount == 0)
	    	{
	    		endIndex = i;
	    		i = -1U;//uint so this becomes largest possible value
	    	}
	    	i++;
	    }
	    }
	    //string_view endOfExp = string_view(ctx.source).substr(endIndex+3,ctx.source.length());
	    uint32_t index = startIndex + 4;
	    //should always start after when is finished to build boolean expression

        //find boolean expression after when
        //find code block to exec if be evals to true
        //find out if next is when, else, or end
        //if when take when to end extract to function frame with parent of current push to stack set index to after end
        //if else take code block after else and put on stack set index to after end
        //if end set index to after end
        //then push code block to be exec if true on stack
        //then push conditional onto stack with boolean expression
	    
	    const uint32_t whenIndex = startIndex;
	    double boolExpValue;
	    const uint32_t elseIndex = ctx.source.find(this->elseText, index);
        
        const uint32_t thenIndex = ctx.source.find(this->thenText,index);

            //Parsing boolean expression
            while(ctx.source[index] == ' ' && index < endIndex && index < thenIndex)
            {
                index++;
            }
            const uint32_t startOfBoolExp = index;
            while(ctx.source.size() > index && index < endIndex && index < thenIndex)
            {
                index++;
            }
            string_view booleanExpression = exp.substr(startOfBoolExp,index-startOfBoolExp);
            const uint32_t boolExpLen = index-startOfBoolExp;
	        if(booleanExpression.size() == 0)
	        {
	            throw std::string("Error no boolean expression provided in when.\n");
	        }

		    if(*runtime.boolsettings["o"])
	        {
	            std::cout<<"Executing Boolean Expression: "<<booleanExpression<<'\n';
	        }
            index += 5;//increment index past this when
            const uint32_t nextWhen = ctx.source.find(this->keyWord,index);//find next when
            const uint32_t codeBlockEndIndex = std::min(std::min(endIndex, thenIndex),elseIndex);
            string_view value = ParsingUtil::getExprInStringSV(exp.substr(index, codeBlockEndIndex - index), 0, '{', '}',';').data;//get code block
        //for each when 
        //call super class compile first
        //end of last codeblock
        //for else 
        //for end remove end token
        Keyword::compile(ctx);
        //start of bool exp end of bool exp
        string_view boolExpStartOffset = ctx.target.append(0 - ctx.target.getInstructions().length());
        ctx.target.append(boolExpLen);
        //start of codeblock end of codeblock 
        string_view startOfCodeBlockOffset = ctx.target.append(index);
        ctx.target.append(codeBlockEndIndex - index);
        //start of next when 
        string_view nextWhenIndex = ctx.target.append(nextWhen);
        //ctx.original.compileInstructions(ctx.source.substr(startIndex, boolExpLen), runtime, ctx);
        /*for(int32_t i = startIndex; i < startIndex + boolExpLen; i++)
        {
            ctx.target.append(exp[i]);
        }*/
        //ctx.original.compileInstructions(ctx.source.substr(index, codeBlockEndIndex - index), runtime, ctx);
        for(int32_t i = index; i < codeBlockEndIndex; i++)
        {
            ctx.target.append(exp[i]);
        }
        ctx.src_index = std::min(nextWhen, endIndex+3);
    }
    #endif
    //so new action algo will be
    //parse bool exp
    //parse codeblock
    //parse remainder
    //if remainder has when then include it, if only else disinclude it and only include codeblock in remainder
    //push remainder in subframe
    //push codeblock in subframe
    //push boolean expression in conditional frame
};

#endif /* KEYWORDS_WHENACTION_HPP_ */
