#pragma once
#include "stdafx.h"
#include "camview.h"

#include <iostream>

#define UPIXEL(img, i, j, k)	(*( (unsigned char*) ( (img)->imageData + (img)->widthStep * (j) + (img)->nChannels * (i) + (k)) ))
#define PIXEL(img, i, j, k)		(*( (char*)          ( (img)->imageData + (img)->widthStep * (j) + (img)->nChannels * (i) + (k)) ))


#define Point2D CvPoint2D64f
#define	__SAVEFORDEMO 0 // this is just for DEMO purposes, should be 0

#if __SAVEFORDEMO

int VideoLength = 1800; // save one minute
CvVideoWriter* myWriter = cvCreateVideoWriter("output.avi", CV_FOURCC('F','F','D','S'), 30.0, cvSize(800,600),1);
CvVideoWriter* myWriterOrig = cvCreateVideoWriter("original.avi", CV_FOURCC('F','F','D','S'), 30.0, cvSize(800,600),1);
//std::string myTextGFP = "(b)Processed";
//std::string myTextOrig = "(b)Original";
std::string myTextGFP = "";
std::string myTextOrig = "";


#endif

IplImage* CamView::myMask  = cvCreateImage(cvSize(800,600),8,1); 
twoPointsROI CamView::projROI;

//CamView::CamView(void):silhDetect(this)

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

CamView::CamView(void)
{
	toStop = false;
	askAgain = true;
	isMaskSet = false;
	isROISet = false;
	doDenoising = true;
	saveSeparateSilh = true;
	showInputVideo = true;
	showChangeMask = false;
	showDenoisedChangeMask = true;
	showProcessedArea = true;
	showMask = false;
	isOpen = false;
	isBGCalculated = false;
	saveGreen = true;
	saveBlue = true;
	saveRed = true;

	cvZero(myMask);
	inAVIFileName = "";
	outCSVFileName = "";
	saveCSVoutput = saveBlue || saveGreen || saveGreen;
	frameCount = 0;
	viewProcessedArea = NULL;
	inVideoCapture = NULL;
	fp_outputCSV = NULL;
}

CamView::~CamView(void)
{
}


void CamView::processNewFrame(void)
{

	if (silhDetect.isInited == false)
		silhDetect.initialize(calculatedInitialBG); // I want to make silh to use calculated background instead of using first frame as bg 

	silhDetect.setThreshold(projThresh); // I use the Threshold Value that I get from Slider Control
	silhDetect.setAlpha(0.05); // this rate was 1/3.0 I'm changin it to 0.05
	silhDetect.pt1 = projROI.pt1;
	silhDetect.pt2 = projROI.pt2;
	silhDetect.segmentFrame(currFrame); // here I would have the output in vectors..

}

bool CamView::ProcessTheVideo(void)
{
	int i = 0;
	if (initProcessing()==true)
	{
#if __SAVEFORDEMO
		cvSetCaptureProperty(inVideoCapture,CV_CAP_PROP_POS_FRAMES,double(0.0));
#endif

		while(readNewFrame() && !toStop)
		{
			printf("\n in proc i = %d", i++);
			processNewFrame();
			extractCurrFrameInfo(); // this could be called just in the save CSVoutput.
			show();
			if(saveCSVoutput)
				writeCurrFrameInfo2CSV();
#if __SAVEFORDEMO
			if (i == 1800)
				toStop = true;

#endif
		}

	}  
	// TODO: check the else part...
	//else {
	//	return false;
	//}

	return true;
}

void CamView::closeView()
{
	toStop = true;
	if (fp_outputCSV != NULL)
		fclose(fp_outputCSV);
	silhDetect.finish();
	cvDestroyAllWindows(); //close all windows
	if (inVideoCapture != NULL)
		cvReleaseCapture(&inVideoCapture);
	isOpen = false;

#if __SAVEFORDEMO
	cvReleaseVideoWriter(&myWriter);
	cvReleaseVideoWriter(&myWriterOrig);

#endif
}

