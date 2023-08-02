
// SampleMFCOpenCVDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "SampleMFCOpenCV.h"
#include "SampleMFCOpenCVDlg.h"
#include "afxdialogex.h"

#include <shlwapi.h>

//#include <msxml6.h>
//#include <MsXml2.h>


// Macro that calls a COM method returning HRESULT value.
#define CHK_HR(stmt)        do{ hr=(stmt); if (FAILED(hr)) goto CleanUp; } while(0)

// Macro to verify memory allcation.
#define CHK_ALLOC(p)        do { if (!(p)) { hr = E_OUTOFMEMORY; goto CleanUp; } } while(0)

// Macro that releases a COM object if not NULL.
#define SAFE_RELEASE(p)     do { if ((p)) { (p)->Release(); (p) = NULL; } } while(0)


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CSampleMFCOpenCVDlg dialog



CSampleMFCOpenCVDlg::CSampleMFCOpenCVDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SAMPLEMFCOPENCV_DIALOG, pParent)
	, m_nSpatialRadius(0)
	, m_nColorRadius(0)
	, m_nSegmentLength(0)
	, m_nNumIterations(0)
	, m_dDiffOfGaussRadius1(0)
	, m_dDiffOfGaussSigma1(0)
	, m_dDiffOfGaussRadius2(0)
	, m_dDiffOfGaussSigma2(0)
	, m_nImageIndex(0)
	, m_nFillToleranceLow(0)
	, m_nFillToleranceHigh(0)
	, m_dResultLength(0.0)
	, m_dResultWidth(0.0)
	, m_dBorderBottom(0.0)
	, m_dBorderTop(0.0)
	, m_dBorderLeft(0.0)
	, m_dBorderRight(0.0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	TheImage = NULL;
	m_bDontPaint = FALSE;

	ReadDefaults(false);
}

