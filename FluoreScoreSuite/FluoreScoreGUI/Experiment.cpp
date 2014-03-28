#include "stdafx.h"
#include "Experiment.h"

CExperiment::CExperiment(void)
{
}

CExperiment::~CExperiment(void)
{
}

bool CExperiment::openNewProject(void)
{
	view0.viewID = 0;
	bool v1 = view0.openView();
    return (v1);

}

void CExperiment::close(void)
{
	view0.closeView();
	status = _T("Done!");
}

void CExperiment::ProcessTheVideo(void)
{
//	status = _T("Processing ...");
	view0.ProcessTheVideo();
}