/**
* for defining roi, this is mouse handler
*/
void CamView::roiMouseHandler(int events, int x, int y, int flags, void* param)
{
	IplImage* img = (IplImage*) param;
	static IplImage* roiTemp = cvCloneImage(img);
	static CvPoint startPt;
	static bool firstRightFlag;
	static bool upflag;
	switch(events)
	{
	case CV_EVENT_RBUTTONDOWN:
		firstRightFlag = true;
		startPt.x = x;
		startPt.y = y;
	case CV_EVENT_RBUTTONUP:
		upflag = !upflag;
		if (!upflag)
		{
			projROI.pt1 = startPt;
			projROI.pt2 = cvPoint(x,y);
		}
		if (firstRightFlag == true)
		{
			firstRightFlag = false;
			CamView::projROI.pt1 = startPt;
			CamView::projROI.pt2 = cvPoint(x,y);
			if( cvWaitKey(1)== 13 )
			{
				cvDestroyWindow("ROI");
				return;
			}
		}
	}
	if (flags & CV_EVENT_FLAG_RBUTTON)
	{
		if (!firstRightFlag)
		{
			switch(events){
	case CV_EVENT_MOUSEMOVE:
		roiTemp = cvCloneImage(img); //make a fresh copy of img for roiTemp, (delete the previous ROI)
		cvRectangle(roiTemp,startPt,cvPoint(x,y),CV_RGB(255,255,0) ,3,IPL_DEPTH_8U);
		cvNamedWindow("ROI");
		cvShowImage("ROI",roiTemp);
			}
		}
	}
	return;
}
/**
* Mouse Handler for defining Mask
*/

void CamView::maskMouseHandler(int events, int x, int y, int flags, void* param)
{
	static CvPoint startPt;
	static bool firsttime;

	IplImage* thickFrame = (IplImage*) param;
	if ((myMask != NULL) && (myMask->height!=thickFrame->height )) // just make sure they have the same resolution
	{
		cvReleaseImage(&myMask);
		myMask = cvCreateImage(cvSize((int)(thickFrame->width) , (int)(thickFrame->height)), 8, 1);
		cvZero(myMask);
	}

	switch(events){
				case CV_EVENT_MOUSEMOVE:
					if (flags & CV_EVENT_FLAG_LBUTTON)
					{
						if (!firsttime)
						{
							startPt.x = x;
							startPt.y = y;
						};
						firsttime = true;
						cvLine(thickFrame,startPt,cvPoint(x,y),cvScalar(255,0,0),2);
						cvLine(myMask,startPt,cvPoint(x,y),cvScalar(255),1);
						startPt.x = x;
						startPt.y = y;
						cvShowImage("DRAW MASK",thickFrame);
						cvNamedWindow("DefinedMask");
						cvShowImage("DefinedMask", myMask);

						startPt.x = x;
						startPt.y = y;
						break;
					}
					if( cvWaitKey(1)== 13 )
					{
						cvDestroyWindow("DRAW MASK");
						cvDestroyWindow("DefinedMask");
					}
					break;
				case CV_EVENT_LBUTTONUP:
					firsttime = false;
					break;
	}
	return;
}
/**
* set ROI for the view
*/
void CamView::setProjROI()
{
	isROISet = true;
	IplImage *thisFrame;//, *thisClone;//, *roiTemp;

	if (openView()) // if the video file is readable
		thisFrame = cvQueryFrame(inVideoCapture); // grab the 10th frame

	cvNamedWindow("ROI");
	cvShowImage("ROI", thisFrame);
	cvSetMouseCallback("ROI",&(CamView::roiMouseHandler),thisFrame);
	cvWaitKey();

	CString tempname; // write the result in a file
	int l = this->inAVIFileName.GetLength();
	tempname = (this->inAVIFileName);
	tempname.Delete(l-4,4);
	tempname = tempname + _T("_roiPoints.txt"); //painful operations for CString class!

	CT2A temp(tempname);  // uses LPCTSTR conversion operator for CString and CT2A constructor
	const char* pszA = temp; // uses LPSTR conversion operator for CT2A
	std::string strA (pszA); // ues std::string constructo
	FILE* fp_roi_points = fopen(strA.c_str(),"w");
	fprintf(fp_roi_points,"p1 = (%d, %d)\n", projROI.pt1.x, projROI.pt1.y);
	fprintf(fp_roi_points,"p2 = (%d, %d)\n", projROI.pt2.x, projROI.pt2.y);
	fclose(fp_roi_points);

}

