/**
* @file RecME.cpp
* @author Muhammad Usman Karim Khan
* Group: Prof. Joerg Henkel at Chair for Embedded Systems (CES), KIT
* @brief This file contains the computational adaptation for both number
* of threads and the frequency of the cores.
*/

#include "Utilities.h"
#include "RecME.h"

#define		BLK_SIZE		8						//!< Default Blk size
#define		SW_WIDTH		32						//!< Default SW width
#define		SW_HEIGHT		32						//!< Default SW height

RecME::RecME(i32 iID, i32 iImgWidth, i32 iImgHeight, i32 iImgStride, i32 iBlkSize, i32 iSWWidth, i32 iSWHeight, i32 iSearchStrength):
	m_iID(iID), m_iImgWidth(iImgWidth), m_iImgHeight(iImgHeight), m_iImgStride(iImgStride),m_iSearchStrength(iSearchStrength)
{
	m_iBlkSize = (iBlkSize < 1 ? BLK_SIZE : iBlkSize);
	m_iSWWidth = min((iSWWidth < 1 ? SW_WIDTH : iSWWidth), 256);		// 8-bit MV
	m_iSWHeight = min((iSWHeight < 1 ? SW_HEIGHT : iSWHeight), 256);	// 8-bit MV

	// Only utilize a square search window for now.
	// Make the search window of odd size to have a mid-point
	m_iSWWidth = max(m_iSWWidth,m_iSWHeight) | 0x1;
	m_iSWHeight = max(m_iSWWidth,m_iSWHeight) | 0x1;

	m_iImgWidthInBlks = m_iImgWidth/m_iBlkSize;
	m_iImgHeightInBlks = m_iImgHeight/m_iBlkSize;
	m_iBlksPerFrame = m_iImgWidthInBlks*m_iImgHeightInBlks;

	m_ppbMVRightToLeft[0] = new i8[m_iBlksPerFrame];
	m_ppbMVRightToLeft[1] = new i8[m_iBlksPerFrame];

	m_piSAD = new i32[m_iBlksPerFrame];
}

i32 RecME::BlockDiff(byte *pbInput0, i32 iStride0, byte *pbInput1, i32 iStride1, i32 iBlockSize, i32 iSkipPel)
{
	byte *pbIn0, *pbIn1;
	i32 iSAD = 0;
	for(i32 y=0;y<iBlockSize;y+=(1+iSkipPel))
	{
		m_iNumSADs++;
		pbIn0 = pbInput0 + y*iStride0;
		pbIn1 = pbInput1 + y*iStride0;
		for(i32 x=0;x<iBlockSize;x+=(1+iSkipPel))
			iSAD += ABS(i32(pbIn0[x]) - i32(pbIn1[x]));
	}
	return iSAD;
}

i32 RecME::GetMinSADPt(byte *pbInputBlk, i32 iBlkStride, i32 iBlkSize, byte *pbSearchWin, 
							  i32 iSWWidth, i32 iSWHeight, i32 iSWStride, i32 & iMinPtX, i32 & iMinPtY)
{
#define		COMPUTE_SAD																		\
	do{																						\
		pbRefBlk = pbSearchWin+y*iSWStride+x;												\
		iSAD = BlockDiff(pbInputBlk,iBlkStride,pbRefBlk,iSWStride,iBlkSize,0);				\
		iNumSADs++;																			\
		if(iSAD <= iMinSAD)																	\
		{																					\
			iMinPtX = x;																	\
			iMinPtY = y;																	\
			iMinSAD = iSAD;																	\
		}																					\
	}while(0)

	i32 iSAD;
	i32 iMinSAD = 99999999;
	byte *pbRefBlk;
	i32 iNumSADs = 0;

	for(i32 y=0;y<iSWHeight;y++)
	{
		for(i32 x=0;x<iSWWidth;x++)
		{
			COMPUTE_SAD;
		}
	}

	MAKE_SURE(iMinPtX >=0 && iMinPtX < iSWWidth && iMinPtY >= 0 && iMinPtY < iSWHeight, 
		"Error: The minimum point is outside the window");
	return iMinSAD;
}

