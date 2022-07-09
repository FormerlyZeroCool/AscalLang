let f = {
let bits = [];
let limit = 50;
for x in range(floor((limit+1)/2)){
    arrPush(bits,1);
}
let getbit = (index){bits[floor(index/2)]};
let clearBit = (index){set bits[floor(index/2)] = 0;};

printf(getbit(2));

 let factor = 3;
 const q = floor(sqrt(arrLen(bits)));

    let dx = 1;
while (factor < q){
    set dx = 1;
    for num in range (factor, arrLen(bits),1){
                if (getbit(num) = True){
                    set factor = num;
                    set dx = arrLen(bits);
                }
        }
            for num in range (factor * 3, arrLen(bits), factor * 2){ 
                clearBit(num)
            }

            set factor = factor+2
}
}