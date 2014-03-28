// FluoreScoreDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FluoreScore.h"
#include "FluoreScoreDlg.h"
#include "SilhDetector.h"
#include "experiment.h"

#define PIXEL(img, i, j, k)		*(img->imageData + img->widthStep * i + img->nChannels * j + k)
#define UPIXEL(img, i, j, k)	*( (unsigned char*) (img->imageData + img->widthStep * i + img->nChannels * j + k) )

#define DISPLAYSIZE 1600, 1200
#define RGBSLIDERMIN 0
#define RGBSLIDERMAX  127
#define DEFAULTTHRESH 15

#define __CONSULEOUTPUT 0 // for getting consule output (for debuggin purposes set this to 1)

#ifdef _DEBUG
#define new DEBUG_NEW
#endif;

void mouseHandler(int events, int x, int y, int flags, void* param)
{
	static int count = 0;
	switch(events){
	  case CV_EVENT_LBUTTONDOWN:
		  if(flags & CV_EVENT_FLAG_CTRLKEY) 
		  {
			  //			ROICorners.push_back(cvPoint(x,y));
			  break;
		  }
	  case CV_EVENT_LBUTTONUP:
		  //       printf("Left button up\n");
		  break;
	}
	return;
}


// CAboutDlg dialog used for App About


class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
	//	afx_msg void OnAnalyzeRegionofinterest();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//	ON_COMMAND(ID_ANALYZE_REGIONOFINTEREST, &CAboutDlg::OnAnalyzeRegionofinterest)
END_MESSAGE_MAP()


// CFluoreScoreDlg dialog




CFluoreScoreDlg::CFluoreScoreDlg(CWnd* pParent /*=NULL*/)
: CDialog(CFluoreScoreDlg::IDD, pParent)

//	, Video_file(_T(""))
, m_Thresh(0)
, m_Red(0)
, m_Green(0)
, m_Blue(0)

//	, Output_FileName(_T(""))

//, m_status(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFluoreScoreDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_THRESH, m_Thresh);

	DDX_Control(pDX, IDC_THRESH_SLIDER, m_Thresh_Slider);


	//DDX_Text(pDX, IDC_Opened_File, TheProject.ProjInputVideo);
	//DDX_Text(pDX, IDC_Opened_File2, TheProject.ProjOutputFile);

	DDX_Text(pDX, IDC_Opened_File, myExp.view0.inAVIFileName);
	DDX_Text(pDX, IDC_Opened_File2, myExp.view0.outCSVFileName);



	DDX_Text(pDX, IDC_STATUS, myExp.status);

	DDX_Control(pDX, IDOK, StopResumeButt);
	DDX_Control(pDX, IDC_DENOISING_CHECK, m_doDenoisingchk);
	DDX_Control(pDX, IDC_SAVESEPARATE4SILH_CHECK, m_saveSeparateforSilh);
	DDX_Control(pDX, IDC_SHOW_INPUTVIDEO_CHECK, m_showinputvideo_chck);
	DDX_Control(pDX, IDC_SHOW_CHANGEMASK_CHCK, m_showchangemask_chck);
	DDX_Control(pDX, IDC_SHOW_DENOISEDCHANGEMASK_CHCK, m_showdenoisedchangemask);
	DDX_Control(pDX, IDC_SHOW_PROCESSEDAREA_CHCK, m_showprocessedarea_chck);

}

