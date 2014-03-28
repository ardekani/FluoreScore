#pragma once

#include "View.h"
#include <omp.h>
#include "cv.h"
#include "highgui.h"
#include <iostream>

#define UPIXEL(img, i, j, k)	(*( (unsigned char*) ( (img)->imageData + (img)->widthStep * (j) + (img)->nChannels * (i) + (k)) ))
#define PIXEL(img, i, j, k)		(*( (char*)          ( (img)->imageData + (img)->widthStep * (j) + (img)->nChannels * (i) + (k)) ))
#undef min // to prevent the error that is caused in std::min and std::max
#undef max

#define Point2D CvPoint2D64f


double mypixelDistance(unsigned char *thisFrame, unsigned char *bg, int nChannels)
{
    //Can be faster if you don't use double calculations. use ints.
    if(nChannels == 3)
        return (double) ( 0.299 * (((int) thisFrame[2]) - bg[2]) + 0.587 * (((int) thisFrame[1]) - bg[1]) + 0.114 * ( ((int)thisFrame[0]) - bg[0]) );
    else
        return (double) (thisFrame[0] - bg[0]);
}


Point2D calculateCenterofContour (CvSeq *c) //returns center of a contour, should go to utility file ..
{
	Point2D center;
	double totalX=0.0,totalY=0.0;
	double total=0.0;
	for( int k=0; k<c->total;k++)
	{
		CvPoint* p = (CvPoint*)cvGetSeqElem(c,k);
		totalX += p->x;
		totalY += p->y;
		total++;
	}
	center.x = totalX/total;
	center.y = totalY/total;

	return center;
}


CView::CView(void)
{

	isOpen = false;
	toStop = false;
	askAgain = true;
	isMaskSet = false;
	doDenoising = true;
	saveSeparateSilh = true;
	showInputVideo = true;
	showChangeMask = true;
	showDenoisedChangeMask = true;
	showProcessedArea = true;
	showMask = true;
	saveGreen = true;
	saveBlue = true;
	saveRed = true;
	saveCSVoutput = saveBlue || saveGreen || saveGreen;
	frameCount = 0;
	viewThresh = 20; //default threshold;
	justForDefiningThreshold = false;
	numBins = 4;
	useStatSilh = false;
	showBGModelingProcess = true;
	viewProcessedArea = NULL;
	inVideoCapture = NULL;
    fp_outputCSV = NULL;

}

CView::~CView(void)
{
}

/**
* extract the change mask in new frame
*/
void CView::processNewFrame(void)
{
    if (silhDetect.isInited == false)
		silhDetect.initialize(calculatedInitialBG); // use calculated background instead of starting with firstframe as background

	silhDetect.setThreshold( viewThresh); // I use the Threshold Value that I get from Slider Control

	silhDetect.setAlpha(0.05); // a hard-coded value for adaptation-rate, works pretty good!

	silhDetect.pt1 = projROI.pt1;
	silhDetect.pt2 = projROI.pt2;
	silhDetect.segmentFrame(currFrame); // here I would have the output in vectors..
}


void CView::closeView()
{
	toStop = true;
	if (fp_outputCSV != NULL)
		fclose(fp_outputCSV);
	silhDetect.finish();
	cvDestroyAllWindows(); //close all windows
	cvReleaseCapture(&inVideoCapture);
	isOpen = false;

}
/**
* Read a frame from AVI file
* Partially from FlyKit project (yet to be published)
*/

