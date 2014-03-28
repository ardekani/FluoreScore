#include "Experiment.h"

CExperiment::CExperiment(void)
{
    stopIt = false;
}

CExperiment::~CExperiment(void)
{
}
/**
* Opens all views in experiment
*/

bool CExperiment::openNewProject(void)
{

    bool toret = true;
    for(int i = 0;i<viewList.size();i++)
        toret = toret && viewList[i]->openView();

    return toret;

}
/**
* Calles Process function in all views in experiment
*/

void CExperiment::processNewFrame(void)
{
    int i;
    omp_set_num_threads(int(viewList.size()));
#pragma omp parallel shared(i)
    {
#pragma omp for schedule(dynamic)
        for(i=0;i<viewList.size();i++)
        {
            viewList[i]->processNewFrame();
        }
    }

}

/**
* Calles calculate initial background function in all views in experiment
*/

bool CExperiment::calculateInitialBG(int nFrameBG)
{
    int i; int temp_omp = nFrameBG;
    for(i=0;i<viewList.size();i++)
		viewList[i]->calculateInitialBG(temp_omp);
	return true;
}

/**
* Calles extract current frame info function in all views in experiment
*/
void CExperiment::extractCurrentFrameInfo(void)
{
    int i;
    omp_set_num_threads(int(viewList.size()));
#pragma omp parallel shared(i)
    {
#pragma omp for schedule(dynamic)
        for(i=0;i<viewList.size();i++)
        {
            viewList[i]->extractCurrFrameInfo();
        }
    }
}

/**
* Calles extract current frame info function in all views in experiment
*/

void CExperiment::writeCurrFrameInfo2CSV(void)
{
    int i;
    omp_set_num_threads(int(viewList.size()));
#pragma omp parallel shared(i)
    {
#pragma omp for schedule(dynamic)
        for(i=0;i<viewList.size();i++)
        {
            viewList[i]->writeCurrFrameInfo2CSV();
        }
    }

}

/**
* Calles readNewFrame function in all views in experiment
*/

bool CExperiment::readNewFrame(unsigned long frameIndex)
{
    if(viewList.size()==0) //Nothing to do
        return false;
    int countView = viewList.size();

    omp_set_num_threads(countView); // Number of threads to be used by OpenMP
#pragma omp parallel shared(countView) // Beware of RACE conditions - if not taken care can return "false" at strange moments (different each time u run!_
    {
#pragma omp for schedule(dynamic)
        //Parallelize here
        for(countView=0;countView<viewList.size();countView++)
            viewList[countView]->readNewFrame(frameIndex);
    }

    // Check weather any of the calls failed (currFrame of that view would be NULL) - Could not be checked while grabbing itself, because of OpenMP structured block (no jump allowed)
    for(int i=0;i<viewList.size();i++)
    {
        if(viewList[i]->readFrameResult == false)
            return false;
        viewList[i]->readFrameResult = false;
    }
    return true;
}
/**
* Adds new view to experiment
*/

void CExperiment::addView(void)
{
    CView *temp = new CView();
    viewList.push_back(temp);

}
/**
* Closes all views of experiment
*/

void CExperiment::close(void)
{
    for(int i=0;i<viewList.size();i++)
    {
        viewList[i]->closeView();
        if(viewList[i]!=NULL)
            delete viewList[i];
    }
    viewList.clear();
}

/**
* Calls show function in all views in experiment
*/

void CExperiment::show(void)
{
    for (int i = 0;i<viewList.size();i++)
        viewList[i]->show();
    cvWaitKey(1);
}