BEGIN_MESSAGE_MAP(CFluoreScoreDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_FILE_OPEN, &CFluoreScoreDlg::OnFileOpen)
	ON_COMMAND(ID_ANALYZE_ANALYZE, &CFluoreScoreDlg::OnAnalyzeAnalyze)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_THRESH_SLIDER, &CFluoreScoreDlg::OnNMReleasedcaptureThreshSlider)
	//ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_RED_SLIDER, &CFluoreScoreDlg::OnNMReleasedcaptureRedSlider)
	//ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_BLUE_SLIDER, &CFluoreScoreDlg::OnNMReleasedcaptureBlueSlider)
	//ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_GREEN_SLIDER, &CFluoreScoreDlg::OnNMReleasedcaptureGreenSlider)
	ON_BN_CLICKED(IDOK, &CFluoreScoreDlg::OnBnClickedOk)
	ON_WM_CLOSE()
	ON_STN_CLICKED(IDC_FlyPic, &CFluoreScoreDlg::OnStnClickedFlypic)
	ON_COMMAND(ID_ANALYZE_REGIONOFINTEREST, &CFluoreScoreDlg::OnAnalyzeRegionofinterest)
	//	ON_COMMAND(ID_RECORD_VIDEO, &CFluoreScoreDlg::OnRecordVideo)
	ON_COMMAND(ID_ANALYZE_MASKING, &CFluoreScoreDlg::OnAnalyzeMasking)
	ON_BN_CLICKED(IDC_DENOISING_CHECK, &CFluoreScoreDlg::OnBnClickedDenoisingCheck)
	ON_BN_CLICKED(IDC_SAVESEPARATE4SILH_CHECK, &CFluoreScoreDlg::OnBnClickedSaveseparate4silhCheck)
	ON_BN_CLICKED(IDC_SHOW_INPUTVIDEO_CHECK, &CFluoreScoreDlg::OnBnClickedShowInputvideoCheck)
	ON_BN_CLICKED(IDC_SHOW_CHANGEMASK_CHCK, &CFluoreScoreDlg::OnBnClickedShowChangemaskChck)
	ON_BN_CLICKED(IDC_SHOW_DENOISEDCHANGEMASK_CHCK, &CFluoreScoreDlg::OnBnClickedShowDenoisedchangemaskChck)
	ON_BN_CLICKED(IDC_SHOW_PROCESSEDAREA_CHCK, &CFluoreScoreDlg::OnBnClickedShowProcessedareaChck)
	ON_COMMAND(ID_FILE_EXIT, &CFluoreScoreDlg::OnFileExit)
END_MESSAGE_MAP()


// CFluoreScoreDlg message handlers

BOOL CFluoreScoreDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// Reza initialization for Sliding Controls

	m_Thresh_Slider.SetRange(RGBSLIDERMIN,RGBSLIDERMAX,true);
	m_Thresh_Slider.SetPos(DEFAULTTHRESH);
	m_Thresh = DEFAULTTHRESH;


	//m_doDenoisingchk.SetCheck(TheProject.doDenoising);
	//m_saveSeparateforSilh.SetCheck(TheProject.saveSeparateSilh);
	//m_showinputvideo_chck.SetCheck(TheProject.showInputVideo);
	//m_showdenoisedchangemask.SetCheck(TheProject.showDenoisedChangeMask);
	//m_showchangemask_chck.SetCheck(TheProject.showChangeMask);
	//m_showprocessedarea_chck.SetCheck(TheProject.showProcessedArea);

	m_doDenoisingchk.SetCheck(myExp.view0.doDenoising);
	m_saveSeparateforSilh.SetCheck(myExp.view0.saveSeparateSilh);
	m_showinputvideo_chck.SetCheck(myExp.view0.showInputVideo);
	m_showdenoisedchangemask.SetCheck(myExp.view0.showDenoisedChangeMask);
	m_showchangemask_chck.SetCheck(myExp.view0.showChangeMask);
	m_showprocessedarea_chck.SetCheck(myExp.view0.showProcessedArea);



	UpdateData(false);

//	TheProject.projThresh = (char)(m_Thresh);

	myExp.view0.projThresh = (char)(m_Thresh);


	//	toStop = false;
	//	askAgain = true;


	////////////////////ADD CONSULE FOR DEBUGGING////////////
#if __CONSULEOUTPUT
	AllocConsole();
	freopen("CONOUT$","w",stdout);
	freopen("CONOUT$","w",stderr);
	freopen("CONIN$","r",stdin);
