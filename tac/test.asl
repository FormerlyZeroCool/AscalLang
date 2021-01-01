
import tac.toBinHex
srand(42+rand)
rand+rand
printBinHex(234)
printBinHex(2349497868)
#testing for loops with lists
loc limit = 10
let g = {}
for x in range(limit){
  cloc ra = abs(rand)%100;
 arrPush(g,when ra>65 then 97 when ra>30 then 65 else 48 end+ra%26)
 #printChar(arrGet(g,x))
}

 arrPush(g,10)
let printstr = (g){
for x in range(&g){
  printChar(x)
}
}
printf(34)
for x in range(10){
printStr(g)
}
printf(34)
loadStr(g, "Hello World!\n")
printStr(g)
let s = []
loadStr(s,"May")
g.May
#testing memoization, recursion, and locally scoped recursive functions
printf(fibr(10))
#testing while loop
printf(fib(10))
#testing multi-variable functions
printf(sumBetween(0,10))
#testing recursion
printf(ack(3,3))
#testing some trig keywords
printf(cos(1)^2+sin(1)^2)
#testing complex function declaration, and calling
let root = (c,b){
   loc result = 100;
    if(b>0)
    {
        cloc base = b
	loc one = 1;
        cloc newpow = c-one;
        cloc delta =  1
	let prev;
        cloc count = 0;
        while (delta > 0.00001) * (count < 10000){
            set prev = result;
            set result = result - ((result^c) - base)/((result^newpow)*c);
            set delta = result - prev;
            if(delta < 0){
                set delta = delta*-1;
	    }
            set count = count+1;
        }
    print "Iterations for calculation: "
    printf(count);
    }else{
      set result = 0;
    }
result
}
let pb = {if not(x=0) {print "true"} else {print "false"}}
print("The sqrt of 1024 is 32 ")
when root(2,1024)=32 then pb(1) else  pb(0) end
println(1)
let f = (x){
  if x{
    f(x-1)+1
  }
  else{
    0
  }
}
let g = when x>0 then g(x-1)+1 else 0 end

printf(sumOneTo(10))
let sd = []
let gh = (x){set x = 4}
gh(&sd)
printf(sd)