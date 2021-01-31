/*
 * DerivefnAction.hpp
 *
 *  Created on: Jan 16, 2021
 *      Author: andrewrubinstein
 */

#ifndef KEYWORDS_DERIVEFNACTION_HPP_
#define KEYWORDS_DERIVEFNACTION_HPP_

#include "../Keyword.hpp"

const std::string operators = "+-*/^";

class DerivefnAction: public Keyword {
public:
	DerivefnAction(AscalExecutor *runtime, std::unordered_map<std::string,Object> *memory, std::map<std::string,setting<bool> > *boolsettings):
	Keyword(runtime, memory, boolsettings)
	{
		this->keyWord = "derive";
	}
	std::string action(AscalFrame<double>* frame) override
	{
		//Feel free to change anything, I've just added the boilerplate, and some convenience code for you
	    SubStr exp = ParsingUtil::getFollowingExpr(frame->exp, frame->index, keyWord);
	    std::vector<SubStr> params = Object("","",exp.data).params;
	    //you can change this, just getting some params for you
	    if(params.size()<3)
	    	throw std::string("derive <function name, variable to differentiate, derivative degree>");
	    //id is the function name, getInstructions() will return a reference to the code saved for the function as a string
	    SubStr vns = ParsingUtil::getVarName(frame->exp, frame->index+keyWord.size()+params[0].start);
	    Object *function = runtime->resolveNextExprSafe(frame, vns);
	    std::string withRespectTo = params[1].data;
	    uint16_t degree = (uint16_t) runtime->callOnFrame(frame, params[2].data);
	    //Your code here

        std::stringstream strStream;

		std::string fnString = function->instructionsToString();

	    //end of your code is defining the string derivative
	    derive_inter(0, fnString.length() - 2, withRespectTo.c_str(), string_view(fnString.c_str(), fnString.length() - 1), strStream);
	    std::stringstream call;
	    call<<"let "<<function->id<<"prime"<<degree<<" = "<<strStream.str();
	    //Saves function that when it is a first derivative of f looks like
	    //fprime1 = first derivative of f
	    runtime->callOnFrame(frame, call.str());
	    //Automatically moves to next expression after keyword execution because we return MAX
	    return MAX;
	}
private:
	// Given the starting starting index of an open parenthesis, finds the respective closing parenthesis
	int findMatchingPara(std::string &str, int startIndex) {
		if (str[startIndex] != '(') {
			return -1;
		}

		int p_count = 1;

		for (int i = startIndex + 1; i < str.length(); ++i) {
			if (str[i] == '(') {
				++p_count;
			}
			else if (str[i] == ')') {
				--p_count;
			}

			if (p_count == 0) {
				return i;
			}
		}

		return -1;
	}