#endif

	////////////////////for consule ////////////


	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CFluoreScoreDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CFluoreScoreDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CFluoreScoreDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CFluoreScoreDlg::OnFileOpen()
{	
	CFileDialog fileDlg( TRUE, NULL, NULL, OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY, (LPCTSTR)L"AVI Files (*.avi)|*.avi||");
	// Initializes m_ofn structure
	fileDlg.m_ofn.lpstrTitle = (LPCTSTR)L"Open Video File";
	// Call DoModal
	if ( fileDlg.DoModal() == IDOK)
	{

		//		if (this->TheProject.ProjInputVideo != fileDlg.GetPathName())
		{

			this->myExp.view0.inAVIFileName = fileDlg.GetPathName();
			this->myExp.view0.outCSVFileName = "";

			//this->TheProject.ProjInputVideo = fileDlg.GetPathName();
			//this->TheProject.ProjOutputFile = "";
			
		}
		UpdateData(false);
		StopResumeButt.SetWindowTextW(L"Stop");




		//std::string ss = std::string(CT2CA(TheProject.ProjInputVideo)); // Convert Video_File from CString to const Char
		//const char *cc = ss.c_str();
		//TheProject.projInputVideoCapture = cvCreateFileCapture(cc);
		//IplImage *myFrame;
		//myFrame = cvQueryFrame(TheProject.projInputVideoCapture);
		//TheProject.frameHeight = myFrame->height;
		//TheProject.frameWidth = myFrame->width;
		//TheProject.projROI.pt1 = cvPoint(0,0); // initialize the ROI , I think it should go to contructor of the project 
		//TheProject.projROI.pt2 = cvPoint(myFrame->width,myFrame->height);
		////printf("I opened the file\n");

	}
}


void CFluoreScoreDlg::OnAnalyzeAnalyze()
{
	myExp.view0.toStop = false;
	if (this->myExp.view0.inAVIFileName == "")
	//if (this->TheProject.ProjInputVideo == "")
		MessageBox ((LPCTSTR)L"Please Open an AVI file first", (LPCTSTR)L"salam");
	else 
	{
		if (this->myExp.view0.outCSVFileName == "" || myExp.view0.askAgain)
		//if (this->TheProject.ProjOutputFile == "" || TheProject.askAgain) 
			{
				// Get information for output file
				CFileDialog SaveOutputDlg ( false, NULL, NULL, OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY, (LPCTSTR)L"CSV Files (*.csv)|*.csv||");
				// Initializes m_ofn structure
				SaveOutputDlg.m_ofn.lpstrTitle = (LPCTSTR)L"Save Output file";

				CString csTemp;
				if ( SaveOutputDlg.DoModal() == IDOK)
				{
					CString appendText = L".csv";
					if(SaveOutputDlg.GetPathName().Right(4).Compare(appendText))
						csTemp =  SaveOutputDlg.GetPathName() + appendText;
					else
						csTemp =  SaveOutputDlg.GetPathName();
					//std::string s ((LPCTSTR)csTemp);
					this->myExp.view0.outCSVFileName = csTemp;

					UpdateData(false);
				}
				else
					return;
			}



		{

				if (!this->myExp.view0.isBGCalculated)
				{
					myExp.status = _T("Calculating background...");
					UpdateData(false);
					this->myExp.view0.calculateInitialBG(100); // chose 100 for number of frames to be used for background modelling
					this->myExp.view0.isBGCalculated = true;

				}


					cvSetCaptureProperty(myExp.view0.inVideoCapture,CV_CAP_PROP_POS_FRAMES,double(0.0)); // put it back


					myExp.status = _T("Processing...");
					UpdateData(false);
					this->myExp.ProcessTheVideo();
					this->myExp.close();


				//if (!TheProject.isBGCalculated)
				//{
				//	TheProject.status = _T("calculating background...");
				//	UpdateData(false);
				//	TheProject.calculateInitialBG(100);
				//	TheProject.isBGCalculated = true;


				//}

				//	TheProject.status = _T("processing...");
				//	UpdateData(false);
				//	TheProject.ProcessTheVideo();






		}
	}

}


