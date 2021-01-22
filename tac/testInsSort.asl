import tac.insertion
cloc randnum = input "enter the amount of numbers to sort with insertion sortendl"
let s = []
for x in range(randnum){
    arrPush(s,48+abs(rand%10))
}
print "Time to run insertion sort on unsorted list (randnum) longendl"
print insert.sort(&s)
print "Time to run insertion sort on sorted list (randnum) longendl"
print insert.sort(&s)
printStr(s)