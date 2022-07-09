let f = []
for x in range(10){arrPush(f,abs(rand%10))};
let prl = (list){
for x in range(&list){
    printf(x)
}
}

import tac.utils
let heap = {}
let heap.data = []
let cloneArr = (a,b){
    for x in range(&b){
        arrPush(a,x);
    }
}
let heap.heapify = (pdat){
	let this.data = pdat;
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
	while (current*2+1 > end)=0{
		if (current*2+2>end)=0{
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
		else if ((current*2+1 > end)=0) * (this.data[current*2+1]>this.data[current]){
			swap(&this.data[current*2+1],&this.data[current]);
			set current = current*2+1;
		}
		else {
			set current = end;
        }
	}
}
let heap.rdown = (start,end){
	cloc c = end;
	when exists 
}
let heap.clear = let this.data = []


