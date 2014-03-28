/**
helpCalib (version 1.0.0)
A part of FluoreScoreSuite 
Input: Intrinsic and extrinsic parameters
Ouput: projection matrices of cameras (projmat.txt)
for more details please visit http:\\fluorescore.cmb.usc.edu
Written by Ravi Prakash (raviprak@usc.edu)
Updated by Reza Ardekani (dehestan@usc.edu)
April 2012
*/

#include <iostream>
#include <fstream>
#include "cv.h"

using namespace std;

void getMatrix(fstream &theFile, CvMat* toFill)
{
	double dTemp;
	for(int i=0; i < toFill->rows; ++i)
	{
		for(int j=0; j < toFill->cols; ++j)
		{
			theFile>>dTemp;
			cvSetReal2D(toFill, i, j, dTemp);
		}
	}
}

void skip(fstream &theFile, int numToSkip)
{
	double dTemp;
	for(int i=0; i < numToSkip; ++i)
		theFile>>dTemp;
}

bool getProjectionMatrix(fstream &intParam, fstream &extParam, CvMat *projM)
{
	if(projM->rows != 3 || projM->cols != 4 )
	{
		printf("\ngetProjectionMatrix got cvMat of wrong size. Need 3x4 matrix");
		return false;
	}
	CvMat *K, *R, *t;
	K = cvCreateMat(3, 3, projM->type);	
	R = cvCreateMat(3, 3, projM->type);
	t = cvCreateMat(3, 1, projM->type);

	getMatrix(intParam, K);
	getMatrix(extParam, R);
	skip(extParam, 3);
	getMatrix(extParam, t);

	CvMat *temp = cvCreateMat(3, 4, projM->type);
	for(int i=0; i<3; ++i)
		for(int j=0 ; j<3; ++j)
			cvSetReal2D(temp, i, j, cvGetReal2D(R, i, j));
	for(int i=0; i<3; ++i)
		cvSetReal2D(temp, i, 3, cvGetReal2D(t, i, 0));

	cvMatMul(K, temp, projM);
	return true;
}

#define MAXSTRING 100
int main(int argc, char* args[])
{
	char fileName[MAXSTRING];
	int numCam = 2;

	//if(argc == 2)	//If only argument given is number of cameras
	{
		CvMat *projM = cvCreateMat(3, 4, CV_64FC1);
		for(int i = 0; i< numCam; ++i)
		{
			_snprintf_s(fileName, MAXSTRING, MAXSTRING, "Camera_%d_Intrinsic.txt", i);
			fstream intParam(fileName, std::ios::in);
			if(intParam.is_open() == false)
			{
				printf("\nCouldn't find file %s", fileName);
				printf("\nPlease visit http://fluorescore.cmb.usc.edu for more info\nPress Enter to exit...");
				getchar();
				return -1;
//				continue;
			}
			
			_snprintf_s(fileName, MAXSTRING, MAXSTRING, "Camera_%d_Extrinsic.txt", i);
			fstream extParam(fileName, std::ios::in);
			if(extParam.is_open() == false)
			{
				intParam.close();
				printf("\nCouldn't find file %s", fileName);
				printf("\nPlease visit http://fluorescore.cmb.usc.edu for more info\nPress Enter to exit...");
				getchar();
				return -1;
//				continue;
			}
			
			if(true == getProjectionMatrix(intParam, extParam, projM) )
			{
				ofstream out("projMat.txt");
				out<< "For Camera " << i <<":"<<endl;
				for(int j=0; j<projM->rows; ++j)
					for(int k=0; k<projM->cols; ++k)
						out<<""<<endl<<"PMatrix_"<<j<<k<<"="<<cvGetReal2D(projM, j, k);
						//printf("\n%d%d=%f", j, k, cvGetReal2D(projM, j, k));

				out<<""<<endl;
				out<<""<<endl;
			}

			intParam.close();
			extParam.close();
		}
	}
	printf("\n\nPress any key to exit");
	getchar();
}