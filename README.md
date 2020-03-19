# AscalLang
<h3>Andrew's Simple Calculator Language</h3>
Ascal is a language designed for people in STEM fields who deal with long mathematical expressions, and formula.<br>
Ascal provides an easy way to save commonly used mathematical functions, and run them with dynamically specified parameters<br>
<br>
Ascal tries to be as succinct as possible while expressing the mathematical functions, and aims to use a mathematical syntax, it does this to make the UX as easy as possible for people in STEM to get used to.<br>
<br>
<h3>A quick help guide to using the language, further documentation to come</h3>
Note: functions have been tested to provide at least 160 levels of nesting, and although it does not currently exist support for loops, and other conditional execution modules like if and else<br>
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
because it replaces x with c^2, and c with the parameter supplied.
<br>
<br>

<h3>Print Commmands:</h3>
By default when running any expression the Ascal Interpreter will print out the result of any expressions calculations, but keep reading to learn how to print information about variables saved in memory.<br>
<br>
printall is a command that will print all the information saved in memory pertaining to all saved functions<br>
print [variableName] to print everything in memory related to the specified variable.<br>
printsdf to print only system defined functions.<br>
printudf to print only user defined functions<br>
<br>
<h3>Memory Managment:</h3>
You can delete a variable by typing delete [variableName]<br>
Or delete all saved variables by typing deleteall<br>
Note: System Defined Variables cannot be deleted, but you can overwrite their values, to refresh with the original values delete the variable you overwrote with the delete [varName] command, and all system defined functions will revert to the state they where in at startup
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
