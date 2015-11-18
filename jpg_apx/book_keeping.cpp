#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>

using namespace std;

/** 
 * @file book_keeping.cpp
 * @brief: this file is a collection of modules to take care of file handlings, such as writing, readin,
 * removing, etc
 * @author Behzad Boroujerdian
 * @date 2015-07-06
 */


/** 
 * @brief writing an array of values in the output (we need to cast the values to float before 
 * doing so
 * 
 * @param outputFileName
 * @param output
 * @param outputLength
 * @param openningStatement
 * @param closingStatment
 */
void writeOutput(string outputFileName, float *output, int outputLength, 
        string openningStatement, string closingStatment) {

    ofstream myfile;
    myfile.open(outputFileName.c_str(), std::ios_base::app); //this lines opens the file for appending(not just
                                                             //writing
    //myfile<< openningStatement << "\n";
    for (int i=0; i < outputLength; i++) {
        myfile << output[i] << "\n";
    }
    //myfile<< "\n"<< closingStatment << "\n";
    myfile.close();
}

void writeOutput(string outputFileName, short int *output, int outputLength, 
        string openningStatement, string closingStatment) {

    ofstream myfile;
    myfile.open(outputFileName.c_str(), std::ios_base::app); //this lines opens the file for appending(not just
                                                             //writing
    //myfile<< openningStatement << "\n";
    for (int i=0; i < outputLength; i++) {
        myfile << output[i] << "\n";
    }
    //myfile<< "\n"<< closingStatment << "\n";
    myfile.close();
}



/** 
 * @brief writing only one value to the output
 * 
 * @param outputFileName
 * @param output
 * @param openningStatement
 * @param closingStatment
 */
void writeOutput(string outputFileName, float output, string openningStatement, string closingStatment) {

    ofstream myfile;
    myfile.open(outputFileName.c_str(), std::ios_base::app); //this lines opens the file for appending(not just
//    myfile << openningStatement << "\n";
    myfile<<output << "\n";
//    myfile << closingStatment << "\n";
    myfile.close();
}

//
//---------guide::: testing the functionality of above module
//int main () {
//    string fileToWriteIn = "test_file.txt";
//    float fileInput[4] = {1,2,3,4};
//    string myString = "here";
//    writeOutput(fileToWriteIn, fileInput, 4, myString, "end");
//    return 0;
//}

/** 
 * @brief removing the file
 * 
 * @param outputFileName
 */
void rmOutputFile(string outputFileName) {
    //if file exists 
    ofstream file(outputFileName.c_str());
    if (file){
        remove(outputFileName.c_str());
    }
}
