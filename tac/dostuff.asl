
clear

let f = {
    
    cloc val = input "Enter a number: ";
    cloc min = val;
    cloc max = val;
    while val>0{
      if min>val{
        set min = val;
        
      };
      if max<val{
        set min = val;
        
      };
      set val = input "Enter a number: ";
      
    };
    if min>0{
      printf(min);
      printf(max);
    }
  }
f