/**
* set Mask for the view
*/
void CamView::setMask(void)
{
	isMaskSet = true;
	cvZero(myMask);
	IplImage *myFrame1, *tmpdilate;       
	if (openView()) // if the video file is readable
		myFrame1 = cvQueryFrame(inVideoCapture);

	tmpdilate = cvCreateImage(cvSize(myFrame1->width,myFrame1->height),8,1);
	cvNamedWindow("DRAW MASK");
	cvShowImage("DRAW MASK", myFrame1);
	cvSetMouseCallback("DRAW MASK",&(CamView::maskMouseHandler),myFrame1);
	cvWaitKey();
	cvDilate(myMask, tmpdilate, NULL,1);
	//cvErode(myMask, tmpdilate, NULL,1);
	//cvNamedWindow("DilatedMask");
	//cvShowImage("DilatedMask",tmpdilate);

	// finding contours in Dilated mask and filling them

	CvMemStorage* storage = cvCreateMemStorage();
	CvSeq* first_contour = NULL;

	int Nc = cvFindContours(tmpdilate,storage,&first_contour,sizeof(CvContour),CV_RETR_EXTERNAL);//	CV_RETR_LIST);

	for( CvSeq* c=first_contour; c!=NULL; c=c->h_next ) 
		cvDrawContours( myMask, c, cvScalar(255), cvScalar(255), -1, CV_FILLED, 8);
	cvNamedWindow("Final Mask");
	cvShowImage( "Final Mask", myMask );
	cvWaitKey(0);


	//save the mask in a jpg file
	CString tempname;
	int l = inAVIFileName.GetLength();
	tempname = (inAVIFileName);
	tempname.Delete(l-4,4); // remove the file extension
	tempname = tempname + _T("_mask.jpg"); //painful operations for CString class!

	CT2A temp(tempname);  // uses LPCTSTR conversion operator for CString and CT2A constructor
	const char* pszA = temp; // uses LPSTR conversion operator for CT2A
	std::string strA (pszA); // ues std::string constructo
	cvSaveImage(strA.c_str(),myMask);

	cvReleaseImage(&tmpdilate);
	cvReleaseMemStorage(&storage);
	cvDestroyAllWindows();
}

bool CamView::readNewFrame(void)
{
	currFrame = cvQueryFrame(inVideoCapture);
	if(currFrame==NULL) // Some error occured in grabbing frame from AVI
		return false;
	return true;

}

bool CamView::openView(void)
{
	if (inVideoCapture != NULL)
		inVideoCapture = NULL;

	std::string ss = std::string(CT2CA(inAVIFileName)); // Convert Video_File from CString to const Char
	inVideoCapture= cvCreateFileCapture(ss.c_str()); // Create the file capture

	if(inVideoCapture == NULL) // Cannot open File
	{
		printf("\ncan not open input file");
		getchar();
		return false;
	}

	frameHeight   = (int) cvGetCaptureProperty(inVideoCapture, CV_CAP_PROP_FRAME_HEIGHT);
	frameWidth   = (int) cvGetCaptureProperty(inVideoCapture, CV_CAP_PROP_FRAME_WIDTH);
	numFrames = (int) cvGetCaptureProperty(inVideoCapture,  CV_CAP_PROP_FRAME_COUNT); // will use it later ...
	isOpen = true;
	return true;

}

bool CamView::initProcessing (void)
{
	if (!isROISet)
	{
		projROI.pt1 = cvPoint(0,0); // initialize the ROI , I think it should go to contructor of the project 
		projROI.pt2 = cvPoint(frameWidth,frameHeight);
	}

	silhDetect.pt1 = projROI.pt1;
	silhDetect.pt2 = projROI.pt2;
	numBins = 4;
	writeHeaderinCSV();
	if (!isMaskSet)
	{
		if ((myMask != NULL) && (myMask->height!=frameHeight )) // just make sure they have the same resolution
		{
			cvReleaseImage(&myMask);
			myMask = cvCreateImage(cvSize((int)(frameWidth) , (int)(frameHeight)), 8, 1);
			cvZero(myMask);
		}

	}

	return true;
}


