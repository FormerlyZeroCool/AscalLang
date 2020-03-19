# AscalLang
<h3>Andrew's Simple Calculator Language</h3>
Ascal is a language designed for people in STEM fields who deal with long mathematical expressions, and formula.<br>
Ascal provides an easy way to save commonly used mathematical functions, and run them with dynamically specified parameters<br>
<br>
<h3>Ascal is essentially a calculator on steroids, and is exactly what I need for many of my classes.</h3> 
Ascal tries to be as succinct as possible while expressing the mathematical functions, and aims to use a mathematical syntax familiar to people in STEM fields.<br>
to make the UX as easy as possible for students to get used to.<br>
<br>
A quick help guide to using the language, further documentation to come<br>
Note: functions have been tested to provide at least 160 levels of nesting, and should support much more, <br>My guess would be nearer to 1,000; however, it is possible with complex function definitions with many parameters could shrink this number to be closer to ~400 to 500 levels<br>
<br>
Enter a mathematical expression, or you can also use<br> 
parameters to choose between the following options or see below "redo" for how to handle variables/functions.<br>
valid params are:<br>
t to show time taken to interpret, and calculate expression<br>
o to show operations in order of execution in console<br>
d to show debug information in console<br>
u to show execute previous statement in console or "undo"<br>
r to show "redo"<br>
<br>
<h3>How to use variables/functions:</h3>
let [variableName] = [expression] to save an expression with the given variable name.<br>
const [variableName] = [expression] will save the result of the expression with the given variable name.<br>
<br>
to use a saved expression simply type the variable name, then in parenthesis supply any parameters
<br>
Example:<br>
let x = c^2<br>
x(4)<br>
and the program will print the result of 4^2<br>
because it replaces x with c^2, and c with the parameter supplied.<br>

you can print the expression a variable holds by typing<br>
print [variableName] or printall to print everything in memory,<br>
printsdf prints only system defined functions<br>
printudf prints only user defined functions<br>

You can also delete a variable by typing delete [variableName]<br>
Or delete all saved variables by typing deleteall<br>
<br>
You can print all variables, and their expressions by typing printall<br>
<br>
<br>
<h3>Operators Documentation:</h3>
  (...) - Parenthesis have the highest priority, and any expression inside them will be calculated before anything outside<br>
  '^' - or the exponentiation operator has the second highest priority, and will return the first operand raised to the power of the
  second<br>
  This operator cannot handle fractional powers, and will return the result as if it was raised to the power of 0<br>
  written like : x^z<br>
  
  'X' , '/' , '%' ,'$' , 'P' all have the same third level of priority<br>
  Asterisk or X is the multiplication operator<br>
  '/' is the division operator<br>
  '%' is not exactly modulus, as it returns the entire remainder, including the decimal part<br>
  So, 3.3 % 2 = 1.3, not 1 like with the common modulus operator
  '$' the left operand of $ defines the fractional part of a power we are raising the second operand to so something like:<br>
  operand2^(1/operand1)<br>
  Although the exponentiation operator cannot handle fractional powers<br>
  'P' is the permutation operator from statistics or factorial(operand1)/factorial(operand1-operand2)<br>
  
  '+' , '-' after that the next priority level is for addition, and subtraction<br>
  
  '=' , '>' , '<' and finally we have comparison operators with the lowest priority which either return a 1 or a 0<br>
