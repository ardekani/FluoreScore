#include "utility.h"

#include <math.h>

/**
* Reads projection matrices that are saved in a text file in a specific format(output of helpCalib.exe)
* This function is from flykit project (yet to publish)
*/

CvMat* readProjMatrixFromFile(std::string fileName,int camIndex)
{
	CvMat *projMat = cvCreateMat(3, 4, CV_64FC1);
	FILE *fp;
	
	if(projMat == NULL) // Failed to allocate memory : return NULL value to indicate failure
		return projMat;

	fopen_s(&fp,fileName.c_str(),"r");

	if(fp==NULL)  // Failed to open file : return NULL value to indicate failure
	{
		cvReleaseMat(&projMat);
		projMat = NULL;
		return projMat;
	}
	
	int equal_Signs_ToSkip = camIndex * 3 * 4; // A single projection matrix is defined completly in 3*4 = 12 "=" signs, skip those to read next
	int i=0;
	for(i=0;!feof(fp) && i<equal_Signs_ToSkip;)
		if(fgetc(fp)== int('='))
			i++;

	if(i!=equal_Signs_ToSkip) // File does not has the specified camera index : error
	{
		cvReleaseMat(&projMat);
		fclose(fp);
		projMat = NULL;
		return projMat;
	}
	
	// Get the projection Matrix values now
	for(int row=0;row<3;row++)
	{
		for(int col=0;col<4;col++)
		{
			// Keep skipping to next '=' sign and keep reading next float value
			int ch=!EOF; // Initialize with something !=EOF, so that it does loop condition does not fail on first instance

			while(ch!=EOF && ch!=int('='))
				ch = fgetc(fp);
			if(ch==EOF) // File does not has complete projection matrix : ERROR
			{
				cvReleaseMat(&projMat);
				fclose(fp);
				projMat = NULL;
				return projMat;
			}
			double temp;
			fscanf_s(fp,"%lf",&temp); // Read the next value after '=' sign
			cvmSet(projMat,row,col,temp);
		}
	}
	fclose(fp);
	return projMat;
}


/**
* Returns 'a' 3D point that has 2D projection point of v, in a specific view with P = (invPm)^-1
*/
Point3D backProject(Point2D v, CvMat *invPm)
{
	CvMat *temp = cvCreateMat(3, 1, CV_64FC1);
	cvmSet(temp, 0, 0, v.x);
	cvmSet(temp, 1, 0, v.y);
	cvmSet(temp, 2, 0, 1);
	CvMat *pt3D = cvCreateMat(4, 1, CV_64FC1);

	cvMatMul(invPm, temp, pt3D);
	Point3D toRet;
	toRet.x = cvmGet(pt3D, 0, 0) / cvmGet(pt3D, 3, 0);
	toRet.y = cvmGet(pt3D, 1, 0) / cvmGet(pt3D, 3, 0);
	toRet.z = cvmGet(pt3D, 2, 0) / cvmGet(pt3D, 3, 0);
	cvReleaseMat(&temp);
	cvReleaseMat(&pt3D);
	return toRet;
}
/** Returns the frameNumber, and X and Y from the current line
* current line has csv format, the first column is framenumber and last two columns are X and Y
*/
lineEntry returnframeXY (char curLine[2000])
{
	lineEntry temp;
	char *pch; 	double last, prev_last;
	pch = strtok(curLine,",");
	temp.frameNum = atoi(pch); //framenumber is the first column
	last = -1.0;
	prev_last = -1.0;

	while (1)
	{
		pch = strtok (NULL, ",");
		if (pch != NULL)
		{
			prev_last = last; // this is just to read the last two columns of CSV file which contain x, y 
			last = atof(pch);
		}else
			break;
	}
	temp.x = prev_last;
	temp.y = last;
	return temp;
}

Point3D meanOfPoints(Point3D a,Point3D b)
{
	Point3D mean;
	mean.x = (a.x+b.x)/2.0;
	mean.y = (a.y+b.y)/2.0;
	mean.z = (a.z+b.z)/2.0;
	return mean;
}

/**
* finds the intersection (shortest line) between two 3D lines
* from http://local.wasp.uwa.edu.au/~pbourke/geometry/lineline3d/lineline.c
*/
bool LineLineIntersect(Point3D p1,Point3D p2,Point3D p3,Point3D p4,Point3D *pa,Point3D *pb,double *mua, double *mub)
{
	Point3D p13,p43,p21;
	double d1343,d4321,d1321,d4343,d2121;
	double numer,denom;
	double ep = 0.0001;


	p13.x = p1.x - p3.x;
	p13.y = p1.y - p3.y;
	p13.z = p1.z - p3.z;
	p43.x = p4.x - p3.x;
	p43.y = p4.y - p3.y;
	p43.z = p4.z - p3.z;
	if ( fabs(p43.x)  < EPSILON && fabs(p43.y)  < EPSILON && fabs(p43.z)  < EPSILON)
	  return(false);


	p21.x = p2.x - p1.x;
	p21.y = p2.y - p1.y;
	p21.z = p2.z - p1.z;
	if (fabs(p21.x)  < EPSILON && fabs(p21.y)  < EPSILON && fabs(p21.z)  < EPSILON)
	  return(false);

	d1343 = p13.x * p43.x + p13.y * p43.y + p13.z * p43.z;
	d4321 = p43.x * p21.x + p43.y * p21.y + p43.z * p21.z;
	d1321 = p13.x * p21.x + p13.y * p21.y + p13.z * p21.z;
	d4343 = p43.x * p43.x + p43.y * p43.y + p43.z * p43.z;
	d2121 = p21.x * p21.x + p21.y * p21.y + p21.z * p21.z;

	denom = d2121 * d4343 - d4321 * d4321;
	if (fabs(denom) < EPSILON)
	  return(false);
	numer = d1343 * d4321 - d1321 * d4343;

	*mua = numer / denom;
	*mub = (d1343 + d4321 * (*mua)) / d4343;

	pa->x = p1.x + *mua * p21.x;
	pa->y = p1.y + *mua * p21.y;
	pa->z = p1.z + *mua * p21.z;
	pb->x = p3.x + *mub * p43.x;
	pb->y = p3.y + *mub * p43.y;
	pb->z = p3.z + *mub * p43.z;

	return(true);
}