/////////////////////////////////////////////






void CFluoreScoreDlg::OnNMReleasedcaptureThreshSlider(NMHDR *pNMHDR, LRESULT *pResult)
{
	//m_Thresh = m_Thresh_Slider.GetPos();
	//UpdateData(false);
	////TheProject.silhDetect.setThreshold( (char) (m_Thresh)); // Reza: I use the Threshold Value that I get from Slider Control
	//TheProject.projThresh = (char)(m_Thresh);
	//TheProject.askAgain = false; //changed it, getting the name of output file for each change is annoying!
	//TheProject.silhDetect.frameCount = 0;
	//this->OnAnalyzeAnalyze();
	//*pResult = 0;


	m_Thresh = m_Thresh_Slider.GetPos();
	UpdateData(false);
	//TheProject.silhDetect.setThreshold( (char) (m_Thresh)); // Reza: I use the Threshold Value that I get from Slider Control
	myExp.view0.projThresh = (m_Thresh);
	myExp.view0.askAgain = false; //changed it, getting the name of output file for each change is annoying!
	myExp.view0.silhDetect.frameCount = 0;
	this->OnAnalyzeAnalyze();
	*pResult = 0;



}

//void CFluoreScoreDlg::OnNMReleasedcaptureRedSlider(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	// TODO: Add your control notification handler code here
//	m_Red = m_Red_Slider.GetPos();
//	UpdateData(false);
//	*pResult = 0;
//}
//
//
//
//void CFluoreScoreDlg::OnNMReleasedcaptureBlueSlider(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	// TODO: Add your control notification handler code here
//	m_Blue = m_Blue_Slider.GetPos();
//	UpdateData(false);
//	*pResult = 0;
//}
//
//void CFluoreScoreDlg::OnNMReleasedcaptureGreenSlider(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	// TODO: Add your control notification handler code here
//	m_Green = m_Green_Slider.GetPos();
//	UpdateData(false);
//	*pResult = 0;
//}

void CFluoreScoreDlg::OnBnClickedOk()  //FIX THIS, THIS IS STOP/RESTART BUTTON
{
	//////TODO: Add your control notification handler code here
	//CString temp = L"Stop";
	//CString state;
	//StopResumeButt.GetWindowTextW(state);
	//if ( !temp.Compare( state ) )
	//{	 
	//	this->cVideoClose();
	//	this->TheProject.projOFile.close();
	//	StopResumeButt.SetWindowTextW(L"Restart");
	//}
	//else 
	//{
	//	StopResumeButt.SetWindowTextW(L"Stop");
	//	TheProject.askAgain = false;
	//	OnAnalyzeAnalyze();
	//}

	////TODO: Add your control notification handler code here
	CString temp = L"Stop";
	CString state;
	StopResumeButt.GetWindowTextW(state);
	if ( !temp.Compare( state ) )
	{	 
		this->cVideoClose();
		//this->TheProject.projOFile.close();
		StopResumeButt.SetWindowTextW(L"Restart");
	}
	else 
	{
		StopResumeButt.SetWindowTextW(L"Stop");
		myExp.view0.askAgain = false;
		myExp.view0.openView();
		OnAnalyzeAnalyze();
	}


}

void CFluoreScoreDlg::OnClose()
{
	this->cVideoClose();
	printf("\noutfrom it");
	OnCancel(); // To close the main dialog file
}

void CFluoreScoreDlg::cVideoClose()
{
	myExp.close();
}
void CFluoreScoreDlg::OnStnClickedFlypic()
{
	// TODO: Add your control notification handler code here
}


