#include "MyController.hpp"
//#include "framework.h"
//#include "SampleMFCOpenCV.h"
//#include "SampleMFCOpenCVDlg.h"
//#include "afxdialogex.h"
//#include "CShowImage.h"

//#include <shlwapi.h>
#include <opencv2\calib3d\calib3d.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <iostream>
#include <iomanip>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

std::vector<Vec3f> MyController::CreateObjectPoints(cv::Size sz, float w, float h)
{
    float x, y;

    std::vector<Vec3f> chessboard ;

    for (y = 0; y < sz.height; y++)
    {
        for (x = 0; x < sz.width; x++)
        {
            chessboard.push_back(cv::Vec3f(x * w, y * h, 0));
        }
    }

    return chessboard;
}

long long MyController::SumData(Mat& img)
{
    long long rval = 0;
    long i;
    long nSize=0 ;
    uchar* pData;

    if (img.isContinuous()) 
    {
        nSize = (long long) img.elemSize();
        nSize = (long long) img.elemSize1();
        nSize = (long long)img.size().width * (long long)img.size().height * (long long)img.elemSize();
    }
    pData = img.data;

    if (nSize > 0) 
    {
        for (i = 0; i < nSize; i++)
        {
            rval += *pData;
            pData++;
        }
    }

    return rval;
}

string serializeVec2f(Vec2f& vec) 
{
    string rval;
    stringstream ss;

    ss << setprecision(7) << vec[0] << "," << vec[1];
    rval = ss.str();

    //rval.Format("%.7f,%.7f ", vec[0], vec[1]);
    return rval;
}


