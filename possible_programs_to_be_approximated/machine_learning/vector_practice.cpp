#include <stdio.h>
#include <vector>
#include <math.h>
#include <iostream>
using namespace std;
int main() {
  printf("hello\n");
  vector<double> myVct1;
  vector<double> myVct2;
  for (int i=0; i<10; i++) 
    myVct1.push_back(i);
 
 for (int i=0; i<10; i++) 
    myVct2.push_back(2*i);
  
  vector< vector<double> > myVctVct;
  myVctVct.push_back(myVct1);
  myVctVct.push_back(myVct2);
   


  for (int i =0; i <myVctVct.size(); i++) {
      for (int j= 0; j<10; j++) 
          cout<< myVctVct[i][j]<<endl;;
  }

}
