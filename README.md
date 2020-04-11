# AscalLang
<h3>Andrew's Simple Calculator Language</h3>
Ascal is a language designed for people in STEM fields who deal with long mathematical expressions, and formula.<br>
Ascal provides an easy way to save commonly used mathematical functions, and run them with dynamically specified parameters<br>
There are also now plans to encapsulate Ascal within a class for use as a programming language in other C++ programs<br>
<br>
Ascal tries to be as succinct as possible while expressing the mathematical functions, and aims to use a mathematical syntax, it does this to make the UX as easy as possible for people in STEM to get used to.<br>
<br>
<h3>A quick help guide to using the language, further documentation to come</h3>
Note: functions have been tested to provide at least 160 levels of nesting, and although it does not currently exist support for loops, and other conditional execution modules like if and else; however, not, and true built in functions, along with the boolean operators = , <,> can be used for boolean logic, and we now have support for the when then end keywords allowing basic decision control structures  wo we also support recursion now! In my tests on Mac OSX High Sierra I can do about 6000 recursive stack frames with default stack size settings, unfortunately you just get the error segmentation fault if a stack overflow occurs.<br>
The not function returns 1 if a 0 is supplied, and zero with all other values, the true functions returns 1 if any value other than 0 is supplied
<br>
<h5>Your input testing the value supplied in this case 0</h5>
>>`not(0)`
<p>
<h5>Result of test:</h5> <br>Final Answer: 
</p>
1
<br>

<h5>Your input testing the value supplied in this case 1</h5>
>>`not(1)`
<p>
<h5>Result of test:</h5> <br>Final Answer: 
</p>
0
<br>

<h5>Your input testing the value supplied in this case 1</h5>
>>`true(1)`
<br>
<p>
<h5>Result of test:</h5><br>Final Answer: 
</p>
1

<br>
<h5>Your input testing the value supplied in this case 0</h5>
>>`true(0)`
<p>
<h5>Result of test:</h5><br>Final Answer:
</p> 
0
<br>
<br>
Enter a mathematical expression, or you can also use<br> 
parameters to choose between the following options or see below for how to use variables/functions.<br>
<h3>Valid params are:</h3>
t to show time taken to interpret, and calculate expression<br>
o to show operations in order of execution in console<br>
d to show debug information in console<br>
u to show execute previous statement in console or "undo"<br>
r to show "redo"<br>
<br>
<p>
<h4>Example of how o parameter works</h4>
>>```let f = x^2+x```
<br>
Loaded Function: `f`
<br>
expression: `x^2+x`
<br>
<br>
Final Answer:<br> 
0<br>
>>`o`
<br>
Show Operations Status: 1<br>
<br>
>>`f(2)`
<br>
`2^2 = 4`
<br>
`4+2 = 6`
<br>
Final Answer: 
<br>
6
<br>
>>`o`
<br>
Show Operations Status: 0
<br>
>>`f(2)`
<br>
Final Answer: 
<br>
6
</p>
<br>
<br>
<h4>Example of how t parameter works</h4>
 >>`t`
<br>
Print time taken to run calculation Status: 1<br>
>>`2+2`
<br>
finished computation at Thu Apr  2 15:00:01 2020<br>
elapsed time: 0.00134s<br>
Final Answer: <br>
4<br>
>>`t`<br>
Print time taken to run calculation Status: 0<br>
>>`2+2`<br>
Final Answer: <br>
4<br>
<br>
<h4>Examples of r and u parameters short for redo and undo</h4>
>>`cos(pi)`<br>
Final Answer: <br>
-1<br>
<br>
>>`u`<br>
`cos(pi)`<br>
Final Answer: <br>
-1<br>
<br>
>>`r`<br>
`cos(pi)`<br>
Final Answer:<br> 
-1
<br>
<h3>How to use variables/functions:</h3>
let [variableName] = [expression] to save an expression with the given variable name, to the global scope.<br>
const [variableName] = [expression] will save the result of the expression with the given variable name to the global scope.<br>
loc [variableName] = [expression] to save an expression with the given variable name to the local scope.<br>
cloc [variableName] = [expression] to save the result of calculating the provided expression with the given variable name to the local scope.<br>
Think locally scoped version of const
<br>
loc, and cloc are useful with batched commands like `loc x = value^2; x`(4)
or `loc y = 5;loc x = 5;y+x`<br>
which results in 10
<br>
loc, and cloc can also be used inside of multi-line functions implemented like:<br>
let x = {<br>
loc y = z^2<br>
y(c^2)<br>
}<br>
which defines the global function x for later reference, the expression x will be defined with is:loc y = z^2;y(c^2);<br>
when we run the function x it will define y in its local scope, so when in the next statement is executed y will be defined, and c will be taken from the parameter supplied to the function x
<br>
<br>
Example:<br>
`let x = c^2`<br>
`x(4)`<br>
and the program will print the result of 4^2<br>
because it replaces x with c^2, and c with the parameter supplied.
<br>

