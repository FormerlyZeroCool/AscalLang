# AscalLang
<h3>Andrew's Simple Calculator Language</h3>
Ascal is a language designed for people in STEM fields who deal with long mathematical expressions, and formula.<br>
Ascal provides an easy way to save commonly used mathematical functions, and run them with dynamically specified parameters<br>
There are also now plans to encapsulate Ascal within a class for use as a programming language in other C++ programs<br>
<br>
Ascal tries to be as succinct as possible while expressing the mathematical functions, and aims to use a mathematical/C style syntax, it does this to make the UX as easy as possible for people in STEM to get used to.<br>
<br>
<h3>A quick help guide to using the language, further documentation to come</h3>
Note: functions have been tested to provide at least 160 levels of nesting.<br>
The not function returns 1 if a 0 is supplied, and zero with all other values, the true functions returns 1 if any value other than 0 is supplied
<br>
<h5>Your input testing the value supplied in this case 0</h5>
>>

`not(0)`
<p>
<h5>Result of test:</h5> <br>Final Answer: 
</p>
1
<br>

<h5>Your input testing the value supplied in this case 1</h5>
>>

`not(1)`
<p>
<h5>Result of test:</h5> <br>Final Answer: 
</p>
0
<br>

<h5>Your input testing the value supplied in this case 1</h5>
>>

`true(1)`
<br>
<p>
<h5>Result of test:</h5><br>Final Answer: 
</p>
1

<br>
<h5>Your input testing the value supplied in this case 0</h5>
>>

`true(0)`
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
>>

`let f = x^2+x`
<br>
Loaded Function: 

`f`
<br>
expression: 

`x^2+x`
<br>
<br>
Final Answer:<br> 
0<br>
>>

`o`
<br>
Show Operations Status: 1<br>
<br>
>>

`f(2)`<br>
`2^2 = 4`<br>
`4+2 = 6`<br>
Final Answer: 
<br>
6
<br>
>>

`o`
<br>
Show Operations Status: 0
<br>
>>

`f(2)`
<br>
Final Answer: 
<br>
6
</p>
<br>
<br>
<h4>Example of how t parameter works</h4>
 >>
 
 `t`
<br>
Print time taken to run calculation Status: 1<br>
>>

`2+2`
<br>
finished computation at Thu Apr  2 15:00:01 2020<br>
elapsed time: 0.00134s<br>
Final Answer: <br>
4<br>
>>

`t`<br>
Print time taken to run calculation Status: 0<br>
>>

`2+2`<br>
Final Answer: <br>
4<br>
<br>
<h4>Examples of r and u parameters short for redo and undo</h4>
>>

`cos(pi)`<br>
Final Answer: <br>
-1<br>
<br>
>>

`u`<br>
`cos(pi)`<br>
Final Answer: <br>
-1<br>
<br>
>>

`r`<br>
`cos(pi)`<br>
Final Answer:<br> 
-1
<br>
<h3>How to use variables/functions:</h3>


`let [variableName] = [expression]` to save an expression with the given variable name, to the global scope.<br>
`const [variableName] = [expression]` will save the result of the calculating expression with the given variable name to the global scope.<br>
`loc [variableName] = [expression]` to save an expression with the given variable name to the local scope.<br>
`cloc [variableName] = [expression]` to save the result of calculating the provided expression with the given variable name to the local scope.<br>
Think locally scoped version of const
<br>
loc, and cloc are useful with batched commands like 

`loc x = value^2; x`(4)
or 

`loc y = 5;loc x = 5;y+x`<br>
which results in 10
<br>
loc, and cloc can also be used inside of multi-line functions implemented like:<br>


`let x = {
loc y = z^2
y(c^2)
}`

<br>
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
This functionality is still in testing, but it appears stable in this release<br>
Example Ascal Code for a recursive summation series function:<br>


