/*
 * Calculator.hpp
 *
 *  Created on: Dec 9, 2020
 *      Author: andrewrubinstein
 */

#ifndef CALCULATOR_HPP_
#define CALCULATOR_HPP_
#include <map>
#include <cmath>
#include <iostream>

template <class t>
class Calculator {
private:
	static std::map<char,t (*)(t&,t&)> operations;
	t add(t &and1,t &and2){    return and1+and2;}
	t subtract(t &and1,t &and2){return and1-and2;}
	t multiply(t &and1,t &and2){return and1*and2;}
	t divide(t &and1,t &and2){return and2!=0?and1/and2:throw std::string("Error division by zero.");}
	t doubleModulus(t &and1,t &and2)
	{
	    t result;
	      if(and2 != 0)
	      {
	        	  double quotient =  and1/and2;
	        	  double flooredQuotient;
	        	  modf(quotient,&flooredQuotient);
	        	  result =  and1 - and2*flooredQuotient;

	        	  if(and1*and2 < 0 && result >0)
	        	  {
	        	      result *= -1;
	        	  }
	        	  else if(and1*and2 > 0 && result <0)
	              {
	        	        result *= -1;
	        	  }

	      }
	      else
	      {
	          throw std::string("Error modulus by zero.");
	      }
	      return result;
	}
	t exponentiate(t &and1,t &and2){return std::pow(and1,and2);}
	t permute(t &and1,t &and2){
	    t result =  1;

	    for(int i = 0;i<and2 && and1 - i > 1;i++)
	    {
	        result *= and1 - i;
	    }
	    return result;
	}
	t combinations(t &and1,t &and2)
	{
	    t result =  1;
	    for(int i = 0;i<and2 && and1 - i > 1;i++)
	    {
	        result =  (result*(and1 - i))/(and2-i);
	    }
	    return result;
	}
	t log(t &and1,t &and2){return std::log(and2)/std::log(and1);}
	t rootOp(t &and1,t &and2){return std::pow(and2,1/and1);}
	t equals(t &and1,t &and2){return and1==and2;}
	t lessThan(t &and1,t &and2){return and1<and2;}
	t greaterThan(t &and1,t &and2){    return and1>and2;}
public:
	Calculator(){}


	t calc(char op,t and1,t and2)
	{
	    t result =  0;
	    switch (op) {
	    case '+': result = add(and1,and2); break;
	       case '-': result = subtract(and1,and2); break;
	       case '*': result = multiply(and1,and2); break;
	       case '/': result = divide(and1,and2); break;
	       case '%': result = doubleModulus(and1,and2); break;
	       case '^': result = exponentiate(and1,and2); break;
	       case 'P': result = permute(and1,and2); break;
	       case 'C': result = combinations(and1,and2); break;
	       case '@': result = log(and1,and2); break;
	       case '$': result = rootOp(and1,and2); break;
	       case '=': result = equals(and1,and2); break;
	       case '<': result = lessThan(and1,and2); break;
	       case '>': result = greaterThan(and1,and2); break;
	    }
	    return result;
	}
	static bool isNonParentheticalOperator(char op)
	{

	    switch (op) {
	       case '+': return true;
	       case '-': return true;
	       case '*': return true;
	       case '/': return true;
	       case '%': return true;
	       case '^': return true;
	       case 'P': return true;
	       case 'C': return true;
	       case '@': return true;
	       case '$': return true;
	       case '=': return true;
	       case '<': return true;
	       case '>': return true;
	    }
	    return false;
	}
	static bool isOperator(char op)
	{
		if(op == '(' || op == ')')
			return true;
		else
			return isNonParentheticalOperator(op);
	}
	static char getPriority(char ator)
	{
	  int priority = -1;
	  switch (ator){
	  case '(':
		  priority = 0;
		  break;
	  case ')':
		  priority = 0;
		  break;
	  case '^':
		  priority = 90;
		  break;
	  case '*':
		  priority = 80;
		  break;
	  case '/':
		  priority = 80;
		  break;
	  case '%':
		  priority = 80;
		  break;
	  case '$':
		  priority = 80;
		  break;
	  case 'P':
		  priority = 80;
		  break;
	  case 'C':
		  priority = 80;
		  break;
	  case '+':
		  priority = 70;
		  break;
	  case '-':
		  priority = 70;
		  break;
	  case '=':
		  priority = 60;
		  break;
	  case '>':
		  priority = 60;
		  break;
	  case '<':
		  priority = 60;
		  break;
	  case 'a':
		  priority = -255;

	  }
	  return priority;
	}
	virtual ~Calculator(){}
};

#endif /* CALCULATOR_HPP_ */