void CSampleMFCOpenCVDlg::ReadDefaults(bool bInit) {
	FileStorage fs;
	cv::String tstr;

	if (bInit) {
		m_nSpatialRadius = 10;
		m_nColorRadius = 30;
		m_nSegmentLength = 75;
		m_nNumIterations = 20;

		m_dDiffOfGaussRadius1 = 1;
		m_dDiffOfGaussSigma1 = 1.0;
		m_dDiffOfGaussRadius2 = 2;
		m_dDiffOfGaussSigma2 = 1.0;

		m_nFillToleranceLow = 10;
		m_nFillToleranceHigh = 10;

		m_dResultLength = 0;
		m_dResultWidth = 0;
		m_dBorderBottom = 9.6875;
		m_dBorderTop = 10.75;
		m_dBorderLeft = 6.75;
		m_dBorderRight = 2.5;

		cameraMatrix.create(3, 3, CV_64F);
		homographyMatrix.create(3, 3, CV_64F);
		distortionMatrix.create(5, 1, CV_64F);
	}
	else
	{
		m_nSpatialRadius = AfxGetApp()->GetProfileInt("Mean Shift Segmentation", "Spatial Radius", 10);
		m_nColorRadius = AfxGetApp()->GetProfileInt("Mean Shift Segmentation", "Color Radius", 30);
		m_nSegmentLength = AfxGetApp()->GetProfileInt("Mean Shift Segmentation", "Segment Length", 75);
		m_nNumIterations = AfxGetApp()->GetProfileInt("Mean Shift Segmentation", "Num Iterations", 20);

		m_dDiffOfGaussRadius1 = AfxGetApp()->GetProfileInt("Difference Of Gaussians", "Radius 1", 1);
		m_dDiffOfGaussSigma1 = atof(AfxGetApp()->GetProfileString("Difference Of Gaussians", "Sigma 1", "1.0"));
		m_dDiffOfGaussRadius2 = AfxGetApp()->GetProfileInt("Difference Of Gaussians", "Radius 2", 2);
		m_dDiffOfGaussSigma2 = atof(AfxGetApp()->GetProfileString("Difference Of Gaussians", "Sigma 2", "1.0"));

		m_nFillToleranceLow = AfxGetApp()->GetProfileInt("Fill Tolerance", "Low", 10);
		m_nFillToleranceHigh = AfxGetApp()->GetProfileInt("Fill Tolerance", "High", 10);

		m_dResultScale = atof(AfxGetApp()->GetProfileString("Measure Area", "ResultScale", "0.0"));
		m_dResultLength = atof(AfxGetApp()->GetProfileString("Measure Area", "ResultLength", "0.0"));
		m_dResultWidth = atof(AfxGetApp()->GetProfileString("Measure Area", "ResultWidth", "0.0"));
		m_dBorderBottom = atof(AfxGetApp()->GetProfileString("Measure Area", "BorderBottom", "0.0"));
		m_dBorderTop = atof(AfxGetApp()->GetProfileString("Measure Area", "BorderTop", "0.0"));
		m_dBorderLeft = atof(AfxGetApp()->GetProfileString("Measure Area", "BorderLeft", "0.0"));
		m_dBorderRight = atof(AfxGetApp()->GetProfileString("Measure Area", "BorderRight", "0.0"));

		tstr = AfxGetApp()->GetProfileStringA("Calibration", "Matrices", "");
		if (tstr.length() > 0)
		{
			fs.open(tstr, FileStorage::READ | FileStorage::MEMORY, cv::String("UTF-8"));
			fs["Camera"] >> cameraMatrix;
			fs["Homography"] >> homographyMatrix;
			fs["Distortion"] >> distortionMatrix;
			fs.release();
		}

		std::string lookat;

		lookat = "camera mat\n";
		lookat << cameraMatrix;
		tstr = lookat.c_str();

		lookat = "homography mat\n";
		lookat << homographyMatrix;
		tstr = lookat.c_str();

		lookat = "distortion mat\n";
		lookat << distortionMatrix;
		tstr = lookat.c_str();




		if (homographyMatrix.rows > 0)
			invert(homographyMatrix, invHomographyMatrix);
		else
			invHomographyMatrix.create(3, 3, CV_64F);


		if (cameraMatrix.rows == 0 || cameraMatrix.cols == 0)
			cameraMatrix.create(3, 3, CV_64F);
		if (homographyMatrix.rows == 0 || homographyMatrix.cols == 0)
			homographyMatrix.create(3, 3, CV_64F);
		if (distortionMatrix.rows == 0 || distortionMatrix.cols == 0)
			distortionMatrix.create(5, 1, CV_64F);
	}
}


void CSampleMFCOpenCVDlg::SaveDefaults() {
	CString tstr;

	AfxGetApp()->WriteProfileInt("Mean Shift Segmentation", "Spatial Radius", m_nSpatialRadius);
	AfxGetApp()->WriteProfileInt("Mean Shift Segmentation", "Color Radius", m_nColorRadius);
	AfxGetApp()->WriteProfileInt("Mean Shift Segmentation", "Segment Length", m_nSegmentLength);
	AfxGetApp()->WriteProfileInt("Mean Shift Segmentation", "Num Iterations", m_nNumIterations);

	AfxGetApp()->WriteProfileInt("Difference Of Gaussians", "Radius 1", (int)m_dDiffOfGaussRadius1);
	tstr.Format("%.8lf", m_dDiffOfGaussSigma1);
	AfxGetApp()->WriteProfileString("Difference Of Gaussians", "Sigma 1", tstr);
	AfxGetApp()->WriteProfileInt("Difference Of Gaussians", "Radius 2", (int)m_dDiffOfGaussRadius2);
	tstr.Format("%.8lf", m_dDiffOfGaussSigma2);
	AfxGetApp()->WriteProfileString("Difference Of Gaussians", "Sigma 2", tstr);

	AfxGetApp()->WriteProfileInt("Fill Tolerance", "Low", m_nFillToleranceLow);
	AfxGetApp()->WriteProfileInt("Fill Tolerance", "High", m_nFillToleranceHigh);

	FileStorage fs;
	cv::String mstr;

	tstr.Format("%.8lf", m_dResultScale);
	AfxGetApp()->WriteProfileString("Measure Area", "ResultScale", tstr);
	tstr.Format("%.8lf", m_dResultLength);
	AfxGetApp()->WriteProfileString("Measure Area", "ResultLength", tstr);
	tstr.Format("%.8lf", m_dResultWidth);
	AfxGetApp()->WriteProfileString("Measure Area", "ResultWidth", tstr);
	tstr.Format("%.8lf", m_dBorderBottom);
	AfxGetApp()->WriteProfileString("Measure Area", "BorderBottom", tstr);
	tstr.Format("%.8lf", m_dBorderTop);
	AfxGetApp()->WriteProfileString("Measure Area", "BorderTop", tstr);
	tstr.Format("%.8lf", m_dBorderLeft);
	AfxGetApp()->WriteProfileString("Measure Area", "BorderLeft", tstr);
	tstr.Format("%.8lf", m_dBorderRight);
	AfxGetApp()->WriteProfileString("Measure Area", "BorderRight", tstr);


	fs.open("Jerry.xml", FileStorage::WRITE | FileStorage::MEMORY, cv::String("UTF-8"));
	fs << "Camera" << cameraMatrix;
	fs << "Homography" << homographyMatrix;
	fs << "Distortion" << distortionMatrix;
	mstr = fs.releaseAndGetString();
	AfxGetApp()->WriteProfileString("Calibration", "Matrices", mstr.c_str());
}

void CSampleMFCOpenCVDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_Mean_Shift_Segmentation_Num_Iterations, m_nNumIterations);
	DDX_Text(pDX, IDC_Mean_Shift_Segmentation_Segment_Length, m_nSegmentLength);
	DDX_Text(pDX, IDC_Mean_Shift_Segmentation_Color_Radius, m_nColorRadius);
	DDX_Text(pDX, IDC_Mean_Shift_Segmentation_Spatial_Radius, m_nSpatialRadius);
	DDX_Text(pDX, IDC_Diff_Of_Gauss_Radius_1, m_dDiffOfGaussRadius1);
	DDX_Text(pDX, IDC_Diff_Of_Gauss_Sigma_1, m_dDiffOfGaussSigma1);
	DDX_Text(pDX, IDC_Diff_Of_Gauss_Radius_2, m_dDiffOfGaussRadius2);
	DDX_Text(pDX, IDC_Diff_Of_Gauss_Sigma_2, m_dDiffOfGaussSigma2);
	DDX_Text(pDX, IDC_Fill_Tolerance_Low, m_nFillToleranceLow);
	DDX_Text(pDX, IDC_Fill_Tolerance_High, m_nFillToleranceHigh);
	DDX_Text(pDX, IDC_Image_Index, m_nImageIndex);
}

BEGIN_MESSAGE_MAP(CSampleMFCOpenCVDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_OPEN_IMAGE, &CSampleMFCOpenCVDlg::OnBnClickedOpenImage)
	ON_BN_CLICKED(IDC_Open_Aruco_Image, &CSampleMFCOpenCVDlg::OnBnClickedOpenArucoImage)
	ON_BN_CLICKED(IDC_Open_Calib_Images, &CSampleMFCOpenCVDlg::OnBnClickedOpenCalibImages)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CSampleMFCOpenCVDlg message handlers

BOOL CSampleMFCOpenCVDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	SaveDefaults();

	Point2f a(0.3f, 0.f), b(0.f, 0.4f);
	Point pt = (a + b) * 10.f;

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
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

	// TODO: Add extra initialization here
	// 
	// Create the IPL image

	/*
    CvSize ImgSize;
    ImgSize.width = (int)IMAGE_WIDTH;
    ImgSize.height = (int)IMAGE_HEIGHT;
	TheImage = cv::Mat::Mat(ImgSize, CV_8UC(IMAGE_CHANNELS));

	// Initialize the IPL image -actually not necessary

	//Grayscale
	if (TheImage.channels() == 1)
	{
		float dx = (TheImage.cols / 256.0f);

		for (int h = 0; h < TheImage.cols; h++)
			for (int w = 0; w < TheImage.cols; w++)
				TheImage.at<uchar>(h,w) = (char)(w / dx); // Copy data to ipl
	}
	else if (TheImage.channels() == 3) //The image is RGB
	{
		int h, w;
		float dx = (TheImage.cols / 256.0f);

		for (h = 0; h < TheImage.cols; h++)
			for (w = 0; w < TheImage.cols; w++)
				TheImage.at<Vec3b>(h, w)[0] = (char)(w / dx); // Copy data to ipl

		for (h = 0; h < TheImage.cols; h++)
			for (w = 0; w < TheImage.cols; w++)
				TheImage.at<Vec3b>(h, w)[1] = (char)(255 - w / dx); // Copy green data to ipl

		for (h = 0; h < TheImage.cols; h++)
			for ( w = 0; w < TheImage.cols; w++)
				TheImage.at<Vec3b>(h, w)[2] = (char)(w / dx); // Copy data to ipl
	}*/

	//Initialize the BMP display buffer
	if (TheImage.empty()==false)
	{
		bmi = (BITMAPINFO*)buffer;
		bmih = &(bmi->bmiHeader);
		memset(bmih, 0, sizeof(*bmih));
		bmih->biSize = sizeof(BITMAPINFOHEADER);
		bmih->biWidth = TheImage.cols;
		bmih->biHeight = TheImage.rows;
		bmih->biPlanes = 1;
		bmih->biCompression = BI_RGB;
		bmih->biBitCount = 8 * TheImage.channels();
		palette = bmi->bmiColors;
		if (TheImage.channels() == 1)
		{
			for (int i = 0; i < 256; i++)
			{
				palette[i].rgbBlue = palette[i].rgbGreen = palette[i].rgbRed = (BYTE)i;
				palette[i].rgbReserved = 0;
			}
		}
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSampleMFCOpenCVDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSampleMFCOpenCVDlg::OnPaint()
{
	if (IsIconic())
	{
		DisplayMyData();
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
		DisplayMyData();
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSampleMFCOpenCVDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

bool CSampleMFCOpenCVDlg::CheckMatricesLoaded() 
{ 
	return false; 
}


void CSampleMFCOpenCVDlg::DisplayMyData()
{
	CPaintDC dc(this);
	CDC* pDC = &dc;

	if (!m_bDontPaint) 
	{
		bmi = (BITMAPINFO*)buffer;
		bmih = &(bmi->bmiHeader);
		memset(bmih, 0, sizeof(*bmih));
		bmih->biSize = sizeof(BITMAPINFOHEADER);
		bmih->biWidth = TheImage.cols;
		bmih->biHeight = TheImage.rows;
		bmih->biPlanes = 1;
		bmih->biCompression = BI_RGB;
		bmih->biBitCount = 8 * TheImage.channels();
		palette = bmi->bmiColors;

		CRect rc;

		GetClientRect(rc);

		if (TheImage.empty() == false)
		{
			int res = StretchDIBits(
				pDC->GetSafeHdc(), //dc
				0, //x dest
				0, //y dest
				int(rc.Width()), //x dest dims
				int(TheImage.rows*rc.Width()/TheImage.cols), //y dest dims
				0, // src x
				0, // src y
				int(TheImage.cols), //x src dims
				int(TheImage.rows), //y src dims
				TheImage.data, // the image bits
				(BITMAPINFO*)bmi, // bitmap information
				DIB_RGB_COLORS, // RGB or palette indexes
				SRCCOPY); // raster operation code

			// Update Window, force View to redraw.
			RedrawWindow(
				NULL, // handle to window
				NULL, // update rectangle
				RDW_INVALIDATE // handle to update region
			);
		}
	}
}

void CSampleMFCOpenCVDlg::OnBnClickedOpenImage()
{
	DetectEdges(true);
}



void CSampleMFCOpenCVDlg::OnBnClickedOpenArucoImage()
{
	DetectCorners();
}


void CSampleMFCOpenCVDlg::OnBnClickedOpenCalibImages()
{
	// TODO: Add your control notification handler code here
	LoadCalibrationImages();
}


void CSampleMFCOpenCVDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	InvalidateRect(NULL, TRUE);
	UpdateWindow();
}
