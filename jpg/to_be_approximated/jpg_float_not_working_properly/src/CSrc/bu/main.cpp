/***************************************************************************/
/*                                                                         */
/*  File: main.cpp                                                         */
/*  Autor: bkenwright@xbdev.net                                            */
/*  URL: www.xbdev.net                                                     */
/*                                                                         */
/***************************************************************************/
/*
	Jpeg File Format Explained
*/
/***************************************************************************/


#include <stdio.h>		// sprintf(..), fopen(..)
#include <stdlib.h>
#include <stdarg.h>     // So we can use ... (in dprintf)
#include <iostream>
#include <fstream>
#include "loadjpg.h"	// ConvertJpgFile(..)
#include "savejpg.h"    // SaveJpgFile(..)
#include <cstring>
#include "globals.h"
#include "comparebmp.h"
//extern hw_ac **myOp;   
using namespace std;
/***************************************************************************/
/*                                                                         */
/* FeedBack Data                                                           */
/*                                                                         */
/***************************************************************************/

//Saving debug information to a log file
void dprintf(const char *fmt, ...) 
{

    //printf(fmt, "%s")
    
    va_list parms;
    char buf[256];

    // Try to print in the allocated space.
    va_start(parms, fmt);
    vsprintf (buf, fmt, parms);
    va_end(parms);

    // Write the information out to a txt file
    FILE *fp = fopen("output.txt", "a+");
    fprintf(fp, "%s", buf);
    printf("%s", buf);
    fclose(fp);

}// End dprintf(..)


 
/***************************************************************************/
/*                                                                         */
/* Entry Point                                                             */
/*                                                                         */
/***************************************************************************/

extern BYTE scalefactor ;
int main(int argc, char** argv)
{
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
    
    const int inputSize = 3; 
    assert(argc == inputSize + 4);
    scalefactor=atoi(argv[4]);
    //string inputPictureFolder(argv[5]);
    string inputFileNameString(argv[4]);
     string inputPictureFolder = "./";
    inputFileNameString = inputPictureFolder + inputFileNameString;
    cout<<inputFileNameString <<endl;    
    string tempString = "";
    //trimming the .bmp suffix 
    for (int i =0; i < inputFileNameString.length() - 4; i++){
        tempString += inputFileNameString[i]; 
    }
    //genearting the char* necessary to pass to the sub_modules 
    string inputFileNameAsFinalStringBMP =  tempString + "_final.bmp";
    string inputFileNameAsTempStringJPG =  tempString + "_temp.jpg";
    char * inputFileName  = new char[inputFileNameString.length()+1];
    std::strcpy (inputFileName, inputFileNameString.c_str()); 
    char * inputFileNameAsFinalBMP  = new char[inputFileNameAsFinalStringBMP.length()+1];
    std::strcpy (inputFileNameAsFinalBMP, inputFileNameAsFinalStringBMP.c_str()); 
    char * inputFileNameAsTempJPG  = new char[inputFileNameAsTempStringJPG.length()+1];
    std::strcpy (inputFileNameAsTempJPG, inputFileNameAsTempStringJPG.c_str()); 
    int b = 5; 
    //int x =  myOp[0]->calc(b, b); //AdditionOp
    /*cout<< inputFileName <<endl;*/
    //cout <<inputFileNameAsTempJPG <<endl;
    /*cout <<inputFileNameAsFinalBMP<<endl;*/
    //encode and save 
    //SaveJpgFile("lena.bmp", "lena_tmp.jpg");
    //ConvertJpgFile("lena_tmp.jpg", "lena_final.bmp");
    //comparebmp("lena.bmp","lena_final.bmp");
    
    SaveJpgFile(inputFileName, inputFileNameAsTempJPG);
    ConvertJpgFile(inputFileNameAsTempJPG, inputFileNameAsFinalBMP);
    char * testinputFileName  = new char[inputFileNameString.length()+1];
    std::strcpy (testinputFileName, inputFileNameString.c_str()); 
 
    double psnr = comparebmp(testinputFileName, inputFileNameAsFinalBMP);
     //----------------------------------------------------------------------------------------------------------------------------------------------------------------------------- 
    //----------------------------------------------------------------------------------------------------------------------------------------------------------------------------- 

    int overAllOutput = psnr; 
    resultFile<< overAllOutput <<endl;
    resultFile<<"*****************end******"<<endl; 
    resultFile.close();
    //ConvertJpgFile("imageJPEG3_1.jpg", "imageJPEG3_1.bmp");
    //ConvertJpgFile("imageJPEG3_2.jpg", "imageJPEG3_2.bmp");
	//ConvertJpgFile("test.jpg", "test.bmp");
	//ConvertJpgFile("cross.jpg", "cross.bmp");
	return 10;
}// End WinMain(..)


