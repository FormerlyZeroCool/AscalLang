o 1 
let printSymbol = (){ print "*" }
let lineBreak = (){ print "endl"; }

const height = input "Type in the height of the triangle: endl"
const i = 0
while i < height {
  const j = 0
  while j < i+1 {
    printSymbol()
    set j = j+1
    pause
  }
  set i = i+1
  lineBreak()
  pause
}
