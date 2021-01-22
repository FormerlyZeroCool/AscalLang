let determin4 = (a,b,c,d){
    a*d-c*b
}
let cross3d = (&a,&b){
    let res = {}
    let res.i = determin4(a.j,a.k,b.j,b.k);
    let res.j = determin4(a.i,a.k,b.i,b.k);
    let res.k = determin4(a.i,a.j,b.i,b.j);
    return res;
}