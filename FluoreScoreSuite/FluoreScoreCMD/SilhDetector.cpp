#include "SilhDetector.h"

using namespace std;


#define UPIXEL(img, i, j, k)	(*( (unsigned char*) ( (img)->imageData + (img)->widthStep * (j) + (img)->nChannels * (i) + (k)) ))
#define PIXEL(img, i, j, k)		(*( (char*)          ( (img)->imageData + (img)->widthStep * (j) + (img)->nChannels * (i) + (k)) ))
#define MINAREAOFCONTOURS 10
cSilhDetector::cSilhDetector()
{
	isInited = false;
	bgMean = NULL;
	alpha = (float) (1/30.0);
	denoisedChangeMask = NULL;
	changeMask = NULL;
	threshold = 20.0;
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

inline double cSilhDetector::pixelDistance(unsigned char *thisFrame, unsigned char *bg, int nChannels)
{
	//Can be faster if you don't use double calculations. use ints.
	if(nChannels == 3)
		return (double) ( 0.299 * (((int) thisFrame[2]) - bg[2]) + 0.587 * (((int) thisFrame[1]) - bg[1]) + 0.114 * ( ((int)thisFrame[0]) - bg[0]) );
	else
		return (double) (thisFrame[0] - bg[0]);
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
#pragma omp parallel shared(thisFrame, omp_changeMask) private(i,j)		
	{
#pragma omp for schedule(dynamic)
		for( j= min(pt1.y, pt2.y)+1; j<max(pt1.y, pt2.y)-1; ++j) //just look for foreground in ROI
		{
			for( i=min(pt1.x, pt2.x)+1; i<max(pt1.x, pt2.x)-1; ++i)

			{
				double res = pixelDistance( &UPIXEL(thisFrame, i, j, 0), &UPIXEL(bgMean, i, j, 0), 3 );
				if (res<threshold) //if distance of current pixel is less than thrsehold just use it to update the model background, else treat it as forground
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

	int Nc = cvFindContours(myTemp,storage,&first_contour,sizeof(CvContour),CV_RETR_EXTERNAL); // find contours in change mask

	for( CvSeq* c=first_contour; c!=NULL; c=c->h_next ) //Only keep contours with Area > MINAREAOFCONTOURS, smaller contours are assumed to be stray pixels/noises
		if (fabs(cvContourArea(c)) >  MINAREAOFCONTOURS)
			cvDrawContours(denoisedChangeMask,c, cvScalar(255,255,255), cvScalar(255), -1, CV_FILLED, 8);

// clean up
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
