/*
This class does the foreground segmentation. It keeps a background image (bgMean) which it keeps updating every frame using exponentially weighted average. A pixel
is classified as foreground if it becomes darker than the background. The amount of darkening is determined by threshhold (which can be set).

*/
#pragma once
#include <cv.h>
#include <highgui.h>
#include <omp.h>

#include <fstream>
#include <iostream>
#include <deque>
#include <vector>
#include <math.h>
#include <iomanip>

class cSilhDetector
{
public:
    CvPoint pt1, pt2; // ROI from pt1 (TOP LEFT) to pt2(DOWN RIGHT)
    IplImage *bgMean;
    IplImage *changeMask;
    IplImage *denoisedChangeMask;

    bool isInited;
    float alpha;
    double threshold;
    int frameCount;
    inline double pixelDistance(unsigned char *thisFrame, unsigned char *bg, int nChannels);
    void initialize(IplImage *firstFrame);
    cSilhDetector();
    void setThreshold(double thresh)
    { threshold = thresh; }

    void setAlpha(float a)
    { alpha = a; }

    void segmentFrame(IplImage *thisFrame);
    void finish();

};
