#include "eta1.h"
#include "eta2.h"
//#include "etm.h"  
#include "bta.h" 
#include "btm.h"
#include "loa.h"
#include "hw_ac.h"
#include "operatorFile_parser.h"
#include <vector>
#include <string>
#include "setSubType.h"
/* Copyright (C) 
 * This program is free software; you can redistribute it and/or2015 - Behzad Boroujedian
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */




//#define  testFramWork 0 //uncomment so that the file become a standalone testing frame work

enum STATUS {SUCCES, FAILURE};



//setting the operation subType (eta1, btm) and all the inputs for it. In this case type is add, mul, .. and subType is the subType of each of the 
//the mentioned types
/** 
 * @brief: responsible for instantiating the operators found in the main file. After generating the apx version of the operaters (in python) and reading the generated operators (in operatorFile_parser.cpp), this module, finally instantiates the operators
 * 
 * @param myOp: operators to instantiate
 * @param TypeInfo: operators to instantiate with
 * 
 * @return: failure/success status
 */
int setOpSubTypeAndInputs(hw_ac **myOp, vector<string> TypeInfo){
    string typeSpecifier = TypeInfo[0]; 
    size_t Nt; 
    size_t Nia; 
    size_t msb;
    size_t lsb;
    size_t hbl;
    size_t vbl;
    bool table_gen = 0;
  //take out the comment section if you want to add eta1, and 2 but they are not defined
  //for all the overloaded functions. So that needs to be added
    /* 
    if (typeSpecifier == "eta1") {
      //string stream is used to convert the string to size_t 
      stringstream ( TypeInfo[1]) >> Nt;
      stringstream ( TypeInfo[2]) >> Nia;
      stringstream ( TypeInfo[3]) >> msb;
      stringstream ( TypeInfo[4]) >> lsb;
      *myOp = new eta1(Nt, Nia, msb, lsb, table_gen);
    }else if (typeSpecifier == "eta2"){
      //string stream is used to convert the string to size_t 
      stringstream ( TypeInfo[1]) >> Nt;
      stringstream ( TypeInfo[2]) >> Nia;
      stringstream ( TypeInfo[3]) >> msb;
      stringstream ( TypeInfo[4]) >> lsb;
      *myOp = new eta2(Nt, Nia, msb, lsb, table_gen);
    }else if (typeSpecifier == "loa"){
      //string stream is used to convert the string to size_t 
      stringstream ( TypeInfo[1]) >> Nt;
      stringstream ( TypeInfo[2]) >> Nia;
      stringstream ( TypeInfo[3]) >> msb;
      stringstream ( TypeInfo[4]) >> lsb;
      *myOp = new loa(Nt, Nia, msb, lsb, table_gen);
//    }else if (typeSpecifier == "etm"){
//      //string stream is used to convert the string to size_t 
//      stringstream ( TypeInfo[1]) >> Nt;
//      stringstream ( TypeInfo[2]) >> Nia;
//      *myOp = new etm(Nt, Nia, table_gen);
//    }
}
*/
if (typeSpecifier == "bta"){
      //string stream is used to convert the string to size_t 
      stringstream ( TypeInfo[1]) >> Nt;
      stringstream ( TypeInfo[2]) >> Nia;
      stringstream ( TypeInfo[3]) >> msb;
      stringstream ( TypeInfo[4]) >> lsb;
      *myOp = new bta(Nt, Nia, msb, lsb, table_gen);
    }else if (typeSpecifier == "btm"){
      //string stream is used to convert the string to size_t 
      stringstream ( TypeInfo[1]) >> Nt;
      stringstream ( TypeInfo[2]) >> Nia;
      stringstream ( TypeInfo[2]) >> vbl;
      *myOp = new btm(Nt, Nia, table_gen);
    } 
    else {
      return FAILURE;
    }
    return SUCCES;
}

//testing framWork
#ifdef testFramWork
int main() {
    int var1 = 5;
    int var2 = 6; //MultiplicationOp AdditionOp
   
    string OpListFile; 
    if (argc != 2) {
        cout<< "provide the name of the file that you want the result to be written to"<<endl;
        cout<< "Example: resultFolderName.txt resultFile.txt operatorFile.txt"<<endl; 
        return 0; 
    }else{
        OpListFile = argv[1]; 
    }
    
    //parse the OpListFile to get the list of operators
    vector<vector<string> > OpTypeVec;
    int status = operatorFileParser(OpListFile, OpTypeVec);
    
    //defiing an array of MyOps 
    hw_ac **myOp = new hw_ac*[OpTypeVec.size()];
    //instantiating the array elements to the values of OpTypeVec 
    //note: OpTypeVec is populated with the parsed values in the OpListFile 
    for (int i = 0; i<OpTypeVec.size(); i++) {
        int status = setOpSubTypeAndInputs(&myOp[i], OpTypeVec[i]);
        if (status == FAILURE) {
            printf("this type is not acceptable \n"); 
            return 0;
        }
    } 
    
    for (int i = 0; i<OpTypeVec.size(); i++) {
      int result = myOp[i]->calc(var1, var2); 
        printf("this is the return value %d\n", result);
    } 
    return 1;
}
#endif
