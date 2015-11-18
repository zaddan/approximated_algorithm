/**
* @file RecME.cpp
* @author Muhammad Usman Karim Khan
* Group: Prof. Joerg Henkel at Chair for Embedded Systems (CES), KIT
* @brief This file contains the computational adaptation for both number
* of threads and the frequency of the cores.
*/

#include <fstream>

#include "Operators.h"
#include "Utilities.h"
#include "operatorFile_parser.h"
#include "setSubType.h"
#include "operandFile_parser.h"
#include "globals.h"
#include "foo.h"

using namespace std;
extern hw_ac **myOp;   
//extern vector<int> inputVar;

//int notMain(){ //uncomment when you want to run the run_unit_tests
int main(int argc, char* argv[]){
    string resultFolderName; 
    string resultFileName; 
    string operatorFileName;
    //string operandFileName;
    if (argc < 4) {
        cout<< "provide the name of the file that you want the result to be written to"<<endl;
        cout<< "Example: resultFolderName.txt resultFile.txt operatorFile.txt"<<endl; 
        return 0; 
    }else{
        resultFolderName= argv[1]; 
        resultFileName = argv[2]; 
        operatorFileName = argv[3]; 
    }
    assign_global_variables(resultFolderName, operatorFileName);
    string resultFileNameCompleteAddress = resultFileName;
    ofstream resultFile;
    resultFile.open(resultFileNameCompleteAddress.c_str(), ios_base::app);
    resultFile<<"*****************start******"<<endl; 
    
    
   
    //----------------------------------------------------------------------------------------------------------------------------------------------------------------------------- 
    //----------------------------------------------------------------------------------------------------------------------------------------------------------------------------- 
    //keep the bellow part of the main file intact for various files 
    //get the input from the user 
    const int inputSize = 6; 
    int inputVar[inputSize] = {}; 
    assert(argc == inputSize + 4);
    for (int i =0 ; i < 6; i++) {
        inputVar[i] = atoi(argv[4 + i]);
    }
         
    
    int a = myOp[0]->calc(inputVar[0],inputVar[1]); //MultiplicationOp
    int b = foo(a, inputVar[2]); 
    int d = myOp[1]->calc(b,inputVar[3]); //MultiplicationOp
    int c = myOp[2]->calc(inputVar[4],inputVar[5]); //AdditionOp
    int e = myOp[3]->calc(c,d); //MultiplicationOp
    //keep the above part of the main file intact for various files 
    //writing the result 
     //----------------------------------------------------------------------------------------------------------------------------------------------------------------------------- 
    //----------------------------------------------------------------------------------------------------------------------------------------------------------------------------- 
    int overAllOutput = e; 
    cout <<"asdfasdfasdfasdfasdf"<<endl;
    cout <<overAllOutput<<endl;
    resultFile<< overAllOutput <<endl;
    resultFile<<"*****************end******"<<endl; 
    resultFile.close();
    return 0;
}    
