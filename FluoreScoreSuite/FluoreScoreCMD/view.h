#ifndef VIEW_H
#define VIEW_H
#include "SilhDetector.h"

struct twoPointsROI{

        CvPoint pt1;
        CvPoint pt2;
};
/**
* structure too keep intensity values as well as 2D positions for each frame
*/
struct FrameIntensityInfo
{
    std::vector<std::vector<int>> rChannel; // this keeps red channel info for all components detected in current frame ..
    std::vector<std::vector<int>> bChannel;// this keeps blue channel info for all components detected in current frame ..
    std::vector<std::vector<int>> gChannel;// this keeps green channel info for all components detected in current frame ..
    std::vector<std::vector<double>> pos_size; // keeps x,y, and total num of pixels for each detected component
};

class CView
{
public:
        IplImage* currFrame;
		IplImage* myMask;
        IplImage* viewMask;
		IplImage* viewProcessedArea;
		IplImage *calculatedInitialBG;
		FrameIntensityInfo currFrameInfo;
        int viewID;
        int frameWidth;
        int frameHeight;
        int numFrames;
        int frameCount;
        int numBins;
        bool isMaskSet, isROISet;
        bool toStop, askAgain;
        bool doDenoising;
        bool saveSeparateSilh;
        bool showInputVideo;
        bool showChangeMask;
        bool showDenoisedChangeMask;
        bool showProcessedArea;
        bool showMask;
		bool showBGModelingProcess;
        bool isOpen;
        bool saveCSVoutput; // its gonna save green channel as default
        bool saveBlue;
        bool saveRed;
        bool saveGreen;
		bool useStatSilh;
        twoPointsROI projROI; 
        //char inAVIFileName[10000];
        //char outCSVFileName[10000];
		std::string inAVIFileName;
		std::string outCSVFileName;
        std::string viewSettings;
        cSilhDetector silhDetect;
        double viewThresh;
        CvCapture* inVideoCapture;
        FILE *fp_outputCSV;

		CView(void);
        ~CView(void);
        void processNewFrame(void);
        bool readNewFrame(unsigned long frameIndex);
		static void onTrackbarSlide(int thresh);
        void closeView(void);
        bool openView(void);
        void writeCurrFrameInfo2CSV(void);
        void extractCurrFrameInfo(void);
        void show(void);
        bool readFrameResult;
        void writeHeaderinCSV(void);
        bool justForDefiningThreshold; // to check if we are in the state of defining thresholds
		void calculateInitialBG(int);
};




#endif // VIEW_H
