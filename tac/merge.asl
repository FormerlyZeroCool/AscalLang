import tac.utils
let merge = {}
let merge.intdata = []
let merge = (dat,start,mid,end){
    #print "merging from (start) to (end)endl"
    cloc counter = 0;
    cloc rindex = mid+1
    cloc lindex = start
    while not(lindex>mid) * not(rindex>end){
        if(dat[lindex]>dat[rindex]=0){
            set this.intdata[counter] = dat[lindex]
            set lindex = lindex+1
        }
        else{
            set this.intdata[counter] = dat[rindex]
            set rindex = rindex+1
        }
        set counter = counter+1
    }
    while not(lindex>mid){
        set this.intdata[counter] = dat[lindex]
        set counter = counter+1;
        set lindex = lindex+1;
    }
    while not(rindex>end){
        set this.intdata[counter] = dat[rindex]
        set counter = counter+1;
        set rindex = rindex+1;
    }
    for x in range(start,end+1){
        set dat[x] = this.intdata[x-start];
    }
}
let merge.sortrm = (arr,start,end){
    if start<end { 
    #print "splitting (start) to (end) endl"
        merge.sortrm(&arr,start,floor((start+end)/2));
        merge.sortrm(&arr,floor((start+end)/2)+1,end);
        merge(&arr,start,floor((start+end)/2),end);
    }
}
let merge.sort = (arr){
    let this.intdata = []
    for x in range(0,arrLen(arr)){
        arrPush(this.intdata,0)
    };
    merge.sortrm(&arr,0,arrLen(arr)-1)
}