	// Recursively determines the derivative. Rather than creating new strings to pass to recursive calls, derive_inter uses the pair of indices i and j
    // to determine which string portion is to be processed. The characters between indices i and j are processed, including the characters at i and j.
    void derive_inter(int i, int j, const char *by, string_view expr, std::stringstream &strStream) {
        int p_count = 0;

		/*for(int k = i; k <= j; ++k) {
			std::cout << expr[k];
		}

		std::cout << std::endl;*/

        // This loop attempts to find a pair of terms with "parenthesis depth" 0, where parenthesis depth is 0 if k isn't inside any parentheses, 1 if 
        // inside one set of parentheses, etc. This loop only looks for terms separated by + or -.
        for (int k = i; k <= j; ++k) {
            if (expr[k] == '(') {
                ++p_count;
            }
            else if (expr[k] == ')') {
                --p_count;
            }
            else if (p_count == 0 && (expr[k] == '+' || expr[k] == '-')) {
                derive_inter(i, k - 1, by, string_view(expr, i, k - 1), strStream);
                strStream << expr[k];
                derive_inter(k + 1, j, by, string_view(expr, k + 1, j), strStream);
                return;
            }
        }

        p_count = 0;

        // This loop looks for multiplication or division of two terms with parenthesis depth 0. Does not handle multiplying a term by a coefficient, this is handled later.
        for (int k = i; k <= j; ++k) {
            if (expr[k] == '(') {
                ++p_count;
            }
            else if (expr[k] == ')') {
                --p_count;
            }
            if (p_count == 0 && expr[k] != '^') {
                if (expr[k] == '*') {
                    // Apply product rule
                    strStream << "((";
                    derive_inter(i, k - 1, by, string_view(expr, i, k - 1), strStream);
                    strStream << ")*(" << expr.substr(k + 1, j - k) << ")+(";
                    derive_inter(k + 1, j, by, string_view(expr, k + 1, j), strStream);
                    strStream << ")*(" << expr.substr(i, k - i)<< ")";
                    return;
                }
                else if (expr[k] == '/') {
                    // Apply quotient rule
                    strStream << "(((";
                    derive_inter(i, k - 1, by, string_view(expr, i, k - 1), strStream);
                    strStream << ")*(" << expr.substr(k + 1, j - k) << ")+(";
                    derive_inter(k + 1, j, by, string_view(expr, k + 1, j), strStream);
                    strStream << ")*(" << expr.substr(i, k - i) << "))/(" << expr.substr(k + 1, j - k) << ")^2)";
                    return;
                }
                else if (i != j && k + 1 <= j) {
                    // Apply product rule when '*' isn't present
                    if (((expr[k] == ')') && (operators.find(expr[k + 1]) == operators.npos))) {
                        //std::cout << expr.substr(i, k - i + 1) << std::endl;
                        //std::cout << expr.substr(k + 1, j - k) << std::endl;
                        strStream << "((";
                        derive_inter(i, k, by, string_view(expr, i, k), strStream);
                        strStream << ")*(" << expr.substr(k + 1, j - k) << ")+(";
                        derive_inter(k + 1, j, by, string_view(expr, k + 1, j), strStream);
                        strStream << ")*(" << expr.substr(i, k - i + 1) << "))";
                        return;
                    }
                    else if ((expr[k + 1] == '(') && (operators.find(expr[k]) == operators.npos)) {
                        //std::cout << expr.substr(i, k - i + 1) << std::endl;
                        //std::cout << expr.substr(k + 1, j - k) << std::endl;
                        strStream << "((";
                        derive_inter(i, k, by, string_view(expr, i, k), strStream);
                        strStream << ")*(" << expr.substr(k + 1, j - k) << ")+(";
                        derive_inter(k + 1, j, by, string_view(expr, k + 1, j), strStream);
                        strStream << ")*(" << expr.substr(i, k - i + 1) << "))";
                        return;
                    }
                }
            }
            else if (p_count == 0 && expr[k] == '^') {
                if (expr[k + 1] != '(') {
                    while (k <= j && expr[k] != '(' && expr[k] != '*' && expr[k] != '/') {
                        ++k;
                    }
                }
            }
        }

        p_count = 0;
        
        // If the previous two cases failed, the expression is checked for the ^ operator, and applies the relevant power rule.
        for (int k = i; k <= j; ++k) {
            if (expr[k] == '(') {
                ++p_count;
            }
            else if (expr[k] == ')') {
                --p_count;
            }
            else if (p_count == 0 && expr[k] == '^') {
                if (expr.substr(k + 1, j - k).find(by) != -1) {
                    //Apply generalized power rule 
                    ///Currently unimpemented
                    strStream << "";
                    return;
                }
                else {
                    //Apply basic power rule
                    strStream << "((" << expr.substr(k + 1, j - k) << ")*(" << expr.substr(i, k - i) << ")^(" << expr.substr(k + 1, j - k) << "-1))";
                    return;
                }
            }
        }

        // If none of the previous cases passed, and there are parentheses, the parentheses are considered useless and are discarded in another recursive call.
        if (expr[i] == '(' && expr[j] == ')') {
            derive_inter(i + 1, j - 1, by, expr, strStream);
            return;
        }

        p_count = 0;

        // The only considerations left are if there is a lone x term (possibly with a coefficient) raised to the first power, or there is no x term.
        if (expr.substr(i, j - i + 1).find(by) != -1) {
            if (0 != expr.substr(i, j - i + 1).find(by) && expr[expr.substr(i, j - i + 1).find(by) + i - 1] != ' ') {
                //std::cout << expr.substr(i, j - i + 1) << std::endl; 
                //std::cout << expr.substr(i, j - i + 1).find(by) + i - 1 << std::endl;
				//std::cout << '\"' << expr[expr.substr(i, j - i + 1).find(by) + i - 1] << '\"' << std::endl;
                strStream << expr.substr(i, expr.substr(i, j - i + 1).find(by));
                return;
            }
            else {
                strStream << 1;
                return;
            }
        }
        strStream << 0;
        return;
    }
};

#endif /* KEYWORDS_DERIVEFNACTION_HPP_ */
