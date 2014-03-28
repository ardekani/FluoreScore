#pragma once
#include "camview.h"

class CExperiment
{
public:

    CamView view0;
	CString status;
	CExperiment(void);
	~CExperiment(void);
	bool openNewProject(void);
	void close(void);
	void ProcessTheVideo(void);

};
