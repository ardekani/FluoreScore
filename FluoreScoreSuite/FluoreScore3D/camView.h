#pragma once
#include "utility.h"
class CcamView
{
public:
	int index;
	CvMat *projMatrix; 
	CvMat *invProjMatrix; 
	Point3D opticalCenter;
	std::vector<lineEntry> entryVec;
	const char* fname;
	CcamView(void);
	~CcamView(void);
	Point3D getOpticalCenter( CvMat* pM );
	bool CcamView::initialize(std::string projMatFileName);
	bool CcamView::read2Dpoints(bool);
};
