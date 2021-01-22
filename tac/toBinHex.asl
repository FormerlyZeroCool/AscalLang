
let printBinHex = (num){
    loc len = 0;
    loc rbin = {};
    #print "In decimal: (num)endl";
    while(num>0);
    {
      arrPush(rbin, floor(num%2));
      set num = floor(num/2);
      set len = len+1;
      
    };
    while not(len%4=0);
    {
      arrPush(rbin,0);
      set len = len+1;
      
    };
    loc bin = {};
    loc printBin = 0;
    #print "In bin: ";
    for x in range(0,&rbin,-1)
    {
        if x=1{
            set printBin = 1;
        }
        if printBin{
            print "(x)";
        }
        arrPush(bin,x);
      
    };
    #println(1);
    print " "
    cloc power = (len+3)%4
    loc dec = {};
    for x in range(&bin){
      set num = num+x*2^power;
      if(power=0);
      {
        arrPush(dec,num);
        set num = 0;
        set power = 4;
        
      };
      set power = power-1;
      
    };
    set num = 0;
    loc toHex = (x){
       when x>9 then printChar(x+55) else printChar(x+48) end
    };
    #print "In hex: ";
    for x in range(&dec);
    {
      toHex(x);
      
    };
    println(1);
    
  }
  
