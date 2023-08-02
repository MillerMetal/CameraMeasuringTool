
// SampleMFCOpenCVDlg.h : header file
//

#pragma once

// CSampleMFCOpenCVDlg dialog
using namespace cv;

#define CV_FLOODFILL_FIXED_RANGE (1 << 16)


class CSampleMFCOpenCVDlg : public CDialogEx
{
// Construction
public:
	CSampleMFCOpenCVDlg(CWnd* pParent = nullptr);	// standard constructor



// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SAMPLEMFCOPENCV_DIALOG };
#endif

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
	DECLARE_MESSAGE_MAP()

protected:
	BITMAPINFO* bmi;
	BITMAPINFOHEADER* bmih;
	RGBQUAD* palette;
	unsigned int buffer[sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256];
	long m_nImageWidth;
	long m_nImageHeight;
	BOOL m_bDontPaint;

	CString szOrgImage;
	CString szLastFileName;

public:
	afx_msg void OnBnClickedOpenImage();
	// The number of iterations the algorithm will use during the calculations.

	// Member functions
	bool UndistortBigImage(cv::Mat &cvimgBefore, cv::Mat &cvimgUndistorted);
	bool CheckMatricesLoaded(void);
	void DisplayMyData();
	bool EraseDrawingBorders(cv::Mat& cvimg);
	bool GetDefaultBorders(std::vector<cv::Point2f>& defborder);
	bool GenerateDetectMaskPolys(cv::Size sz);
	bool DrawRectDetectMaskPolys(cv::Mat& img, cv::Mat& rval);
	bool DrawRectDetectMask(cv::Mat& img, cv::Mat& rval);
	bool CalcEmptyPolys(cv::Mat& imgSrc, cv::Mat& imgResult, float** histIgnoreValsResult, int &nBins);
	void DisplayEmptyPolys(cv::Mat& img, float* histIgnoreVals, bool bUseRealRack = false);
	void ReadDefaults(bool bInit=true);
	void SaveDefaults();
	void DetectEdges(bool bDoFileOpen);
	void DetectCorners();
	void LoadCalibrationImages();
	std::vector<Vec3f> CreateObjectPoints(cv::Size sz, float w = 1.0f, float h = 1.0f);
	LONGLONG SumData(Mat& img);

	// Member variables
	long m_nNumIterations;
	long m_nSegmentLength;
	long m_nColorRadius;
	long m_nSpatialRadius;

	// defaults
	cv::Mat TheImage;
	cv::Mat cameraMatrix;
	cv::Mat distortionMatrix;
	cv::Mat newCameraMat;
	cv::Mat homographyMatrix;
	cv::Mat invHomographyMatrix;
	cv::Mat m_matFillPoints;
	std::vector<std::vector<cv::Point2f>> m_vecRackPolys;
	std::vector<std::vector<cv::Point>> m_vecMaskPolys;

	double m_dDiffOfGaussRadius1;
	double m_dDiffOfGaussSigma1;
	double m_dDiffOfGaussRadius2;
	double m_dDiffOfGaussSigma2;
	double m_dResultScale;
	double m_dResultLength;
	double m_dResultWidth;
	double m_dBorderBottom;
	double m_dBorderTop;
	double m_dBorderLeft;
	double m_dBorderRight;

	CString m_szCameraMatrix;
	CString m_szDistortionMatrix;
	CString m_szHomographyMatrix;

	long m_nFillToleranceLow;
	long m_nFillToleranceHigh;
	long m_nImageIndex;
	afx_msg void OnBnClickedOpenArucoImage();
	afx_msg void OnBnClickedOpenCalibImages();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


