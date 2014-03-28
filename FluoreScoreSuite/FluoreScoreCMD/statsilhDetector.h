#pragma once
#include <cv.h>
#include <fstream>
#include "cvaux.h"


// Expose only segmentFrame. Everything else should be automatically done internally. Constructor. Destructor. isInited
/*
This class does the foreground segmentation. It keeps a background image (bgMean) which it keeps updating every frame using exponentially weighted average. A pixel
is classified as foreground if it becomes darker than the background. The amount of darkening is determined by threshhold (which can be set).

*/
class statSilhDetector
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

    void initialize(IplImage *firstFrame);

    statSilhDetector();


    void setThreshold(char thresh)
    { threshold = thresh; }

    void setAlpha(float a)
    { alpha = a; }

    //void segmentFrame(IplImage *thisFrame, std::fstream& OutputDataFile, CvPoint pt1, CvPoint pt2, const IplImage* const mask);
    void segmentFrame(IplImage *thisFrame);


	CvBGStatModel* bg_model;

    void finish();

};
