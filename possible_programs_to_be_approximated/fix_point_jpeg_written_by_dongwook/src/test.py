import math
for x in range(0,8):
  for u in range(0,8):
    if(u==0):
      cu = 1.0/1.4142135623730951
    else:
      cu = 1.0;
    coeff= cu  * math.cos( ((2*x+1) * u * math.pi) / 16) 
    print coeff,x,u,cu
