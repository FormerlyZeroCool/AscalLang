let f = {
    loc n = (x){
      loc c = 0;
      while c<x{
        print " ";
        set c = c+1;
        
      };
      
    };
    loc quitl = (x){
      if(x < 1){
        print "Take careendl";
        quit;
        
      };
      
    };
    cloc iterations = input "How many lines should I print? ";
    quitl(iterations);
    cloc max = input "Enter an amplitude: ";
    loc x = 0;
    loc y = 0;
    if(iterations = 0){
      set iterations = -1;
      
    };
    pause;
    while True{
      set y = y+1;
      if(y%(iterations) = 0){
        set y = 0;
        loc x = 0;
        set iterations = input "How many lines should I print? ";
        quitl(iterations);
        set max = input "Enter an amplitude: ";
        quitl(max);
        
      };
      set x = (x+1)%max;
      n(max+cos(x/(max/(2*pi)))* max);
      print "Hello!endl";
      sleep(20);
      
    };
    
 };