//void mouseHandler(int events, int x, int y, int flags, void* param)
//  {
//    switch(events){
//      case CV_EVENT_LBUTTONDOWN:
//        if(flags & CV_EVENT_FLAG_CTRLKEY) 
// //         printf("Left button down with CTRL pressed\n");
//			printf("%d and %d\n",x,y);
//        break;
//
//      case CV_EVENT_LBUTTONUP:
// //       printf("Left button up\n");
//        break;
//    }
//	return;
//  }


//  event: CV_EVENT_LBUTTONDOWN,   CV_EVENT_RBUTTONDOWN,   CV_EVENT_MBUTTONDOWN,
//         CV_EVENT_LBUTTONUP,     CV_EVENT_RBUTTONUP,     CV_EVENT_MBUTTONUP,
//         CV_EVENT_LBUTTONDBLCLK, CV_EVENT_RBUTTONDBLCLK, CV_EVENT_MBUTTONDBLCLK,
//         CV_EVENT_MOUSEMOVE:
//
//  flags: CV_EVENT_FLAG_CTRLKEY, CV_EVENT_FLAG_SHIFTKEY, CV_EVENT_FLAG_ALTKEY,
//         CV_EVENT_FLAG_LBUTTON, CV_EVENT_FLAG_RBUTTON,  CV_EVENT_FLAG_MBUTTON
//
//# Register the handler:
//



void CFluoreScoreDlg::OnAnalyzeRegionofinterest()
{

	if (this->myExp.view0.inAVIFileName == "")
		MessageBox ((LPCTSTR)L"Please Open an AVI file first", (LPCTSTR)L"Error!");
	else 
		myExp.view0.setProjROI();

}

void CFluoreScoreDlg::OnAnalyzeMasking()
{
	// TODO: Add your command handler code here
	if (this->myExp.view0.inAVIFileName == "")
		MessageBox ((LPCTSTR)L"Please Open an AVI file first", (LPCTSTR)L"Error!");
	else 
	{
		myExp.view0.setMask();
	}
}

void CFluoreScoreDlg::OnBnClickedDenoisingCheck()
{
	if (m_doDenoisingchk.GetCheck())
	{
		myExp.view0.doDenoising = true;
		printf ("I have been checked");
	}
	else
	{
		myExp.view0.doDenoising = false;
		printf ("I have been unchecked");
	}
	// TODO: Add your control notification handler code here
}

void CFluoreScoreDlg::OnBnClickedSaveseparate4silhCheck()
{
	if (m_saveSeparateforSilh.GetCheck())
	{
		myExp.view0.saveSeparateSilh = true;
		printf ("I have been checked");
	}
	else
	{
		myExp.view0.saveSeparateSilh = false;
		printf ("I have been unchecked");
	}
	// TODO: Add your control notification handler code here
}

void CFluoreScoreDlg::OnBnClickedShowInputvideoCheck()
{
	if (m_showinputvideo_chck.GetCheck())
		myExp.view0.showInputVideo = true;
	else
		myExp.view0.showInputVideo = false;

	// TODO: Add your control notification handler code here
}

void CFluoreScoreDlg::OnBnClickedShowChangemaskChck()
{
	if (m_showchangemask_chck.GetCheck())
		myExp.view0.showChangeMask = true;
	else
		myExp.view0.showChangeMask = false;
	// TODO: Add your control notification handler code here
}

void CFluoreScoreDlg::OnBnClickedShowDenoisedchangemaskChck()
{
	if(m_showdenoisedchangemask.GetCheck())
		myExp.view0.showDenoisedChangeMask = true;
	else 
		myExp.view0.showDenoisedChangeMask = false;
	// TODO: Add your control notification handler code here
}

void CFluoreScoreDlg::OnBnClickedShowProcessedareaChck()
{
	if(m_showprocessedarea_chck.GetCheck())
		myExp.view0.showProcessedArea = true;
	else
		myExp.view0.showProcessedArea = false;

	// TODO: Add your control notification handler code here
}

void CFluoreScoreDlg::OnFileExit()
{
	OnClose();
}
