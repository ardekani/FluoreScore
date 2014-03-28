//#include "stdafx.h"
#include <cv.h>
#include <highgui.h>
#include <omp.h>
#include "statSilhDetector.h"
#include <fstream>
#include <iostream>
#include <deque>
#include <vector>
#include <math.h>
#include <iomanip>

using namespace std;


#define UPIXEL(img, i, j, k)	(*( (unsigned char*) ( (img)->imageData + (img)->widthStep * (j) + (img)->nChannels * (i) + (k)) ))
#define PIXEL(img, i, j, k)		(*( (char*)          ( (img)->imageData + (img)->widthStep * (j) + (img)->nChannels * (i) + (k)) ))



statSilhDetector::statSilhDetector()
{
    isInited = false;
    bgMean = NULL;
    alpha = (float) (1/30.0);
    //threshold = 0x7F;
    threshold = -10;


	denoisedChangeMask = NULL;
	changeMask = NULL;
}

void statSilhDetector::initialize(IplImage *firstFrame)
{
   // static int numtimes = 0;

	printf("\n stat init");
	   cvSaveImage("d:\\statframe.jpg",firstFrame);
	bg_model = cvCreateFGDStatModel( firstFrame );

   // if( bgMean != NULL )
   // {
   //     cvReleaseImage(&bgMean);
   //     bgMean = NULL;
   // }
   // bgMean = cvCloneImage(firstFrame);		//Take first frame as mean.

   changeMask = cvCreateImage( cvGetSize(firstFrame),IPL_DEPTH_8U, 1);
   cvZero(changeMask);
   denoisedChangeMask = cvCloneImage( changeMask);
    isInited = true;
   // //frameCount=0;
   //// numtimes++;
   // //printf("\nnumtimmmmmesssss = %d",numtimes);

   // //    char fn[100];
   // //    sprintf(fn,"d:\\QTFSdcm%d.jpg",fc);
   // cvSaveImage("d:\\qqqttt1stbgmean.jpg",bgMean);
 

}


void statSilhDetector::segmentFrame(IplImage *thisFrame)
{

printf("\nin segment");

    if(isInited == false)
        initialize(thisFrame);


   cvUpdateBGStatModel( thisFrame, bg_model );
/*   cvShowImage("newBG",bg_model->background);
   cvShowImage("newFG",bg_model->foreground);


	printf("\nafter Ini")*/;
    static int fc = 0;
    IplImage* thisFrameClone;
    thisFrameClone = cvCloneImage(thisFrame);
    printf("\n just before zeroing denoise and usual change mask");

    cvZero(denoisedChangeMask);
    cvZero(changeMask);

    printf("\nthreshold in silh = %d",threshold);

  threshold = char(-10);


    char fn [100];
    //                sprintf(fn,"d:\\QTFSdcm%d.jpg",fc);
    //                cvSaveImage(fn,denoisedChangeMask);
    //                       sprintf(fn,"d:\\ToTest\\QTdcm%d.jpg",fc);
    //                        cvSaveImage(fn,denoisedChangeMask);


    sprintf(fn,"d:\\ToTest\\QTChangeMask%d.jpg",fc);
    cvSaveImage(fn,changeMask);
    sprintf(fn,"d:\\ToTest\\QTDenoisedChangeMask%d.jpg",fc);
    cvSaveImage(fn,denoisedChangeMask);

  //  sprintf(fn,"d:\\ToTest\\QTbgMean%d.jpg",fc);
    //cvSaveImage(fn,bgMean);


    fc++;


}

void statSilhDetector::finish()
{
    if (bgMean != NULL)
        cvReleaseImage(&bgMean);
    bgMean = NULL;
    isInited = false;
}
