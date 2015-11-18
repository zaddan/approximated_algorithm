#include <fstream>
#include "Operators.h"
#include "Utilities.h"
#include "operatorFile_parser.h"
#include "setSubType.h"
#include "operandFile_parser.h"
#include "globals.h"
#include "foo.h"
extern hw_ac **myOp;   
//extern vector<int> inputVar;

int foo(int a, int inputVar){
    int b = myOp[4]->calc(a,inputVar); //AdditionOp
    return b;
}