<br>
<h3>The when then end set of keywords.</h3>
<p>
The when then end keywords are the first way in Ascal to conditionally execute your code, allowing for recursion.<br>
This functionality is still in development, and does not support multi-variable functions currently(issues with parameter memory scoping)<br>
Example Ascal Code for a recursive summation series function:<br>
let f = when x>1 then f(x-1)+x when x<1 + x=1 then 1 end;<br>
If the parser sees the keyword when it will evaluate the expression between the when, and the proceeding then. If the expression evaluates <br> to anything other than 0 it will be interpreted as true, and the expression following the then will be executed, otherwise the next<br> expression proceeding the next when clause will be executed, all cases must be covered, missing cases will cause the program to fail,<br> working on else support.<br>
*Note if you end up with a stack overflow you will currenty get a segmentation fault, and the Ascal Interpreter will crash
                                        </p>
<h3>Print Commmands:</h3>
By default when running any expression the Ascal Interpreter will print out the result of any expressions calculations, but keep reading to learn how to print information about variables saved in memory.<br>
<br>
print all is a command that will print all the information saved in memory pertaining to all saved functions<br>
print [variableName] to print everything in memory related to the specified variable.<br>
print sdf to print only system defined functions.<br>
print udf to print only user defined functions<br>
<br>
<h3>Memory Managment:</h3>
<h4>For Global Variables:</h4>
You can delete a variable by typing delete [variableName]<br>
Or delete all saved variables by typing delete all<br>
<h5>Note:</h5>
1) If you redefine a variable using the same name as a previously used variable, the old one will be overwritten automatically <br>
2) System Defined Variables cannot be deleted, but you can overwrite their values, to refresh with the original values delete the variable you overwrote with the delete [varName] command, and all system defined functions will revert to the state they where in at startup
<br>
<h4>For Local Variables:</h4>
Local Variables are automatically managed by the program stack, and will be cleared at the end of a batch of statements or a function call 
<br>
<br>
<h3>Operators Documentation:</h3>
Operator precedence is defined below, in a case where two adjacent operations have the same priority the left most operation will occur first<br>
<br>
  (...) - Parenthesis have the highest priority, and any expression inside them will be calculated before anything outside<br>
  '^' - or the exponentiation operator has the second highest priority, and will return the first operand raised to the power of the
  second<br>
  This operator cannot handle fractional powers, and will return the result as if it was raised to the power of 0<br>
  written like : x^z<br>
  
  'X' , '/' , '%' ,'$' , 'P' all have the same third level of priority<br>
  <br>
  Asterisk or 'X' is the multiplication operator<br>
  <br>
  '/' is the division operator<br>
  <br>
  '%' is not exactly modulus, as it returns the entire remainder, including the decimal part<br>
  So, 3.3 % 2 = 1.3, not 1 like with the common modulus operator
  <br>
  <br>
  '$' the left operand of '$' defines the fractional part of a power we are raising the second operand to so something like:<br>
  rightOperand^(1/leftOperand)<br>
  Although the exponentiation operator cannot handle fractional powers<br>
  <br>
  'P' is the permutation operator from statistics or factorial(leftOperand)/factorial(leftOperand-rightOperand)<br>
  <br>
  '+' , '-' after that the next priority level is for addition, and subtraction<br>
  <br>
  '=' , '>' , '<' and finally we have comparison operators with the lowest priority which either return a 1 or a 0<br>
  <br><br>
  <p>
 <h4>Try Running this code</h4>
