#include <iostream>
using std::cout;
using std::endl;
#include <fstream>
using namespace std;
#include <cstring>
#include <string.h>
#include <vector>
enum STATUS {SUCCESS, FAILURE};
const int MAX_CHARS_PER_LINE = 512;
const int MAX_TOKENS_PER_LINE = 20;
const char* const DELIMITER = " ";
#include "operandFile_parser.h"
#include <unistd.h>
#include <sstream>
#define GetCurrentDir getcwd
/* Copyright (C) 
 * This program is free software; you can redistribute it and/or2015 - Behzad Boroujerdian
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



/** 
 * @brief this module is responsible for parsing the file that contains the input values for the main module
 * 
 * @param fileName: name of the file that contains the input values
 * @param 
 * @param &inputVar: vector that gets populated with the input values
 * 
 * @return: indicate the status of the module (whether it returned successfully or not)
 */
int operandFileParser(string fileName, vector<int> &inputVar)
{
  // create a file-reading object
   ifstream fin;
  fin.open(fileName); // open a file
 
//
//  char cCurrentPath[FILENAME_MAX];
//
// if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath)))
//     {
//     return errno;
//     }
//
//  cCurrentPath[sizeof(cCurrentPath) - 1] = '\0'; /* not really required */
//
//    printf ("The current working directory is %s", cCurrentPath);
//
//  
//  
  
if (!fin.good()){
      printf("the input file to operandFile not found"); 
      return FAILURE; // exit if file not found
  } 
// read each line of the file
  while (!fin.eof())
  {
    // read an entire line into memory
    char buf[MAX_CHARS_PER_LINE];
    fin.getline(buf, MAX_CHARS_PER_LINE);
    // parse the line into blank-delimited tokens
    int n = 0; // a for-loop index
    
    // array to store memory addresses of the tokens in buf
    const char* token[MAX_TOKENS_PER_LINE] = {}; // initialize to 0
    
    // parse the line
    token[0] = strtok(buf, DELIMITER); // first token
    
    if (token[0]) // zero if line is blank
    {
      
    for (n = 1; n < MAX_TOKENS_PER_LINE; n++)
      {
        token[n] = strtok(0, DELIMITER); // subsequent tokens
        if (!token[n]) break; // no more tokens
      }
    }
    // process (print) the tokens
    if (n != 0) { 
        for (int i = 0; i < n; i++) {// n = #of tokens
            stringstream strValue;
            strValue << token[i];
            int intValue;
            strValue >> intValue;
            inputVar.push_back(intValue);  
        } 
   } 
  }
  return SUCCESS;
//  for (int i =0; i<VTypeInfo.size(); i++){
//     for(int j= 0; j <VTypeInfo[i].size(); j++){
//        cout<<VTypeInfo[i][j]<<endl;
//    }
//  }

}


