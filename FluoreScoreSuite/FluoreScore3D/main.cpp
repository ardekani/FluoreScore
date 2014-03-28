/**
FluoreScore3D (version 1.0.0)
A part of FluoreScoreSuite
Input: 2D measurements and projection matrices from two views (cameras)
Ouput: 3D position (whenever it is possible, i.e. 2D measurements are available in both views)
for more details please visit http:\\fluorescore.cmb.usc.edu
Written by Reza Ardekani (dehestan@usc.edu)
April 2012
*/


#include "camview.h"
#define __VERBOSE 0

/**
* Uses 2D measurements and projection matrices of two views and returns the corresponding 3D point
* TODO: move it to utility.cpp
*/
Point3D return3DpointFrom2Views(Point2D p2Dv1, Point2D p2Dv2, CcamView V1, CcamView V2)//CvMat* projMat1, CvMat* projMat2);
{
	Point3D toRet;
	///toRet.x = 0; toRet.y = 0;toRet.z = 0;
	Point3D p3Dv1,p3Dv2;
	p3Dv1 = backProject(p2Dv1, V1.invProjMatrix);
	p3Dv2 = backProject(p2Dv2, V2.invProjMatrix);
	double mua, mub;
	Point3D pa, pb;
	LineLineIntersect(p3Dv1,V1.opticalCenter,p3Dv2,V2.opticalCenter,&pa,&pb,&mua,&mub);
	toRet = meanOfPoints(pa,pb);
	return toRet;
}


int main(int argc, char *argv[])
{

	std::string cam0Filename,cam1Filename,projmatFilename,outputFilename;

	if (argc != 5)
	{
		std::cout<<"Some input arguements are missing!\n";
		std::cout<<"Please enter input arguments in this order:\nFluoreScore3D.exe <camera1> <camera2> <projmatrix> <outputfile>\n";
		std::cout<<"\nPlease visit http://fluorescore.cmb.usc for more info\nPress Enter to exit...";
		getchar();
		return -1;
	}
	cam0Filename = argv[1];
	cam1Filename = argv[2];
	projmatFilename = argv[3];
	outputFilename = argv[4];

	CcamView cam0, cam1;
	cam0.index = 0;cam1.index = 1;
	FILE *fp;
	//string a;

	cam0.fname = cam0Filename.c_str();
	cam1.fname = cam1Filename.c_str();
	fp = fopen(outputFilename.c_str(),"w");
	if (fp == NULL)
	{
		printf("\n can't open %s file for output", outputFilename.c_str());
		getchar();
		return 1;
	}

	fprintf(fp,"frameNumber, x , y , z");

	if(!cam0.initialize(projmatFilename))
	{
		printf("\n can't open %s file for proj", projmatFilename);
		getchar();
		return 1;

	}
	
	if(!cam1.initialize(projmatFilename))
	{
		printf("\n can't open %s file for proj", projmatFilename);
		getchar();
		return 1;
	}

	if(!cam0.read2Dpoints(true))
	{
		printf("\n can't open %s file for cam0", cam0Filename);
		getchar();
		return 1;
		
	}//skip header
	if(!cam1.read2Dpoints(true))
	{
		printf("\n can't open %s file for cam1", cam1Filename);
		getchar();
		return 1;		
	}//skip header


	size_t indx0,indx1, curFrame0, curFrame1;
	indx0 = indx1 = 0;
	
	int counter = 0;
	while (indx0 < cam0.entryVec.size() && indx1<cam1.entryVec.size())
	{
		curFrame0 = cam0.entryVec[indx0].frameNum;
		curFrame1 = cam1.entryVec[indx1].frameNum;

		if(curFrame0 > curFrame1)
		{
			indx1++;
		}else if (curFrame0 < curFrame1)
		{
			indx0++;
		}
		else
		{
			if(curFrame0 ==curFrame1) //im writing three 'if' just to make the code more clear, it can be writen in a way shorter way using 'continue' 
			{
				counter++;
				Point2D p0,p1;
				p0.x = cam0.entryVec[indx0].x;
				p0.y = cam0.entryVec[indx0].y;
				p1.x = cam1.entryVec[indx1].x;
				p1.y = cam1.entryVec[indx1].y;
				Point3D temp;
				temp = return3DpointFrom2Views(p0,p1,cam0,cam1);
#if __VERBOSE
				printf("\n------------");
				printf("\n 3D point is (%.2lf, %.2lf, %.2lf)",temp.x,temp.y,temp.z);
#endif
				fprintf(fp, "\n%d, %.2lf, %.2lf, %.2lf",curFrame0,temp.x, temp.y, temp.z);
			}
			indx0++;
			indx1++;
		}
	};
#if __VERBOSE
	printf("\nNumber of the generated 3D points is %d", counter);
#endif
	printf("\n Done! the result 3D points are saved in %s.",outputFilename.c_str());

	return 0;
}





