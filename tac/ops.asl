2+2

let g = when x>0 then g(x-1)*2 else 1 end
let f = (x){
  loc c = 1
  while x>0{
    set x = x-1
    set c = c*2
}
  c
};
let ftw = {
  loc c = 0;
  while 1{
    print "We won! (c) times!endl";
    set c = c+1 ;
    
  };
  
}
let saw = (x,y,z){
  loc n = {
    loc c = 0;
    while c<x{
      print " ";
      set c = c+1;
      
    };
    
  };
  loc c = 0;
  loc w = 1;
  while c<x{
    n(abs(c-floor(c+0.5))*y + 5);
    print "Xendl";
    set c = c+z ;
    if (c>w){
  set w = w+1;
  println(60);
  }
  };
  
}
let isNeg = {
if x<0{
printStr "The number is negative (-1*x)endl"
}
else{
printStr "The number is positive (x)endl"
}
}
let j = {
      if x=0{
        printStr "hello (e)endl";
  
      }else if x=1{
        printStr "endlit'sworking! else ifendlendl";
        
      }else{
	printStr "endlhello world from else (e*100)endl"
        if x>5{
          printStr "endl(x) is greater than 5endl";
          
        }else{
          printStr "endl(x) is not greater than 5endl";
          
        };
        
      };
      
  }
let k = (x){
  let prod = 0
  if x<0{
     set prod = 1
     while x<0{
      set prod = x*prod
      set x = x+1
     }
  }else{
    set prod = 1
    while x>0{
      set prod = x*prod
      set x = x-1
    }
  }
  prod
}

let isprime = (x){
  loc counter = 3
  cloc max = sqrt(x)
  cloc prime = when ((x-1)%2 = 0)then 1 else 0 end
  while (counter<max)*prime{
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


let fibr = when x > 1 then fibr(x-1)+fibr(x-2) else x end


let printfib = (x){
loc counter = 0;
while counter<x{
  print fib(counter);
  set counter = counter+1;
}
}






let h = (x){
    loc c = 0
    while x>0{
      set c = 1+c
      set x = x-1
      }
    print c
}

let game = {
loc count = 0;
while isprime(input "Guess a prime if you guess wrong I exitendl"){
    printStr "You guessed right, good job!endl"
    set count = count+1
  }
  printStr "You guessed wrong!endlBut, you got (count) right!"
}

let game2 = {
loc count = 0;
while log(input "Guess a power of 2 if you guess wrong I exitendl")/log(2)%1 < 0.000001{
    printStr "You guessed right, good job!endl"
    set count = count+1
  }
  printStr "You guessed wrong!endlBut, you got (count) right!"
}

let constant = 1
let linear = (x){
      let counter = 0;
      while counter<x{
        set counter = counter +1;
      };
      counter;    
};
    
let lmc = (x){
    let counter = 0;
    cloc limit = log(x)/log(2);
    print "The limit of iterations: (limit)endl";
    while counter<limit{
      set counter = counter+1;
      
    };
    counter;
    
  }
 
let nlmc = (x){
      let counter = 0;
      cloc limit = x*(log(x)/log(2));
      while counter<limit{
        set counter = counter+1;
        
      };
      counter;
      
    };
let exponential = {
      loc counter = 0;
      cloc limit = 2^x;
      while counter<limit{
        set counter = counter+1;
        
      };
      counter;
      
    };
let poly = {
      loc counter = 0;
      cloc limit = x^2;
      while counter<limit{
        set counter = counter+1;
        
      };
      counter+;
      
    };

let tochar = (x){
      set x = x%26;
      when x = 0 then printStr "A" when x = 2 then printStr "B" when x = 3 then printStr "C" when x = 4 then printStr "D" else printStr "TBD" end
    };
    
  

let rsum = (num){
print "(pi)"
print "hello, world
}
let libby = rsum(x);let rs = {
print "hello, world pi is(pi)endl"
print "Magic (pi) pi is magic yessendl"
libby(x)
}
let anoterFn = rs(x)
let srs = anoterFn(x)


let altmult = (b,a){when a>0 then b+altmult(a-1,b) else 0 end}

let mult = (a,b){when a>0 then b+ mult(a-1,b) else 0 end}

let optmult = (a,b){when a>b then mult(b,a) else mult(a,b) end}

let pow = (a,b){when b>1 then optmult(a,pow(a,b-1)) else 1 end}

