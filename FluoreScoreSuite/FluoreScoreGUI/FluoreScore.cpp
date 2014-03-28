/**
FluoreScore3D (version 1.0.0)
A part of FluoreScoreSuite
GUI version for defining Mask, threshold and ROI
for more details please visit http:\\fluorescore.cmb.usc.edu
Written by Reza Ardekani (dehestan@usc.edu)
April 2012
*/


// FluoreScore.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "FluoreScore.h"
#include "FluoreScoreDlg.h"
#include "SilhDetector.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFluoreScoreApp

BEGIN_MESSAGE_MAP(CFluoreScoreApp, CWinApp)
ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CFluoreScoreApp construction

CFluoreScoreApp::CFluoreScoreApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CFluoreScoreApp object

CFluoreScoreApp theApp;

// CFluoreScoreApp initialization

BOOL CFluoreScoreApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Changed Local AppWizard-Generated Applications"));

	CFluoreScoreDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();

//	dlg.Video_file = ""; // Reza: Set the input file name to empty
	if (nResponse == IDOK)
	{
//		dlg.close();

		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
