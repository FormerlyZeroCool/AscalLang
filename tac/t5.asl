
import tac.merge
import tac.quick
import tac.heap
import tac.insertion
t 1
cloc arr = []
cloc arr2 = []
cloc arr3 = []
cloc arr4 = []
cloc nrand = input "type the amount of numbers to be sorted "
for x in range(nrand){
    arrPush(arr,abs(rand)%10+48)
    arrPush(arr2,when rand%100<55 then abs(rand)%26+65 else abs(rand%10)+48 end)
    arrPush(arr3,abs(rand)%10+48)
    arrPush(arr4,abs(rand)%10+48)
}
print "endlQuick sorting (arrLen(arr)) random numbers"
print quick.sort(&arr2)
for x in range(&arr2){
    printChar(x)
}
    print "endl"
       
print "endlMerge sorting (arrLen(arr)) random numbers"
print merge.sort(&arr)
for x in range(&arr){
    printChar(x)
}
    print "endl"
    
print "endlHeap sorting (arrLen(arr)) random numbers"
print heap.heapSort(&arr3)
for x in range(&heap.data){
    printChar(x)
}
    print "endl"
    
print "endlInsert sorting (arrLen(arr)) random numbers"
print insert.sort(&arr4)
for x in range(&arr4){
    printChar(x)
}
    print "endl"
    
t 0