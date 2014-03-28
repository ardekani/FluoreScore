#pragma once
#include "view.h"
#include "omp.h"
class CExperiment
{
public:
    std::vector<CView*> viewList;
    bool stopIt;
    std::string expSettings;

 	CExperiment(void);
	~CExperiment(void);   
	void addView(void);
    void close(void);
    bool readNewFrame(unsigned long frameIndex);
    void show(void);
    void extractCurrentFrameInfo(void);
    void writeCurrFrameInfo2CSV(void);
	bool calculateInitialBG(int);    
	bool openNewProject(void);
    void processNewFrame(void);
};