void RecME::GenerateMVs(byte *pbRefFrame, byte *pbCurrFrame, 
	i32 iPelsAvailLeft, i32 iPelsAvailRight, i32 iPelsAvailTop, i32 iPelsAvailBottom)
{
	i32 iSWMinX, iSWMaxX, iSWMinY, iSWMaxY, iSWWidth, iSWHeight, iMinPtX, iMinPtY;
	i32 x,y;
	i32 iPelMinX = -iPelsAvailLeft;
	i32 iPelMinY = -iPelsAvailTop;
	i32 iPelMaxX = m_iImgWidth+iPelsAvailRight;
	i32 iPelMaxY = m_iImgHeight+iPelsAvailBottom;
	i32 iBlkNum = 0;
	i32 iStartMVX, iStartMVY;

	y = 0;
	x = 0;
	for(i32 i=0;i<m_iImgHeightInBlks;i++,x=0,y+=m_iBlkSize)
	{
		for(i32 j=0;j<m_iImgWidthInBlks;j++,iBlkNum++,x+=m_iBlkSize)
		{
			// Estimate the start MV in the search window for the motion estimation
			// Currently its 0, but it can be a better guess (like PMV in H.264)
			iStartMVX = 0;
			iStartMVY = 0;
			
			// Search window is around the predicted MV
			iSWMinX = max(x+iStartMVX-m_iSWWidth/2,iPelMinX);
			iSWMinY = max(y+iStartMVY-m_iSWHeight/2,iPelMinY);
			iSWMaxX = min(x+iStartMVX+m_iSWWidth/2,iPelMaxX-m_iBlkSize);
			iSWMaxY = min(y+iStartMVY+m_iSWHeight/2,iPelMaxY-m_iBlkSize);
			iSWWidth = iSWMaxX-iSWMinX+1;
			iSWHeight = iSWMaxY-iSWMinY+1;

			// Starting search point in the search window
			iMinPtX = x+iStartMVX-iSWMinX;
			iMinPtY = y+iStartMVY-iSWMinY;

			// Get the minimum point SAD inside the search window.
			m_piSAD[iBlkNum] = GetMinSADPt(&pbCurrFrame[y*m_iImgStride+x],m_iImgStride,m_iBlkSize, &pbRefFrame[iSWMinY*m_iImgStride+iSWMinX],
				iSWWidth,iSWHeight,m_iImgStride,iMinPtX,iMinPtY);

			// Generate the MV, from the current frame to the reference frame
			m_ppbMVRightToLeft[0][iBlkNum] = iSWMinX+iMinPtX-x;
			m_ppbMVRightToLeft[1][iBlkNum] = iSWMinY+iMinPtY-y;
		}
	}
}

void RecME::MotionEstimation(byte *pbLeftFrame, byte *pbRightFrame, i32 iPelsAvailLeft, i32 iPelsAvailRight,
		i32 iPelsAvailTop, i32 iPelsAvailBottom)
{
	m_iNumSADs = 0;
	m_uiTime = GetTimeInMiliSec();

	// Generate the motion vectors from the right to the left frame.
	// Use only the first channel
	GenerateMVs(pbLeftFrame, pbRightFrame, iPelsAvailLeft, iPelsAvailRight, iPelsAvailTop, iPelsAvailBottom);
	m_uiTime = GetTimeInMiliSec()-m_uiTime;
}

void RecME::GetMV(i8 **pbMVX, i8 **pbMVY)
{
	*pbMVX = m_ppbMVRightToLeft[0];
	*pbMVY = m_ppbMVRightToLeft[1];
}

void RecME::GetMV(i32 iBlkIdx, i8 & iMVX, i8 & iMVY)
{
	iMVX = m_ppbMVRightToLeft[0][iBlkIdx];
	iMVY = m_ppbMVRightToLeft[1][iBlkIdx];
}

void RecME::GetMV(i32 iX, i32 iY, i8 & iMVX, i8 & iMVY)
{
	i32 iBlkIdx = iY/m_iBlkSize*m_iImgWidthInBlks + iX/m_iBlkSize;
	iMVX = m_ppbMVRightToLeft[0][iBlkIdx];
	iMVY = m_ppbMVRightToLeft[1][iBlkIdx];
}

RecME::~RecME()
{
	delete [] m_ppbMVRightToLeft[0];
	delete [] m_ppbMVRightToLeft[1];
	delete [] m_piSAD;
}