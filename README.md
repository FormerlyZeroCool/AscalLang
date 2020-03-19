# AscalLang
Andrew's Simple Calculator Language
Ascal is a language designed for people in STEM fields who deal with long mathematical expressions, and formula
Ascal provides an easy way to save commonly used mathematical functions, and run them with dynamicly specified parameters
Ascal is essentially a calculator on steroids, and is exactly what I needed for many of my classes, 
Ascal aims to be as succinct as possible while expressing the mathematical functions, and aims to use a mathematical syntax 
to make the UX as easy as possible for students to get used to.

A quick help guide to using the language, further documentation to come<br>
Enter a mathematical expression, or you can also use 
parameters to choose between the following options or see below "redo" for how to handle variables/functions.
valid params are:
t to show time taken to interpret, and calculate expression
o to show operations in order of execution in console
d to show debug information in console
u to show execute previous statement in console or "undo"
r to show "redo"

How to use variables/functions:<br>
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
Operators Documentation:<br>
  (...) - Parenthesis have the highest priority, and any expression inside them will be calculated before anything outside<br>
  '^' - or the exponentiation operator has the second highest priority, and will return the first operand raised to the power of the
  second<br>
  This operator cannot handle fractional powers, and will return the result as if it was raised to the power of 0<br>
  written like : x^z<br>
  
  'X' , '/' , '%' ,'$' , 'P' all have the same third level of priority<br>
  Asterisk or X is the multiplication operator<br>
  '/' is the division operator<br>
  '%' is not exactly modulus, as it returns the entire remainder, including the decimal part<br>
  '$' the left operand of $ defines the fractional part of a power we are raising the second operand to so something like:<br>
  operand2^(1/operand1)<br>
  Although the exponentiation operator cannot handle fractional powers<br>
  'P' is the permutation operator from statistics or operand1!/(operand1-operand2)!<br>
  
  '+' , '-' after that the next priority level is for addition, and subtraction<br>
  
  '=' , '>' , '<' and finally we have comparison operators with the lowest priority which either return a 1 or a 0<br>
