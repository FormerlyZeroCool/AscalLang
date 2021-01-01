
let prl = (list){
for x in range(&list){
    printf(x)
}
}

let heap = {}
let heap.data = []
let swap = (x,y){
    cloc z = x;
    set x = y;
    set y = z;
}
let cloneArr = (a,b){
    for x in range(&b){
        arrPush(a,x);
    }
}
let heap.heapify = (pdat){
	cloneArr(&this.data, &pdat);
	for i in range(floor(arrLen(pdat)/2),-1,-1){
		this.down(i,arrLen(this.data)-1);
	}
    print "finished heapifyendl"
}
let heap.heapSort = (dat){
    this.heapify(&dat);
    cloc len = arrLen(dat)-1
	for i in range(arrLen(dat)){
		swap(&this.data[0],&this.data[len - (i)]);
		this.down(0,len - (i+1));
	}
    print "finished heap sortendl"
}
let heap.down = (start,end){
	cloc current = start;
	while not(current*2+1 > end){
		if not(current*2+2>end){
			if ((this.data[current*2+1] > this.data[current*2+2]) * (this.data[current*2+1]>this.data[current])){
				swap(&this.data[current*2+1],&this.data[current]);
				set current = current*2+1;

			}
			else if (this.data[current*2+2]>this.data[current]){
					swap(&this.data[current*2+2],&this.data[current]);
					set current = current*2+2;
			}
			else {
				set current = end;
            }
		}
		else if (not(current*2+1 > end) * (this.data[current*2+1]>this.data[current])){
			swap(&this.data[current*2+1],&this.data[current]);
			set current = current*2+1;
		}
		else {
			set current = end;
        }
	}
}
let heap.clear = let this.data = []

