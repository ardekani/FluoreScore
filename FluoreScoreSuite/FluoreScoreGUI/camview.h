#ifndef VIEW_H
#define VIEW_H

#pragma once
#include <cv.h>
#include "SilhDetector.h"
#include <highgui.h>
#include <string>



struct twoPointsROI{

        CvPoint pt1;
        CvPoint pt2;
};


struct FrameIntensityInfo{
    std::vector<std::vector<int>> rChannel; // this keeps red channel info for all components detected in current frame ..
    std::vector<std::vector<int>> bChannel;// this keeps blue channel info for all components detected in current frame ..
    std::vector<std::vector<int>> gChannel;// this keeps green channel info for all components detected in current frame ..
	std::vector<std::vector<double>> pos_size; // keeps x,y, and total num of pixels for each detected component
};

class CamView
{
public:
        IplImage* currFrame;
		IplImage *calculatedInitialBG;
		IplImage* viewProcessedArea;
        FrameIntensityInfo currFrameInfo;
        static IplImage* myMask; // this is for processing mask
		bool isMaskSet, isROISet;
        bool toStop, askAgain;
        bool doDenoising;
        bool saveSeparateSilh;
        bool showInputVideo;
        bool showChangeMask;
        bool showDenoisedChangeMask;
        bool showProcessedArea;
        bool showMask;
        bool saveCSVoutput; // its gonna save green channel as default
        bool saveBlue;
        bool saveRed;
        bool saveGreen;				
		bool isOpen;
        int frameCount;
        int numBins;
		int frameWidth;
        int frameHeight;
        int numFrames;
		int viewID;
        int projThresh;
        static	twoPointsROI projROI;
		CString inAVIFileName;
        CString outCSVFileName;
        cSilhDetector silhDetect;
        CvCapture* inVideoCapture;
        FILE *fp_outputCSV;
        CamView(void);
        ~CamView(void);
        void processNewFrame(void);
        bool readNewFrame(void);
        void closeView(void);
        void setProjROI(void);
        void setMask(void);
        bool ProcessTheVideo(void);
        bool openView(void);
        bool initProcessing(void);
        void writeCurrFrameInfo2CSV(void);
        void extractCurrFrameInfo(void);
		void calculateInitialBG(int);
		bool isBGCalculated;
		void show(void);
		void writeHeaderinCSV(void);
        static void roiMouseHandler(int events, int x, int y, int flags, void* param);
        static void maskMouseHandler(int events, int x, int y, int flags, void* param);

};

#endif // VIEW_H