void CamView::writeHeaderinCSV()
{
	if (outCSVFileName == "")
	{
		printf("\ncannot write to output file...");
		getchar();
		return;
	}

	if (saveCSVoutput)  
	{
		std::string ss = std::string(CT2CA(outCSVFileName)); // Convert Video_File from CString to const Char
		fp_outputCSV = fopen(ss.c_str(),"w");
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

void CamView::calculateInitialBG(int nFramesBG)
{
	if(!isOpen)
		openView();

	IplImage *tmp_frame = cvQueryFrame(inVideoCapture); // Read the next frame 
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




		cvShowImage(mycaption1,bgcurFrame);
		cvShowImage(mycaption2,currbgMean);
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


void CamView::show(void)
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
	}
	else
		cvDestroyWindow(mycaption4);
	if (showProcessedArea)
	{
		cvShowImage(mycaption5, viewProcessedArea);

	}
	else
		cvDestroyWindow(mycaption5);

	cvWaitKey(showMask||showInputVideo||showDenoisedChangeMask||showChangeMask);


#if __SAVEFORDEMO

	IplImage *bCh,*gCh,*rCh,*res,*andWithProcessedArea;
	bCh = cvCreateImage(cvSize(currFrame->width,currFrame->height),8,1);
	cvZero(bCh);
	gCh = cvCloneImage(bCh);
	rCh = cvCloneImage(bCh);
	andWithProcessedArea = cvCloneImage(bCh);

	res = cvCloneImage(currFrame);
	cvZero(res);

	cvSplit(currFrame,bCh,gCh,rCh,NULL);
	cvZero(bCh);cvZero(rCh);
	cvZero(gCh);
	IplImage *AllOne;
	AllOne = cvCloneImage(bCh); //Zero Matrix
	cvAddS(gCh,cvScalar(255),AllOne);
	//cvAnd(gCh,viewProcessedArea,andWithProcessedArea);

	cvAnd(AllOne,viewProcessedArea,andWithProcessedArea);

	cvMerge(bCh,andWithProcessedArea,rCh,NULL,res);


	CvFont myFont;
	CvPoint pos = cvPoint(5,20);

	cvInitFont(&myFont, CV_FONT_HERSHEY_SIMPLEX, 1,1, 0,2);

	cvPutText(res,myTextGFP.c_str(),pos,&myFont,cvScalar(255,255,255));
	IplImage *justCurrFrame = cvCloneImage(currFrame);
	cvPutText(justCurrFrame,myTextOrig.c_str(),pos,&myFont,cvScalar(255,255,255));

	cvWriteFrame(myWriter,res);
	cvWriteFrame(myWriterOrig,justCurrFrame);
	cvShowImage("res", res);
	cvShowImage("justCurrFrame",justCurrFrame);
	cvReleaseImage(&res);
	cvReleaseImage(&justCurrFrame);
	cvReleaseImage(&rCh);
	cvReleaseImage(&bCh);
	cvReleaseImage(&gCh);
	cvReleaseImage(&andWithProcessedArea);
	cvReleaseImage(&AllOne);
#endif 
}

void CamView::writeCurrFrameInfo2CSV(void)
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


void CamView::extractCurrFrameInfo(void)
{

	IplImage* processMask;
	processMask = cvCloneImage(myMask);
	IplImage* currFrameClone;
	currFrameClone = cvCloneImage(currFrame);
	// save RGB histograms for each individuals

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

	for( CvSeq* c=first_contour; c!=NULL; c=c->h_next ) 
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

			//make sure all border points are valid 

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
		for(int i=0; i < 3; ++i)
		{
			std::vector <int> subbin;
			subbin.resize( 2 * numBins, 0);
			bins.push_back(subbin);
		}
		std::vector <double> threshholds;
		for(int i=0; i<numBins; ++i)
			threshholds.push_back( (i + 1) * 256/numBins );

		for(j= TopLeft.y +1 ; j<= RightBut.y-1; j++)
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

		if (frameCount != 0)
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


	cvReleaseImage(&currFrameClone);
	cvReleaseImage(&processedArea);
	cvReleaseImage(&notProcessMask);
	cvReleaseMemStorage(&storage);
	cvReleaseImage(&processMask);

}