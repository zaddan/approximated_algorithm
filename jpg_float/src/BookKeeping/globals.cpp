#include <fstream>
#include "globals.h"
#include "Operators.h"
#include "operatorFile_parser.h"
#include "setSubType.h"
#include "operandFile_parser.h"
#include <iostream>
using namespace std;

hw_ac **myOp;   
//vector<int> inputVar;
//void assign_global_variables(string resultFolderName, string operatorFileName, string operandFileName){
void assign_global_variables(string resultFolderName, string operatorFileName){
    vector<vector<string> > OpTypeVec;
    enum status {SUCCESS, FAILURE}; 
    string OpListFile = resultFolderName;
    OpListFile += "/";
    OpListFile += operatorFileName;


    int status = operatorFileParser(OpListFile, OpTypeVec);
    printf("the OpTypeVec size%d\n", OpTypeVec.size()); 
    cout<<std::flush; 
    myOp = new hw_ac*[OpTypeVec.size()];
    for (int i = 0; i<OpTypeVec.size(); i++) {
        int status = setOpSubTypeAndInputs(&myOp[i], OpTypeVec[i]);
        if (status == FAILURE) {
            printf("this type is not acceptable \n"); 
            exit(1);// 0;
        }
    }

    //status = operandFileParser(operandFileName, inputVar);
    /*if (status == FAILURE){*/
        //cout <<"could not find the operandfile" << endl;
    /*}*/
}
