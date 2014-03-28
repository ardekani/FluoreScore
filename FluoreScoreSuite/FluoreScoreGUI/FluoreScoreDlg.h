// FluoreScoreDlg.h : header file
//

#pragma once
#include <string.h>
#include "afxcmn.h"
#include "afxwin.h"
#include <fstream>
#include "Silhdetector.h"
//#include "cProjectCapsule.h"
#include "Experiment.h"
#include "Resource.h"


// CFluoreScoreDlg dialog
class CFluoreScoreDlg : public CDialog
{
// Construction
public:
	CFluoreScoreDlg(CWnd* pParent = NULL);	// standard constructor

	void cVideoClose();

// Dialog Data
	enum { IDD = IDD_FluoreScore_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP();
//	std::fstream OFile;
//	cpuSilhDetector silhDetect; // Reza, I Dont like it here, but I haven't find a better place for it yet!

//	cProjectCapsule TheProject;
	CExperiment myExp;

	
//	bool toStop, askAgain;

public:
	afx_msg void OnFileOpen();



//	CString Video_file;
	
	afx_msg void OnAnalyzeAnalyze();
	// Reza this variable is used to show the current value of Threshold for Silh detection
	int m_Thresh;
	int m_Red;
	int m_Green;
	int m_Blue;



	// Reza ROI (from left top point and counter-clockwise)

//	void mouseHandler(int events, int x, int y, int flags, void* param);// decleration for mouse event handler
	

	// Reza Controls Slider for Threshold



	CSliderCtrl m_Thresh_Slider;
	CSliderCtrl m_Red_Slider;
	CSliderCtrl m_Green_Slider;
	CSliderCtrl m_Blue_Slider;
	afx_msg void OnNMReleasedcaptureThreshSlider(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReleasedcaptureRedSlider(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReleasedcaptureBlueSlider(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReleasedcaptureGreenSlider(NMHDR *pNMHDR, LRESULT *pResult);

	

//	CString Video_file;
//	CString Output_FileName;

	afx_msg void OnBnClickedOk();
	afx_msg void OnClose();
	CButton StopResumeButt;

	afx_msg void OnStnClickedFlypic();
public:
	afx_msg void OnAnalyzeRegionofinterest();

	afx_msg void OnAnalyzeMasking();
	//bool m_doDenoising4dlg;
	afx_msg void OnBnClickedDenoisingCheck();
	CButton m_doDenoisingchk;
	CButton m_saveSeparateforSilh;
	afx_msg void OnBnClickedSaveseparate4silhCheck();
	CButton m_showinputvideo_chck;
	CButton m_showchangemask_chck;
	CButton m_showdenoisedchangemask;
	CButton m_showprocessedarea_chck;
	afx_msg void OnBnClickedShowInputvideoCheck();
	afx_msg void OnBnClickedShowChangemaskChck();
	afx_msg void OnBnClickedShowDenoisedchangemaskChck();
	afx_msg void OnBnClickedShowProcessedareaChck();
	CString m_status;
	afx_msg void OnFileExit();
};
