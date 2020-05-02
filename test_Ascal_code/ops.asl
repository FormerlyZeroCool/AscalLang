2+2
  let g = (x){
  loc c = 1
  while x<0{
    set c = x*c
    set x = x+1
  }
  c
};
let f = (x,z){
  loc c = 1
  while x>0{
    set c = z*c
    set x = x-1
  }
  print c
};

let isprime = (x){
  loc counter = 2
  cloc max = sqrt(x)
  cloc prime = when ((x-1)%2 = 0)then 1 else 0 end
  while not(counter > max)*prime{
    when x%counter = 0 then set prime = 0 else 0 end
    set counter = counter+1
  }
  prime
}

let printprimes = (w,x){
  loc counter = w
  while (counter < x){
    when isprime(counter) then print counter else 0 end
    set counter = counter+1
  }
  null
}
printprimes(10000,10050)