bool CView::readNewFrame(unsigned long frameIndex)
{
	if(isOpen == false) // Cannot read any frame grab if view is not open
		return (readFrameResult = false);
	    if((long)frameIndex>=0L && (long)frameIndex<numFrames) // <==> if(Valid Frame number)
	        cvSetCaptureProperty(inVideoCapture,CV_CAP_PROP_POS_FRAMES,double(frameIndex));
	    else
	        return (readFrameResult = false); // Cannot read this frame .. illegal frame number

	currFrame = cvQueryFrame(inVideoCapture);

	if(currFrame==NULL) // Some error occured in grabbing frame from video file
		return (readFrameResult = false);
	return (readFrameResult = true); // Success
}
/**
* Open AVI file for this view
*/
bool CView::openView(void)
{
	inVideoCapture= cvCreateFileCapture(inAVIFileName.c_str()); // Create the file capture
	if(inVideoCapture == NULL) // Cannot open File
	{
		printf("\ncan not open input file %s",inAVIFileName);
		getchar();
		return false;
	}


	frameHeight   = (int) cvGetCaptureProperty(inVideoCapture, CV_CAP_PROP_FRAME_HEIGHT);
	frameWidth   = (int) cvGetCaptureProperty(inVideoCapture, CV_CAP_PROP_FRAME_WIDTH);
	numFrames = (int) cvGetCaptureProperty(inVideoCapture,  CV_CAP_PROP_FRAME_COUNT); // will use it later ...
	isOpen = true;
	return true;

}
/**
* writes the header for csv output file, it can be different with different number of bins/channels
*/
void CView::writeHeaderinCSV()
{
	if (outCSVFileName == "")
	{
		printf("\ncannot write to output file...");
		getchar();
		return;
	}

//	if (saveCSVoutput || true)  /////CHANGE THISS!! QUICK HACK TO TEST SAVE FUNCTION
	if (saveCSVoutput)
	{
		fp_outputCSV = fopen(outCSVFileName.c_str(),"w");
		fprintf(fp_outputCSV,"frameNum, Nc,");
		if (saveBlue)
			for (int nb = 1;nb<=numBins;nb++)
			{
				char temp[20];
				sprintf(temp,"b%dNum,b%dSum,",nb,nb);
				fprintf(fp_outputCSV,"%s",temp);
			}
			if (saveGreen)
				for (int nb = 1;nb<=numBins;nb++)
				{
					char temp[20];
					sprintf(temp,"g%dNum,g%dSum,",nb,nb);
					fprintf(fp_outputCSV,"%s",temp);
				}
				if (saveRed)
					for (int nb = 1;nb<=numBins;nb++)
					{
						char temp[20];
						sprintf(temp,"r%dNum,r%dSum,",nb,nb);
						fprintf(fp_outputCSV,"%s",temp);
					}
					fprintf(fp_outputCSV, "totalNumOfPixels,x,y");
	}
}
/**
* writes current frame's intensity/position info to CSV output file
*/
void CView::writeCurrFrameInfo2CSV(void)
{
	if (fp_outputCSV == NULL)
		writeHeaderinCSV();
	if (currFrameInfo.rChannel.size() == 0)  // if nothing has been detected .. rChannel could be anything here ..put zero in the output file
	{
		int nch = (int)(saveBlue + saveGreen + saveRed);
		fprintf(fp_outputCSV,"\n%d,",frameCount);
		for (int i = 0;i<1+nch*2*numBins+1+1+1;i++)
			fprintf(fp_outputCSV,"0,");
	}

	for (int j = 0;j<currFrameInfo.rChannel.size();j++) // a loop on number of detected components
	{
		fprintf(fp_outputCSV,"\n%d,",frameCount);
		fprintf(fp_outputCSV,"%d,",currFrameInfo.rChannel.size());// number of detected components
		if(saveBlue)
		{
			for (int k = 0;k<currFrameInfo.rChannel[j].size();k++)
				fprintf(fp_outputCSV,"%d," ,currFrameInfo.bChannel[j][k]);
		}
		if(saveGreen)
		{
			for (int k = 0;k<currFrameInfo.bChannel[j].size();k++)
				fprintf(fp_outputCSV,"%d," ,currFrameInfo.gChannel[j][k]);
		}
		if (saveRed)
		{
			for (int k = 0;k<currFrameInfo.gChannel[j].size();k++)
				fprintf(fp_outputCSV,"%d," ,currFrameInfo.rChannel[j][k]);
		}
		for (int k = 0;k<currFrameInfo.pos_size[j].size();k++)
			fprintf(fp_outputCSV,"%.2lf," ,currFrameInfo.pos_size[j][k]);
	}
	frameCount++;
}

