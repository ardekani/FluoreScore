#include "camView.h"

CcamView::CcamView(void)
{
}

CcamView::~CcamView(void)
{
}

/**
* Return optical center using Projection Matrix
*/

Point3D CcamView::getOpticalCenter( CvMat* pM )
{
	CvMat *A = cvCreateMat(3, 3, CV_64FC1);
	CvMat *Ainv = cvCreateMat(3, 3, CV_64FC1);
	CvMat *b = cvCreateMat(3, 1, CV_64FC1);
	for(int i=0; i<3; ++i)
	{
		for(int j=0; j<3; ++j)
			cvmSet(A, i, j, cvmGet(pM,i,j));
		cvmSet(b, i, 0, cvmGet(pM, i,3));
	}
	cvInvert(A, Ainv);
	CvMat *oc = cvCreateMat(3, 1, CV_64FC1);
	cvMatMul(Ainv, b, oc);
	Point3D toRet;
	toRet.x = -1 * cvmGet(oc, 0, 0);				
	toRet.y = -1 * cvmGet(oc, 1, 0);
	toRet.z = -1 * cvmGet(oc, 2, 0);

	cvReleaseMat(&A);
	cvReleaseMat(&Ainv);
	cvReleaseMat(&b);
	cvReleaseMat(&oc);
	return toRet;
}
/**
* Initialize the view
*/

bool CcamView::initialize(std::string projMatFileName)
{
	projMatrix = readProjMatrixFromFile(projMatFileName,index);
	if (projMatrix==NULL)
	{
			printf("\nCannot open Projection Matrix file @ %s\nAborting...",projMatFileName);
			getchar();
			return false;
	}

	opticalCenter = getOpticalCenter(projMatrix);
	invProjMatrix = cvCreateMat(4, 3, CV_64FC1);
	cvInvert(projMatrix,invProjMatrix, CV_SVD); // Compute and store the inverse in newly allocated matrix
	return true;

}
/**
* Load 2D points from input files 
*/

bool CcamView::read2Dpoints(bool skipHeader )
{
	char line[2000];
	lineEntry temp;
	std::ifstream myf;
	myf.open(fname);
	if(!myf.good())
	{
		printf("\n can't open %s", fname);
		getchar();
		return false;

	}
	if (skipHeader)
		myf.getline(line,2000); //skip the header line ...

	while (! myf.eof())
	{
		myf.getline(line,2000);
		if (strlen(line) != 0) // if line is not empty ...
		{
			temp = returnframeXY(line);
		};
		entryVec.push_back(temp);
	}

	return true;
}