import tac.utils
let insert = []
let insert.sort = (pdata){
    cloc sorted = 0;
    for i in range(1,arrLen(pdata)){
        set sorted = i-1;
        cloc unsorted = i;
        while(not(sorted<0) * try(pdata[sorted]>pdata[unsorted])){
            swap(&pdata[sorted],&pdata[unsorted]);
            set sorted = sorted-1
            set unsorted = unsorted-1
        }
    }
}