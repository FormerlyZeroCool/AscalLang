let quick = []
let quick.partition = (arr,first,last){
    cloc smaller = first;
    swap(&arr[first],&arr[floor((first+last)/2)]);
    cloc dataAtpivot = arr[first];
    for x in range(first+1,last+1){
        if(dataAtpivot>arr[x]){
            set smaller = smaller+1
            swap(&arr[x],&arr[smaller]);
        }
    }
    swap(&arr[first],&arr[smaller]);
    smaller;
}
let quick.quicksortr = (arr,start,end){
    if(start<end){
        cloc part = this.partition(&arr,&start,&end);
        this.quicksortr(&arr,start,part-1);
        this.quicksortr(&arr,part+1,end)
    }
}
let quick.sort = (arr){
    quick.quicksortr(&arr,0,arrLen(arr)-1)
}