void MyController::LoadCalibrationImages()
{
	//std::vector<cv::Mat> cvimgsColor;
	//cv::Size patternSize(9,7);
	//vector<cv::Mat> rotationVectors;
	//vector<cv::Mat> translationVectors;
 //   int i,j;
	//bool bShow = false;
 //   double error = 0.0;
 //   float sqW = 1.0f;
 //   float sqH = 0.997f;
 //   CShowImage* pWndShowImage = new CShowImage(this);

 //   CFileDialog openPic(TRUE, 0, 0, OFN_ALLOWMULTISELECT | OFN_FILEMUSTEXIST, "Image Files (*.jpg;*.jpeg;*.jiff;*.bmp;*.png;*.tiff)|*.jpg;*.jpeg;*.jiff;*.bmp;*.png;*.tiff|"
 //       "All Files (*.*)|*.*||", NULL);

 //   openPic.m_ofn.lpstrTitle = "Open Calibration Images - Select Multiple Frames";

 //   // Create buffer for file names.
 //   const DWORD numberOfFileNames = 100;
 //   const DWORD fileNameMaxLength = MAX_PATH + 1;
 //   const DWORD bufferSize = (numberOfFileNames * fileNameMaxLength) + 1;
 //   TCHAR* filenamesBuffer = new TCHAR[bufferSize];

 //   // Initialize beginning and end of buffer.
 //   filenamesBuffer[0] = NULL;
 //   filenamesBuffer[bufferSize - 1] = NULL;

 //   // Attach buffer to OPENFILENAME member.
 //   openPic.m_ofn.lpstrFile = filenamesBuffer;
 //   openPic.m_ofn.nMaxFile = bufferSize;

 //   // Create array for file names.
 //   CString fileNameArray[numberOfFileNames];
 //   int iCtr = 0;

 //   if (openPic.DoModal() == IDOK)
 //   {
 //       // Retrieve file name(s).
 //       POSITION fileNamesPosition = openPic.GetStartPosition();
 //       while (fileNamesPosition != NULL)
 //       {
 //           fileNameArray[iCtr] = openPic.GetNextPathName(fileNamesPosition);
 //           iCtr++;
 //       }
 //   }
 //   else
 //   {
 //       delete pWndShowImage;
 //       return;
 //   }

 //   vector<vector<cv::Vec2f>> _cornersPointsVec(iCtr);
 //   vector<vector<cv::Vec3f>> _cornersObjectList(iCtr);
 //   vector<vector<cv::Vec2f>> _cornersPointsList(iCtr);

 //   // Open all of the calibration images
 //   for (i = 0; i < iCtr; i++)
 //   {
 //       //CString tstr;
 //       cv::Mat tempmat;
 //       cv::String cvstr;

 //       //tstr = CString("MainWindow - Loading Image ") + (LPCTSTR)to_string(i).c_str() + CString(" of ") + (LPCTSTR)to_string(iCtr).c_str();
 //       //AfxGetMainWnd()->SetWindowText(tstr);
 //       cvstr = fileNameArray[i];

 //       tempmat = cv::imread(cvstr, IMREAD_ANYCOLOR);

 //       cvimgsColor.push_back(tempmat);
 //   }

 //   Mat cvimgGray;
 //   Mat cvimgAdaptThresh;
 //   Mat cvimgErode;
 //   Mat gdiGreyImage;
 //   Mat cvimgColorCopy;
 //   //Mat gdiGreyImage;

 //   LONGLONG sum;

 //   CString outstr;

 //   for (i = 0; i < iCtr; i++)
 //   {
 //       CString tstr;
 //       cv::String cvstr;

 //       tstr = CString("MainWindow - PreProcessing Image ") + (LPCTSTR)to_string(i).c_str() + CString(" of ") + (LPCTSTR)to_string(iCtr).c_str();
 //       AfxGetMainWnd()->SetWindowText(tstr);

 //       // First, convert the bitmap to gray
 //       //sum = SumData(cvimgsColor[i]);

 //       cv::cvtColor(cvimgsColor[i], cvimgGray, COLOR_BGR2GRAY);
 //       //sum = SumData(cvimgGray);

 //       cv::threshold(cvimgGray, cvimgAdaptThresh, 220, 255, cv::THRESH_BINARY);
 //       //sum = SumData(cvimgAdaptThresh);

 //       cv::erode(cvimgAdaptThresh, cvimgErode, Mat(), cv::Point(-1,-1), 1);
 //       //sum = SumData(cvimgErode);

 //       // Display the bitmap in predefined area
 //       //gdiGreyImage.Source = <Gray, Byte>(cvimgErode);


 //       // region Next, Find the chess board corners
 //       // _cornersPointsVec[i] = new VectorOfPointF();
 //       bool bFound;
 //       Vec2f m;

 //       bFound = cv::findChessboardCorners(cvimgGray, patternSize, _cornersPointsVec[i]);

 //       if (bFound == false)
 //           MessageBox("Couldn't find corners for " + fileNameArray[i], NULL, NULL);

 //       //region Drawand display the corners on a color copy of the image
 //       if (bFound)
 //       {
 //           TermCriteria criteria = TermCriteria(TermCriteria::EPS + TermCriteria::COUNT, 30, 0.1);

 //           cv::cornerSubPix(cvimgGray, _cornersPointsVec[i], cv::Size(11, 11), cv::Size(-1, -1), criteria);

 //           //Emgu.CV.Image<Bgr, Byte> cvimgColorCopy = cvimgsColor[i].Copy();

 //           _cornersObjectList[i] = CreateObjectPoints(patternSize, sqW, sqH);
 //           _cornersPointsList[i] = _cornersPointsVec[i];

 //           // IMPORTANT NOTE!!!  DrawChessboardCorners requires that the cornersPointsVec
 //           // be a PointF (float), not an int or a double.
 //           cv::drawChessboardCorners(cvimgColorCopy, patternSize, _cornersPointsVec[i], bFound);

 //           // Put the picture up on the screen...
 //           //gdiGreyImage.Source = ToBitmapSource<Bgr, Byte>(cvimgColorCopy);
 //       }
 //       else
 //       {
 //       }


 //       if (bShow)
 //       {
 //           tstr = CString("Image ") + (LPCSTR)to_string(i).c_str() + CString("  Press OK to continue.");
 //           if (MessageBox(tstr,NULL,MB_OKCANCEL) != IDOK)
 //               bShow = false;
 //       }
 //   }

 //   cv::calibrateCamera(_cornersObjectList
 //       , _cornersPointsList
 //       , cvimgsColor[0].size()
 //       , cameraMatrix
 //       , distortionMatrix
 //       , rotationVectors
 //       , translationVectors
 //       , 0
 //       , TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 250, 0.001)
 //   );
 //       


 //   cv::Rect rc ;
 //   Mat rval;
 //   Mat cvimgColorResult;
 //   Mat cvimgSrc;

 //   if (i > 0)
 //       cvimgsColor[0].copyTo(cvimgSrc);
 //   rval = cv::getOptimalNewCameraMatrix(cameraMatrix, distortionMatrix, cvimgsColor[0].size(), 1, cvimgsColor[0].size(), &rc);

 //   bool bShowMsg = true;

 //   SaveDefaults();

 //   pWndShowImage->Create(CShowImage::IDD);
 //   pWndShowImage->ShowWindow(SW_SHOW);
 //   pWndShowImage->UpdateWindow();

 //   for (i = 0; i < cvimgsColor.size(); i++)
 //   {
 //       //Title = "MainWindow - Processing Image " + i.ToString() + " of " + cvimgsColor.Count;
 //       cvimgsColor[i].copyTo(cvimgSrc);
 //       
 //       cv::undistort(cvimgsColor[i], cvimgColorResult, cameraMatrix, distortionMatrix);
 //       cv::drawChessboardCorners(cvimgSrc, patternSize, _cornersPointsVec[i], true);
 //       if (bShowMsg==true)
 //           cv::imshow(cv::String("Show Image"), cvimgSrc);


 //       if (bShowMsg==true)
 //       {
 //           int res;

 //           res = MessageBox(CString("Displaying Undistorted Image ")
 //               + (LPCTSTR)to_string(i).c_str()
 //               + CString("  Press OK to continue, or Cancel (Esc) to stop showing this message.")
 //               , "Progress"
 //               , MB_OKCANCEL
 //           );

 //           if (res != MB_OK)
 //               bShowMsg = false;
 //       }
 //   }

 //   if (pWndShowImage != NULL)
 //   {
 //       pWndShowImage->DestroyWindow();
 //       delete pWndShowImage;
 //   }
}

