let s = {}
let s.may = 4
let s.may.first = 2
let s.f = printf(this.may.first)
#print var s
s.f()
let s.may = xc^2
printf(s.may(9))

let swap = (x,y){
    cloc z = x;
    set x = y;
    set y = z;
}
let a = 7
let b = 9
swap(&a,&b)
printf(a)
printf(b)