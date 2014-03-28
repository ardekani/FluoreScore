
/**
FluoreScoreCMD (version 1.0.0)
A part of FluoreScoreSuite
Input: See printUsage function below
Ouput: See printUsage function below
for more details please visit http:\\fluorescore.cmb.usc.edu
Written by Reza Ardekani (dehestan@usc.edu)
April 2012
*/

#include "stdio.h"
#include "Experiment.h"
#include "string.h"
#include <iostream>
void printUsage()
{
	printf("\nSome input arguments are missing");
	printf("\nFluoreScore Usage:");
	printf("\n\nFSCommandLine x1 x2 x3 x4 x5 x6 x7 x8 x9 x10 x11 x12 x13 x14 x15 x16");
	printf("\n\nx1 : inputAviFile for View 1");
	printf("\n\nx2 : threshold value for View1, should be between 0 and 127");
	printf("\n\nx3 : inputAviFile for View 2");
	printf("\n\nx4 : threshold value for View2, should be between 0 and 127");
	printf("\n\nx5 : showing videos while processing( y or n). not showing videos can make the process faster");
	printf("\n\nx6 : Number of frames to process, put -1 to the whole video");
	printf("\n\nx7 : Mask file name for view 1 (jpg file)");
	printf("\n\nx8 : X-cordination of top-left for view 0 , min = 0, max = video width(640)");
	printf("\n\nx9 : Y-cordination of top-left for view 0 , min = 0, max = video height(480)");
	printf("\n\nx10 : X-cordination of down-right for view 0 , min = 0, max = video width (640)");
	printf("\n\nx11 : Y-cordination of down-right for view 0 , min = 0, max = video height (480)");
	printf("\n\nx12 : Mask file name for view 1 (jpg file)");
	printf("\n\nx13 : X-cordination of top-left for view 1 , min = 0, max = video width(640)");
	printf("\n\nx14 : Y-cordination of top-left for view 1 , min = 0, max = video height(480)");
	printf("\n\nx15 : X-cordination of down-right for view 1 , min = 0, max = video width(640)");
	printf("\n\nx16 : Y-cordination of down-right for view 1 , min = 0, max = video height(480)");
	printf("\n\nx1,x2,x3,x4,x5,x6 are mandatory parameters");
	printf("\n\nif x7-x16 have no input, videos will be processed without mask and ROI");
	printf("\n\nexample1:");
	printf("\n\nFluoreScoreCMD.exe Case_View0.avi 15 Case_View1.avi 15 y -1 mask1.jpg 200 250 400 450 mask2.jpg 100 150 550 500");
	printf("\n\nexample2:");
	printf("\n\nFluoreScoreCMD.exe Case_View0.avi 15 Case_View1.avi 15 y -1\n");
	printf("\nPlease visit http://fluorescore.cmb.usc.edu for further info");
	printf("\nPress Enter to continue");

};
#define __DEBUG 0

