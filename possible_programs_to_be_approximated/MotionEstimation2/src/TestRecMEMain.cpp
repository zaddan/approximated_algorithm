/**
* @file FrameRateInterpParallelMain.cpp
* @author Muhammad Usman Karim Khan
* Group: Prof. Joerg Henkel at Chair for Embedded Systems (CES), KIT
* @brief Test to call the FrameRateInterpParallel class
*/

#include "Utilities.h"
#include "RecME.h"
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#ifdef _MSC_VER
#include <direct.h>
#include <intrin.h>
#include <Windows.h>
#else
#include <libgen.h>	// To get the file name
#include <sys/stat.h>	// To create the director
#endif

#if defined WIN32 || defined WIN64
#define		OS_NAME		"Win"
#else
#define		OS_NAME		"Linux"
#endif

using namespace std;

int main(int argc, char* argv[])
{
	if(argc != 9)
	{
		cout << "The input is not correct." << endl;
		cout << "- Input 8-bits per pixel YUV 420p sequence" << endl;
		cout << "- Image width" << endl;
		cout << "- Image height" << endl;
		cout << "- Number of frames" << endl;
		cout << "- Block size" << endl;
		cout << "- Search window width/height" << endl;
		cout << "- Number of threads" << endl;
		cout << "- Write stats" << endl;
		cout << "Example: foreman_cif.yuv 352 288 300 8 32 1 1" << endl;
		return EXIT_FAILURE;
	}

	for(i32 i=1;i<9;i++)
		cout << argv[i] << " ";
	cout << endl;

	i32 iArgNum = 1;
	i8 pbInputYUVFile[100];
	i8 pbFileName[100];
	i8 pbOutputFileName[100];
	strcpy(pbInputYUVFile, argv[iArgNum++]);
	i32 filenamelen = strlen(pbInputYUVFile);
#if defined _WIN32 || defined _WIN64
	_splitpath(pbInputYUVFile,NULL,NULL,pbFileName,NULL);
	sprintf(pbOutputFileName,"./%s/",pbFileName);	// Create a directory
	_mkdir(pbOutputFileName);
#else
	strcpy(pbFileName,basename(pbInputYUVFile));
	sprintf(pbOutputFileName,"./%s/",pbFileName);	// Create a directory
	mkdir(pbOutputFileName, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif
	ifstream ifsYUVFile;
	ifsYUVFile.open(pbInputYUVFile, ios::binary | ios::in);
	MAKE_SURE(ifsYUVFile.is_open(),"Error: Cannot open input YUV file.");
	
	ofstream ofsMVFile;
	sprintf(pbFileName,"%sMVs.vtxt",pbOutputFileName);
	ofsMVFile.open(pbFileName, ios::out);
	MAKE_SURE(ofsMVFile.is_open(),"Error: Cannot open output MV file.");
	
	i32 iImgWidth = atoi(argv[iArgNum++]);
	i32 iImgHeight = atoi(argv[iArgNum++]);
	i32 iNumFrames = atoi(argv[iArgNum++]);
	i32 iBlkSize = atoi(argv[iArgNum++]);
	i32 iSWSize = atoi(argv[iArgNum++]);
	i32 iNumThreads = atoi(argv[iArgNum++]);
	bit bStats = RET_1_IF_TRUE(atoi(argv[iArgNum++]) > 0);

	byte *pbYUVLeftFrame = new byte[iImgWidth*iImgHeight];
	byte *pbYUVRightFrame = new byte[iImgWidth*iImgHeight];

	RecME *pcRecME = new RecME(0,iImgWidth,iImgHeight,iImgWidth,iBlkSize,iSWSize,iSWSize,10);
	
	ofstream ofsStats;
	if(bStats)
	{
		i8 bHostName[100];
		i8 pbFileName[100];
		sprintf(pbFileName,"%sStats.vtxt",pbOutputFileName);
		ofsStats.open(pbFileName, ios::out);
		MAKE_SURE(ofsStats.is_open(),"Error: Cannot open Statistics file.");
		// Write initial
		GetMachineName(bHostName,100);
		ofsStats<<"Frame Motion Estimation (by Muhammad Usman Karim Khan @ CES, KIT)" << endl;
		ofsStats<<"Only luma frames are estimation, chroma frames are not processed." << endl;
		ofsStats<<"Build date " << __DATE__ << endl;
		ofsStats<< (sizeof(void*)==8?64:32) << "-bits on " << bHostName <<"[" << OS_NAME <<"]" << endl;
		i8 piBuff[80];
		GetCurrentDateTime(piBuff,80);
		ofsStats<<"File generated on "<< piBuff << endl;
		ofsStats<<"Input file: "<< pbInputYUVFile << endl;
		ofsStats<<"Image resolution: " << iImgWidth << "x" << iImgHeight << endl;
		ofsStats<<"Number of frames: " << iNumFrames << endl;
		ofsStats<<"Search window size: " << iSWSize << "x" << iSWSize << endl;
		ofsStats<<"Total threads: " << iNumThreads << endl;
		ofsStats<<"Data written in the following format" << endl;
		ofsStats<<"Frames_num Frames_time Tile_time Tile_SADs Frame_PSNR" << endl;
	}
	for(i32 i=0;i<iNumFrames;i+=2)
	{
		// Read the left (reference) frame
		ifsYUVFile.read((i8 *)pbYUVLeftFrame,iImgWidth*iImgHeight);
		ifsYUVFile.seekg(iImgWidth*iImgHeight/2,ios_base::cur);

		// Read the right (current) frame
		ifsYUVFile.read((i8 *)pbYUVRightFrame,iImgWidth*iImgHeight);
		ifsYUVFile.seekg(iImgWidth*iImgHeight/2,ios_base::cur);
		
		// Get the motion vectors
		pcRecME->MotionEstimation(pbYUVLeftFrame,pbYUVRightFrame,0,0,0,0);
		
		// Write the outputs
		i8 *pbMVX = NULL, *pbMVY = NULL;
		pcRecME->GetMV(&pbMVX,&pbMVY);
		for(i32 j=0;j<pcRecME->GetImgSizeInBlks();j++)
			ofsMVFile << j << "\t" << i32(pbMVX[j]) <<"\t" << i32(pbMVY[j]) <<endl;

		if(bStats)
		{
			ofsStats << i+1 << "\t" << pcRecME->GetTimePerFrame() << "\t";
			ofsStats << pcRecME->GetNumSAD() << "\t" << endl;
		}

		cout << "Frame "<< i+1 <<" done..." << endl;
	}

	ifsYUVFile.close();
	ofsMVFile.close();
	ofsStats.close();

	delete [] pbYUVLeftFrame;
	delete [] pbYUVRightFrame;

	delete pcRecME;
	return EXIT_SUCCESS;
}