/**
* process the change mask of current frame to extract info
*/
void CView::extractCurrFrameInfo(void)
{
	IplImage* processMask;
	processMask = cvCloneImage(myMask);
	IplImage* currFrameClone;
	currFrameClone = cvCloneImage(currFrame);
	////////////////////////////////FROM HERE I WANT TO SAVE RGB HISTOGRAMS FOR EACH INDIVIDUALS
	IplImage *processedArea, *notProcessMask;
	processedArea = cvCreateImage(cvSize(currFrame->width, currFrame->height),IPL_DEPTH_8U, 1);
	notProcessMask = cvCreateImage(cvSize(currFrame->width, currFrame->height),IPL_DEPTH_8U, 1);
	cvNot(processMask,notProcessMask);
	cvAnd(notProcessMask,silhDetect.denoisedChangeMask,processedArea);
	CvMemStorage* storage = cvCreateMemStorage();
	CvSeq* first_contour = NULL;

	if (viewProcessedArea != NULL)
		cvReleaseImage(&viewProcessedArea);
	viewProcessedArea = cvCloneImage(processedArea);

	int Nc = cvFindContours(processedArea,storage,&first_contour,sizeof(CvContour),CV_RETR_EXTERNAL);//	CV_RETR_LIST);

	for( CvSeq* c=first_contour; c!=NULL; c=c->h_next ) //{
		cvDrawContours( processedArea, c, cvScalar(255), cvScalar(255), -1, CV_FILLED, 8);

	CvBox2D box;
	CvPoint2D32f box_vtx[4];
	CvPoint pt, pt0;
	currFrameInfo.bChannel.clear();
	currFrameInfo.gChannel.clear();
	currFrameInfo.rChannel.clear();
	currFrameInfo.pos_size.clear();

	//////////////////before filling it up/////////////////////

	for( CvSeq* c=first_contour; c!=NULL; c=c->h_next )
	{
		Point2D pos;
		pos = calculateCenterofContour(c);


		CvPoint TopLeft, RightBut;
		box = cvMinAreaRect2( c, 0 );
		cvBoxPoints( box, box_vtx );

		pt0.x = cvRound(box_vtx[3].x);
		pt0.y = cvRound(box_vtx[3].y);
		TopLeft.x = pt0.x;	TopLeft.y = pt0.y;	RightBut.x = pt0.x;	RightBut.y = pt0.y;
		for( int i = 0; i < 4; i++ )
		{
			pt.x = cvRound(box_vtx[i].x);
			pt.y = cvRound(box_vtx[i].y);
			if (TopLeft.x > pt.x)
				TopLeft.x = pt.x;
			if (TopLeft.y > pt.y)
				TopLeft.y = pt.y;
			if (RightBut.x < pt.x)
				RightBut.x = pt.x;
			if (RightBut.y < pt.y)
				RightBut.y = pt.y;
			pt0 = pt;
			// make sure all boundries are valid, BOX sometimes gives out of range boundaries (why? God knows!)
			if (RightBut.y>currFrame->height)
				RightBut.y = currFrame->height;
			if (RightBut.y<0)
				RightBut.y = 0;


			if (RightBut.x>currFrame->width)
				RightBut.x = currFrame->width;
			if (RightBut.x<0)
				RightBut.x = 0;


			if (TopLeft.y>currFrame->height)
				TopLeft.y = currFrame->height;
			if (TopLeft.y<0)
				TopLeft.y = 0;


			if (TopLeft.x>currFrame->width)
				TopLeft.x = currFrame->width;
			if (TopLeft.x<0)
				TopLeft.x = 0;
		}

		int totalNumOfDetectedPixels = 0;
		int i, j;

		std::vector <std::vector <int>> bins;
		for(int i=0; i < 3; ++i) // since we have three channels, r, g, b

		{
			std::vector <int> subbin;
			subbin.resize( 2 * numBins, 0); // since we keep pair of (g1Num,g1Sum) and so on
			bins.push_back(subbin);
		}

		std::vector <double> threshholds; //these are thresholds for binning (here they are 64,128,192,256

		for(int i=0; i<numBins; ++i)
			threshholds.push_back( (i + 1) * 256/numBins );

		for(j= TopLeft.y +1 ; j<= RightBut.y-1; j++) // for each individual contour (detected silhouette)
		{
			for(i=TopLeft.x+1; i<= RightBut.x-1; i++)
			{

				if ((PIXEL(silhDetect.denoisedChangeMask,i,j,0) !=0) &&  (PIXEL(processMask,i,j,0) == 0))
				{
					totalNumOfDetectedPixels++;
					for(int k = 0; k < currFrameClone->nChannels; ++k) // k is for channels, it was currFrameClone->nChannels-1, but now in currFrameClone nchannel = 3 so
					{
						for(int l = 0; l < 2*numBins; l+=2)
						{
							if( (int) UPIXEL(currFrameClone, i, j, k) < threshholds[l/2])
							{
								bins[k][l]++;
								bins[k][l+1] += (int) UPIXEL(currFrameClone, i, j, k);
								break;
							}
						}
					}
				}

			}

		}

		if (frameCount != 0) // there is no info for first frame
		{
			currFrameInfo.bChannel.push_back(bins[0]);
			currFrameInfo.gChannel.push_back(bins[1]);
			currFrameInfo.rChannel.push_back(bins[2]);
			std::vector <double> temptotalxy;
			temptotalxy.push_back(totalNumOfDetectedPixels);
			temptotalxy.push_back(pos.x);
			temptotalxy.push_back(pos.y);
			currFrameInfo.pos_size.push_back(temptotalxy);

		}
	}

	// clean up
	cvReleaseImage(&currFrameClone);
	cvReleaseImage(&processedArea);
	cvReleaseImage(&notProcessMask);
	cvReleaseMemStorage(&storage);
	cvReleaseImage(&processMask);

}
/**
* this function displays different outputs based on their flags
*/
void CView::show(void)
{
	char mycaption1[100];
	char mycaption2[100];
	char mycaption3[100];
	char mycaption4[100];
	char mycaption5[100];

	sprintf(mycaption1,"ChangeMask View%d",viewID);
	sprintf(mycaption2,"Denoised changeMask View%d",viewID);
	sprintf(mycaption3,"Input video View%d",viewID);
	sprintf(mycaption4,"Mask View%d",viewID);
	sprintf(mycaption5, "processedArea %d",viewID);

	if (showChangeMask)
	{
		cvNamedWindow(mycaption1);
		cvShowImage(mycaption1,silhDetect.changeMask);
	}
	else
		cvDestroyWindow(mycaption1);

	if (showDenoisedChangeMask)
	{
		cvNamedWindow(mycaption2);
		if (silhDetect.denoisedChangeMask != NULL /*&& !useStatSilh*/)
			cvShowImage(mycaption2,silhDetect.denoisedChangeMask);

	}
	else
		cvDestroyWindow(mycaption2);


	if (showInputVideo)
	{
		cvNamedWindow(mycaption3);
		cvShowImage(mycaption3,currFrame);
	}
	else
		cvDestroyWindow(mycaption3);

	sprintf(mycaption4,"Mask View%d",viewID);
	if (showMask)
	{
		cvNamedWindow(mycaption4);
		cvShowImage(mycaption4,myMask);
		cvShowImage(mycaption5, viewProcessedArea);

	}
	else
	{
		cvDestroyWindow(mycaption4);
		cvDestroyWindow(mycaption5);

	}
	cvWaitKey(showMask||showInputVideo||showDenoisedChangeMask||showChangeMask);
}
/**
* Calculates initial background for current view using nFramesBG frames by weighted averaging
*/
void CView::calculateInitialBG(int nFramesBG)
{
	if(!isOpen)
		openView();

	IplImage *tmp_frame = cvQueryFrame(inVideoCapture); // Read the next frame ("temp" points to internal OpenCV memory and should not be freed : See OpenCV doc on cvQueryFrame() )
	IplImage *currbgMean = cvCloneImage(tmp_frame);
	cvZero (currbgMean);

	CvPoint pt1,pt2;
	pt1.x = 0;	pt1.y = 0;
	pt2.x = tmp_frame->width;
	pt2.y = tmp_frame->height;

	IplImage *bgcurFrame = cvCloneImage(tmp_frame);
	cvZero (bgcurFrame);
	IplImage  *prevFrame;
	prevFrame = cvCloneImage(bgcurFrame);
	double bgIniThreshold = 10;
	double alpha;
	unsigned long int maxFramesInFile = (unsigned long int)cvGetCaptureProperty(inVideoCapture,CV_CAP_PROP_FRAME_COUNT); // Set the maxFramesInFile

	int n = 0;
	char mycaption1[100];
	char mycaption2[100];

	sprintf(mycaption1,"curFrame View%d",viewID);
	sprintf(mycaption2,"currbgMean View%d",viewID);

	for (int myfc = 0;myfc<maxFramesInFile;myfc+=(int)(maxFramesInFile/nFramesBG))
	{
		n++;
		printf("\n%d/%d",n, nFramesBG);

		cvSetCaptureProperty(inVideoCapture,CV_CAP_PROP_POS_FRAMES,double(myfc));
		tmp_frame = cvQueryFrame(inVideoCapture); // Read the next frame ("temp" points to internal OpenCV memory and should not be freed : See OpenCV doc on cvQueryFrame() )

		if(tmp_frame==NULL) // Some error occured in grabbing frame from AVI
		{
			printf("\n an error occured in background modeling part");
			break;
		}
		cvReleaseImage(&bgcurFrame);
		bgcurFrame = cvCloneImage(tmp_frame);
		if (myfc ==0)
		{
			cvReleaseImage(&prevFrame);
			prevFrame = cvCloneImage(bgcurFrame);
			continue;
		}
		else
		{
			alpha = 1.0/(n+1);
			int i,j; // Defined above so as to unable parallelization (OpenMP)
			#pragma omp parallel shared(bgcurFrame,currbgMean, prevFrame) private(i,j)		//SHOULD BE SHARING BGMEAN TOO. BUT IS THROWING AN ERROR :'( WHY??? Dunnooo. Default access clause is sharing though.
			{
				#pragma omp for schedule(dynamic)
				for( j= std::min(pt1.y, pt2.y)+1; j<std::max(pt1.y, pt2.y)-1; ++j) //just look for silhouettes in ROI
				{
					for( i=std::min(pt1.x, pt2.x)+1; i<std::max(pt1.x, pt2.x)-1; ++i)

					{
						if( fabs((mypixelDistance( &UPIXEL(bgcurFrame, i, j, 0), &UPIXEL(prevFrame, i, j, 0), 3 ))) < bgIniThreshold )
						{
							for(int k = 0; k < currbgMean->nChannels; ++k)
							{
								
								UPIXEL(currbgMean, i, j, k) = (unsigned char) (alpha * UPIXEL(bgcurFrame, i, j, k) + (1-alpha) * UPIXEL(currbgMean, i, j, k));
							}
						}
					}
				}
			}
		}
		cvReleaseImage(&prevFrame);
		prevFrame = cvCloneImage(bgcurFrame);
		cvWaitKey(1);
	}

	calculatedInitialBG = cvCloneImage(currbgMean);

	cvDestroyWindow(mycaption1);
	cvDestroyWindow(mycaption2);
	cvReleaseImage(&bgcurFrame);
	cvReleaseImage(&prevFrame);
	cvReleaseImage(&currbgMean);
}