int main(int argc, char *argv[])
{
#if !__DEBUG
	if (argc<6)
	{
		printUsage();
		getchar();
		return 0;
	}
#endif


	CExperiment myExp;
	int nViews = 2; // for current Experiments we use two cameras, this number should be an input argument
	int nBins = 4; //hardcoded value for number of bins, this number should be an input argument
	int numOfFrameToModelBG = 100; //number of frames that are evenly sampled from the video to make the background model, this number should be an input argument

	for (int i = 0;i<nViews;i++)
	{
		myExp.addView(); // I just want to add one view for now ...
		myExp.viewList[i]->viewID = i;
		myExp.viewList[i]->numBins = nBins; 
	}
#if !__DEBUG
	//input file names ...
	std::string temp0;
	temp0 = argv[1];
	myExp.viewList[0]->inAVIFileName =temp0;
	temp0.erase(temp0.end()-4,temp0.end());
	temp0 = temp0 + ".csv";
	myExp.viewList[0]->outCSVFileName = temp0;

	temp0 = argv[3];
	myExp.viewList[1]->inAVIFileName =temp0;
	temp0.erase(temp0.end()-4,temp0.end());
	temp0 = temp0 + ".csv";
	myExp.viewList[1]->outCSVFileName = temp0;

	///getting threshold values
	int inputThresh = atoi(argv[2]);
	if (inputThresh > 127 || inputThresh<0)
	{
		printf("\ninvalid value for inputThresh");
		printUsage();
		getchar();
	}

	myExp.viewList[0]->viewThresh = inputThresh;


	inputThresh = atoi(argv[4]);
	if (inputThresh > 127 || inputThresh<0)
	{
		printf("\ninvalid value for inputThresh");
		printUsage();
		getchar();
	}
	// read 'show' option
	myExp.viewList[1]->viewThresh = inputThresh;

	bool showWindows;
	if(_strcmpi(argv[5],"y") == 0)
		showWindows = true;
	else
		showWindows = false;

	// read number of frames to process, if -1, read all frames
	int lastFramesToProcessIndex = 999999;

	if(atoi(argv[6]) == -1)
		lastFramesToProcessIndex = 9999999;
	else
		lastFramesToProcessIndex = atoi(argv[6]);
	/////////////////////////////////////////////////////

	if (argc == 7)
	{
		printf("\n **********Video will be processed without any mask ond ROI************* ");
		myExp.viewList[0]->isMaskSet = false;
		myExp.viewList[0]->isROISet = false;
		myExp.viewList[1]->isMaskSet = false;
		myExp.viewList[1]->isROISet = false;

	}
	else
	{
		if (argc ==17)
		{
			myExp.viewList[0]->isMaskSet = true;
			myExp.viewList[0]->isROISet = true;
			myExp.viewList[1]->isMaskSet = true;
			myExp.viewList[1]->isROISet = true;

			myExp.viewList[0]->myMask = cvLoadImage(argv[7],0);	 // read change mask
			myExp.viewList[1]->myMask = cvLoadImage(argv[12],0);	 

			//first view ROI
			myExp.viewList[0]->projROI.pt1.x = atoi(argv[8]);
			myExp.viewList[0]->projROI.pt1.y = atoi(argv[9]);
			myExp.viewList[0]->projROI.pt2.x = atoi(argv[10]);
			myExp.viewList[0]->projROI.pt2.y = atoi(argv[11]);

			//second view ROI
			myExp.viewList[1]->projROI.pt1.x = atoi(argv[13]);
			myExp.viewList[1]->projROI.pt1.y = atoi(argv[14]);
			myExp.viewList[1]->projROI.pt2.x = atoi(argv[15]);
			myExp.viewList[1]->projROI.pt2.y = atoi(argv[16]);
		}
		else
		{
			printf("\nnot complete input..");
			printUsage();
			getchar();
			return 0;
		}
	}

#else
	myExp.viewList[0]->viewThresh = 25;
	myExp.viewList[1]->viewThresh = 25;

	myExp.viewList[0]->isMaskSet = false;
	myExp.viewList[0]->isROISet = false;
	myExp.viewList[1]->isMaskSet = false;
	myExp.viewList[1]->isROISet = false;


	//char a[100] = "G:\\test\\eyeless_a64bit.csv";
	//char b[100] = "G:\\test\\eyeless_a.avi";
	//char c[100] = "G:\\test\\eyeless_b64bit.csv";
	//char d[100] = "G:\\test\\eyeless_b.avi";

	myExp.viewList[0]->inAVIFileName = "G:\\test\\eyeless_a.avi";
	myExp.viewList[1]->inAVIFileName = "G:\\test\\eyeless_b.avi";
	myExp.viewList[0]->outCSVFileName = "G:\\test\\eyeless_a64bit.csv";
	myExp.viewList[1]->outCSVFileName = "G:\\test\\eyeless_b64bit.csv";
	//strcpy(myExp.viewList[0]->inAVIFileName ,b);
	//strcpy(myExp.viewList[1]->inAVIFileName ,d);
	//strcpy(myExp.viewList[0]->outCSVFileName ,a);
	//strcpy(myExp.viewList[1]->outCSVFileName ,c);

	//myExp.viewList[0]->outCSVFileName = a.c_str();
	//myExp.viewList[0]->inAVIFileName= b ;

	//myExp.viewList[1]->outCSVFileName = c.c_str();
	//	myExp.viewList[1]->inAVIFileName= d.c_str();

	bool showWindows= false;

	int lastFramesToProcessIndex = 9999999;

#endif


	myExp.openNewProject();


	for (int ii = 0;ii<2;ii++)
	{
		//If there is no ROI defined, all pixels will be processed
		if (!myExp.viewList[ii]->isROISet)
		{
			myExp.viewList[ii]->projROI.pt1.x = 0;
			myExp.viewList[ii]->projROI.pt1.y = 0;
			myExp.viewList[ii]->projROI.pt2.x = myExp.viewList[ii]->frameWidth;
			myExp.viewList[ii]->projROI.pt2.y = myExp.viewList[ii]->frameHeight;
		}
		// if no input file is indicated, there will be no masking
		if (!myExp.viewList[ii]->isMaskSet)
		{
			myExp.viewList[ii]->myMask = cvCreateImage(cvSize(myExp.viewList[ii]->frameWidth,myExp.viewList[ii]->frameHeight),8,1);
			cvZero(myExp.viewList[ii]->myMask);
		}
	}

	printf("\ncalculating background");
	myExp.calculateInitialBG(numOfFrameToModelBG);

	cvSetCaptureProperty(myExp.viewList[0]->inVideoCapture,CV_CAP_PROP_POS_FRAMES,double(0.0)); // put it back
	cvSetCaptureProperty(myExp.viewList[1]->inVideoCapture,CV_CAP_PROP_POS_FRAMES,double(0.0)); // put it back

	printf("\n calculating Initial BG is done!\n");

	//	double t = (double)cvGetTickCount();
	int fn; 
	for(fn = 0 ;myExp.readNewFrame((unsigned long)fn)  && fn < lastFramesToProcessIndex ;fn++)
	{
		if((fn % 100) == 0 )
			printf("fn = %d\n",fn);

		for (int i = 0;i<nViews;i++) // tell silhdetector about ROI
		{
			myExp.viewList[i]->silhDetect.pt1 = cvPoint(0,0);
			myExp.viewList[i]->silhDetect.pt2 = cvPoint(myExp.viewList[i]->currFrame->width,myExp.viewList[i]->currFrame->height);
		}

		myExp.processNewFrame();
		myExp.extractCurrentFrameInfo();
		myExp.writeCurrFrameInfo2CSV();

		if (showWindows) //display output, pressing ESC will terminate the processing
		{
			for (int i = 0;i<nViews;i++)
			{
				myExp.viewList[i]->show();
			}
			char k = cvWaitKey(-1);
			if( k == 27 ) break;
		}

	}

	for(int i = 0;i<nViews;i++)
		myExp.viewList[i]->closeView();

	//	t = (double)cvGetTickCount() - t;
	printf( "\ndone! press Enter");
	exit(0);
	return 0;
}