`t`<br>
`let x = 2^16`<br>
```const sqr =    (1-((1 - x))/2)```<br>
```const sqr1 = (sqr-((sqr^2 - x))/(2*sqr))```<br>
```const sqr2 = (sqr1-((sqr1^2 - x))/(2*sqr1))```<br>
```const sqr3 = (sqr2-((sqr2^2 - x))/(2*sqr2))```<br>
```const sqr4 = (sqr3-((sqr3^2 - x))/(2*sqr3))```<br>
```const sqr5 = (sqr4-((sqr4^2 - x))/(2*sqr4))```<br>
```const sqr6 =  (sqr5-((sqr5^2 - x))/(2*sqr5))```<br>
```const sqr7 =  (sqr6-((sqr6^2 - x))/(2*sqr6))```<br>
```const sqr8 =  (sqr7-((sqr7^2 - x))/(2*sqr7))```<br>
```const sqr9 =  (sqr8-((sqr8^2 - x))/(2*sqr8))```<br>
```const sqr10 = (sqr9-((sqr9^2 - x))/(2*sqr9))```<br>
```const sqr11 = (sqr10-((sqr10^2 - x))/(2*sqr10))```<br>
```const sqr12 = (sqr11-((sqr11^2 - x))/(2*sqr11))```<br>
```const sqr13 = (sqr12-((sqr12^2 - x))/(2*sqr12))```<br>
```const sqr14 = (sqr13-((sqr13^2 - x))/(2*sqr13))```<br>
```const sqr15 = (sqr14-((sqr14^2 - x))/(2*sqr14))```<br>
```print const sqr16 = (sqr15-((sqr15^2 - x))/(2*sqr15))```<br>

```delete x```<br>

`let sqr =x*0+ (1-((1 - x))/2)`<br>
`let sqr1 = x*0+ (sqr(x)-((sqr(x)^2 - x))/(2*sqr(x)))`<br>
`let sqr2 = x*0+ (sqr1(x)-((sqr1(x)^2 - x))/(2*sqr1(x)))`<br>
`let sqr3 = x*0+ (sqr2(x)-((sqr2(x)^2 - x))/(2*sqr2(x)))`<br>
`let sqr4 = x*0+ (sqr3(x)-((sqr3(x)^2 - x))/(2*sqr3(x)))`<br>
`let sqr5 = x*0+ (sqr4(x)-((sqr4(x)^2 - x))/(2*sqr4(x)))`<br>
`let sqr6 = x*0+ (sqr5(x)-((sqr5(x)^2 - x))/(2*sqr5(x)))`<br>
`let sqr7 = x*0+ (sqr6(x)-((sqr6(x)^2 - x))/(2*sqr6(x)))`<br>
`let sqr8 = x*0+ (sqr7(x)-((sqr7(x)^2 - x))/(2*sqr7(x)))`<br>
`let sqr9 = x*0+ (sqr8(x)-((sqr8(x)^2 - x))/(2*sqr8(x)))`<br>
`let sqr10 = x*0+ (sqr9(x)-((sqr9(x)^2 - x))/(2*sqr9(x)))`<br>
`let sqr11 = x*0+ (sqr10(x)-((sqr10(x)^2 - x))/(2*sqr10(x)))`<br>
`let sqr12 = x*0+ (sqr11(x)-((sqr11(x)^2 - x))/(2*sqr11(x)))`<br>
`let sqr13 = x*0+ (sqr12(x)-((sqr12(x)^2 - x))/(2*sqr12(x)))`<br>
`sqr10(49)`
`let ack = when m=0 + n*0  then n+1 when n=0 then ack(m-1,1) when (m + n)>0 then ack(m-1,ack(m,n-1)) end`
`ack(3,2)`
 </p>
