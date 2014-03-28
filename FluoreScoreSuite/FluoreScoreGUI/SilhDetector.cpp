#include "stdafx.h"
#include "SilhDetector.h"

using namespace std;

#define UPIXEL(img, i, j, k)	(*( (unsigned char*) ( (img)->imageData + (img)->widthStep * (j) + (img)->nChannels * (i) + (k)) ))
#define PIXEL(img, i, j, k)		(*( (char*)          ( (img)->imageData + (img)->widthStep * (j) + (img)->nChannels * (i) + (k)) ))

#define Point2D CvPoint2D64f 

cSilhDetector::cSilhDetector()
{
	isInited = false;
	bgMean = NULL;
	alpha = (float) (1/30.0);
	threshold = -10;
}

void cSilhDetector::initialize(IplImage *firstFrame)
{
	if( bgMean != NULL )
	{
		cvReleaseImage(&bgMean);
		bgMean = NULL;
	}
	bgMean = cvCloneImage(firstFrame);		//Take first frame as mean.
	changeMask = cvCreateImage( cvGetSize(firstFrame),IPL_DEPTH_8U, 1); 
	cvZero(changeMask);
	denoisedChangeMask = cvCloneImage( changeMask);
	isInited = true;
}

inline char cSilhDetector::pixelDistance(unsigned char *thisFrame, unsigned char *bg, int nChannels)
{
	//Can be faster if you don't use double calculations. use ints.
	if(nChannels == 3)
		return (char) ( 0.299 * (((int) thisFrame[2]) - bg[2]) + 0.587 * (((int) thisFrame[1]) - bg[1]) + 0.114 * ( ((int)thisFrame[0]) - bg[0]) );
	else
		return (char) (thisFrame[0] - bg[0]);
}


void cSilhDetector::segmentFrame(IplImage *thisFrame)
{
//	double t = (double)cvGetTickCount();
	IplImage* thisFrameClone;
	thisFrameClone = cvCloneImage(thisFrame);

	cvZero(denoisedChangeMask);
	cvZero(changeMask);

	IplImage* omp_changeMask = cvCloneImage(changeMask);
	cvZero(omp_changeMask);

	int i,j; // Defined above so as to unable parallelization (OpenMP)
#pragma omp parallel shared(thisFrame, omp_changeMask) private(i,j)		//SHOULD BE SHARING BGMEAN TOO. BUT IS THROWING AN ERROR :'( WHY??? Dunnooo. Default access clause is sharing though.
	{
#pragma omp for schedule(dynamic)
		for( j= min(pt1.y, pt2.y)+1; j<max(pt1.y, pt2.y)-1; ++j) //just look for silhouettes in ROI
		{
			for( i=min(pt1.x, pt2.x)+1; i<max(pt1.x, pt2.x)-1; ++i)

			{
				double res = pixelDistance( &UPIXEL(thisFrame, i, j, 0), &UPIXEL(bgMean, i, j, 0), 3 );
				if (res<threshold)
				{
					for(int k = 0; k < bgMean->nChannels; ++k)
					{
						UPIXEL(bgMean, i, j, k) = (unsigned char) (alpha * UPIXEL(thisFrame, i, j, k) + (1-alpha) * UPIXEL(bgMean, i, j, k));
					}
				}
				else
				{
					PIXEL(thisFrameClone, i, j, 0) |= 0xFF;
					PIXEL(thisFrameClone, i, j, 3) |= 0xFF;
					PIXEL(thisFrameClone, i, j, 1) &= 0000;
					PIXEL(omp_changeMask,i, j, 0) |= 0xFF;

				}
			}
		}
	}

	cvReleaseImage(&changeMask);
	changeMask = cvCloneImage(omp_changeMask);

	IplImage* myTemp= cvCloneImage(changeMask);

	CvMemStorage* storage = cvCreateMemStorage();
	CvSeq* first_contour = NULL;

	int Nc = cvFindContours(myTemp,storage,&first_contour,sizeof(CvContour),CV_RETR_EXTERNAL);//	CV_RETR_LIST);
	for( CvSeq* c=first_contour; c!=NULL; c=c->h_next ) //make the contourlist I just want to include contours with Area > MINAREAOFCONTOURS;
		if (fabs(cvContourArea(c)) >  10)
			cvDrawContours(denoisedChangeMask,c, cvScalar(255,255,255), cvScalar(255), -1, CV_FILLED, 8);

	cvReleaseMemStorage(&storage);
//	t = (double)cvGetTickCount() - t;
	cvReleaseImage(&myTemp);
	cvReleaseImage(&thisFrameClone);
	cvReleaseImage(&omp_changeMask);
}

void cSilhDetector::finish()
{
	if (bgMean != NULL)
		cvReleaseImage(&bgMean);
	bgMean = NULL;
	isInited = false;
}

