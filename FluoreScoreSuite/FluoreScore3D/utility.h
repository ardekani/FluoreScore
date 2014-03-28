#ifndef UTILITY_H
#define UTILITY_H

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "cv.h"
#include "cxcore.h"
#include "highgui.h"

class CcamView;
struct lineEntry
{
	int frameNum;
	double x;
	double y;
};

#define EPSILON 0.00001 /// used in LineLineIntersect function
#define Point3D CvPoint3D64f /// Redefinition of CvPoint3D64f to a more human readable name
#define Point2D CvPoint2D64f /// Redefinition of CvPoint2D64f to a more human readable name

CvMat* readProjMatrixFromFile(std::string fileName,int camIndex);
Point3D backProject(Point2D v, CvMat *invPm);
lineEntry returnframeXY (char curLine[2000]);
bool LineLineIntersect(Point3D p1,Point3D p2,Point3D p3,Point3D p4,Point3D *pa,Point3D *pb,double *mua, double *mub);
Point3D meanOfPoints(Point3D a,Point3D b);

#endif