`let f = when x>1 then f(x-1)+x when x<1 + x=1 then 1 end;`<br>
If the parser sees the keyword when it will evaluate the expression between the when, and the proceeding then. If the expression evaluates <br> to anything other than 0 it will be interpreted as true, and the expression following the then will be executed, otherwise the next<br> expression proceeding the next when clause will be executed, all cases must be covered, missing cases will cause the program to fail,<br> working on else support.<br>
*Note if you end up with a stack overflow you will currenty get a segmentation fault, and the Ascal Interpreter will crash
                                        </p>
<h3>The While keyword</h3>
<p>
 the While keyword is used for continously executing some instruction while a condition is met<br>
 it is extremely useful for implementing iterative logic, like summation series you don't wan't to or can't simplify to a constant function<br>
 It is composed of two parts, a boolean expression, and a code block which can be defined as follows:<br><br>
 
`    while not(counter > max)*prime{` this line has the boolean expression proceeding the while keyword, and the opening bracket{ denoting the start of the code block, and end of the boolean expression.<br>

`      when x%counter = 0 then set prime = 0 else 0 end;`<br>
`      set counter = counter+1;`<br>
`    };`<br>

</p>
<h3>exists keyword:</h3>
<p>
The exists keyword when supplied the name of a variable returns 1 if the variable exists anywhere in memory, and 0 if it does not. <br>ex:<br>
 
 `exists sin`
</p>
<h3>Print Commmands:</h3>
By default when running any expression the Ascal Interpreter will print out the result of any expressions calculations, but keep reading to learn how to print information about variables saved in memory.<br>
<br>
print all is a command that will print all the information saved in memory pertaining to all saved functions<br>
print [variableName] to print everything in memory related to the specified variable.<br>
print sdf to print only system defined functions.<br>
print udf to print only user defined functions<br>
printStr can be used to print a string, you can also add function called into the string by placing them in parenthesis like so:<br>


`printStr "Hello world the number pi is approximately (pi), and sin of 0 is (sin(0))"`
<br>
<h3>The input keyword:</h3>
<p>
The input keyword is used to get numeric input from the user, and can simply be inserted into any expression instead of a variable to prompt the user to type a number<br>
You may also supply your own prompt using a format similar to the printStr syntax, like so:<br>
 
`2+input "type a number 2 to like (pi) for exampleendl"` the endl keyword inside the quotation marks will insert a line break, so upon execution the prompt will say<br>

`type a number 2 to like (pi) for example`<br>
the user will start typing here, on the next line.<br>
When the user types in their input the expression will be evaluated like normal as if the user's input was a hardcoded value.
</p>
<h3>The import keyword:</h3>
<p>
The import keyword can be used to load functions from files you've already written, there are two ways to use this keyword.<br>
The reccomended usage is to ensure the file is saved with the .asl file extension, and is in a plain text encoding<br>
If you do that you can use the syntax <br>
 
 `import path1.path2.<your_file_name>`<br>
otherwise you must specify the file format, note that the file must still be in a plain text encoding<br>
 
 `import path1.path2.<your_file_name>.<your_extension>`<br>
</p>
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

<h3>The approxInt keyword:</h3>
<p>
 The approxInt keyword is used to calculate a Reimann sum, it takes 4 parameters using the following syntax:<br>
 
 
 `approxInt [your_function_name],[lower_bound]<>[upper_bound],[number_of_partitions]`<br>
 Example:<br>
 
 `approxInt g,0<>1,10000000`<br>
`domain:0 to 1 with a step size in the x of: 1e-07`<br>
`Function: g, function defined as: x^2;`<br>
  
 `Area Under Curve calculated with reimann sum using 10000000 partitions: 0.333333`
</p>
<br>
<h3>The plot keyword:</h3>
<p>
The "plot" keyword in Ascal currently is a simple tool to graph your non-recursive functions in the command line<br>
To use it type: 
 
 `plot [your_function's_name]|[your_second_function's_name]...,[lower_x_bound]<>[upper_x_bound],[lower_y_bound]<>[upper_y_bound],[x_step_size],[y_step_size]<br>`
 
 
`let f = x^2-4`<br>
`plot f,-5<>5,-5<>15,0.2,0.1`<br>
 you should see the graph of f(x) = x^2-4 from -5 to 5 in the x, and -5 to 15 in the y axis, with measurements taken at increments of 0.2 in the x, and 0.1 the y<br>
</p>
<br>
<h3>Operators Documentation:</h3>
Operator precedence is defined below, in a case where two adjacent operations have the same priority the left most operation will occur first<br>
<br>
  (...) - Parenthesis have the highest priority, and any expression inside them will be calculated before anything outside<br>
  '^' - or the exponentiation operator has the second highest priority, and will return the first operand raised to the power of the
  second<br>
  This operator cannot handle fractional powers, and will return the result as if it was raised to the power of 0<br>
  written like : x^z<br>
  
  'X' , '/' , '%' ,'$' , 'P','C' all have the same third level of priority<br>
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
  
  
  `rightOperand^(1/leftOperand)`<br>
  Although the exponentiation operator cannot handle fractional powers with this operator you should be able to express all rational powers<br>
  <br>
  'P' is the permutation operator from statistics or 
  
  `factorial(leftOperand)/factorial(leftOperand-rightOperand)`<br>
  <br>
  <br>
  'C' is the permutation operator from statistics or 
  
  `factorial(leftOperand)/(factorial(leftOperand-rightOperand)*factorial(rightOperand))`<br>
  <br>
  '+' , '-' after that the next priority level is for addition, and subtraction<br>
  <br>
  '=' , '>' , '<' and finally we have comparison operators with the lowest priority which either return a 1 or a 0<br>
  <br><br>
  <p>
 <h4>Try Running this code</h4>
 

`let sqr =x*0+ (1-((1 - x))/2)`<br>
`let sqr1 = x*0+ (sqr(x)-((sqr(x)^2 - x))/(2*sqr(x)))`<br>
`let sqr2 = x*0+ (sqr1(x)-((sqr1(x)^2 - x))/(2*sqr1(x)))`<br>
`let sqr3 = x*0+ (sqr2(x)-((sqr2(x)^2 - x))/(2*sqr2(x)))`<br>
`let sqr4 = x*0+ (sqr3(x)-((sqr3(x)^2 - x))/(2*sqr3(x)))`<br>
`let sqr5 = x*0+ (sqr4(x)-((sqr4(x)^2 - x))/(2*sqr4(x)))`<br>
`let sqr6 = x*0+ (sqr5(x)-((sqr5(x)^2 - x))/(2*sqr5(x)))`<br>
`let sqr7 = x*0+ (sqr6(x)-((sqr6(x)^2 - x))/(2*sqr6(x)))`<br>
`let sqr8 = x*0+ (sqr7(x)-((sqr7(x)^2 - x))/(2*sqr7(x)))`<br>
`sqr8(49)`<br>
`let rsqrt = when rem>0 +radicand*0 then (rsqrt(rem-1,radicand) - (rsqrt(rem-1,radicand)^2-radicand)/(2*rsqrt(rem-1,radicand))) when not(rem>0) then (1-((1 - radicand))/2) end`<br>
`rsqrt(8,49)`<br>
`let ack = when m=0 + n*0  then n+1 when n=0 then ack(m-1,1) when (m + n)>0 then ack(m-1,ack(m,n-1)) end`<br>
`ack(3,2)`

<h5>Code to write an exponentiation function that only uses addition, and subtraction</h5>


`let mult = when a>0 then b+mult(a-1,b) when not(a>0) then 0 end`<br>
`let optmult = when a>b then mult(b,a) when not(a>b) then mult(a,b) end`<br>
`let pow = when a*b*0 + b>1 then  optmult(a,pow(a,b-1)) when not(b>1) then a end`<br>
`pow(2,5)`
 </p>
 <h5>Code to check if a number is prime, and print a list of primes</h5>
 <p>


```let isprime = (x){
  loc counter = 3
  cloc max = sqrt(x)
  cloc prime = when ((x-1)%2 = 0)then 1 else 0 end
  while not(counter > max)*prime{
    when x%counter = 0 then set prime = 0 else 0 end
    set counter = counter+1
  }
  prime
}

let printprimes = (w,x){
  while (w < x){
    when isprime(w) then print w else 0 end
    set w = w+1
  }
  null
}

