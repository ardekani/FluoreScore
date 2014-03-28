#pragma once
//#include <cv.h>
#include <fstream>
#include <cv.h>
#include <highgui.h>
#include <omp.h>
#include <iostream>
#include <deque>
#include <vector>
#include <math.h>
#include <iomanip>

class cSilhDetector
{
public:
    CvPoint pt1, pt2;
    IplImage *bgMean;
    IplImage *changeMask;
    IplImage *denoisedChangeMask;

    bool isInited;
    float alpha;
    char threshold;
    int frameCount;

    inline char pixelDistance(unsigned char *thisFrame, unsigned char *bg, int nChannels);
    void initialize(IplImage *firstFrame);

    cSilhDetector();

	void setThreshold(char thresh)
    { threshold = thresh; }

    void setAlpha(float a)
    { alpha = a; }

    void segmentFrame(IplImage *thisFrame);
    void finish();

};
