#include "MyController.hpp"
//#include "framework.h"
//#include "SampleMFCOpenCV.h"
//#include "SampleMFCOpenCVDlg.h"
//#include "afxdialogex.h"
#include <map>
#include <filesystem>

#include <opencv2/objdetect/aruco_detector.hpp>
//#include <shlwapi.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


class CLineSegmentF {
public:
    CLineSegmentF(cv::Point2f& pt1, cv::Point2f& pt2) { P1 = pt1; P2 = pt2; }

public:
    cv::Point2f P1;
    cv::Point2f P2;
};

void MyController::DetectCorners(InputArray buf) 
{
	//CFileDialog openPic(TRUE, 0, 0, OFN_ALLOWMULTISELECT | OFN_FILEMUSTEXIST, "Image Files (*.jpg;*.jpeg;*.jiff;*.bmp;*.png;*.tiff)|*.jpg;*.jpeg;*.jiff;*.bmp;*.png;*.tiff|"
	//	"All Files (*.*)|*.*||", NULL);
    //const std::filesystem::path mypath = szFileName;

	//openPic.m_ofn.lpstrTitle = "Open Aruco Image";

	if (cameraMatrix.empty() == true || distortionMatrix.empty() == true)
		ReadDefaults(false);   // false so we don't reinitialize the data instead of reading it.

    //if (szFileName.length() == 0)

    cv::Size sz = buf.size();
    if (sz.width == 0 || sz.height == 0)
    {
        return;
    }

    //string pstr = mypath.filename().generic_string();
    
	//AfxGetMainWnd()->SetWindowText("MainWindow - Calibration Image Loaded - " + szFileNameOnly);

    vector<Mat> cvimgsColor;
    Mat newimg;

    //newimg = imdecode(buf, IMREAD_COLOR);//    imread(cv::String(szFileName));
    // Open the target image
    cvimgsColor.push_back(newimg.clone());

    // Return Value allocation
    vector<vector<cv::Point2f>> myMarkerCorners;
    vector<vector<cv::Point2f>> myRejects;
    vector<int> myMarkerIds;

    // Get the default parameters for the conversion
    //DetectorParameters myDetectorParams = new DetectorParameters();
    //myDetectorParams = DetectorParameters.GetDefault();
    cv::aruco::DetectorParameters myDetectorParams = cv::aruco::DetectorParameters();
    cv::aruco::Dictionary dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_ARUCO_ORIGINAL);
    cv::aruco::ArucoDetector detector(dictionary, myDetectorParams);

    // Some image result holders
    Mat cvimgColorBefore; // = imdecode(buf, IMREAD_COLOR); //imread(cv::String(szFileName));
    Mat cvimgColor; // = imdecode(buf, IMREAD_COLOR); //imread(cv::String(szFileName));
    Mat cvimgGrayBefore;
    Mat cvimgGray;

    //cvimgColor.convertTo(cvimgGrayBefore, CV_8UC1);
    //cvimgColor.convertTo(cvimgGray, CV_8UC1);
    cv::cvtColor(cvimgColor, cvimgGrayBefore, COLOR_BGR2GRAY);
    cv::cvtColor(cvimgColor, cvimgGray, COLOR_BGR2GRAY);

    // Using the camera calibration data from the checkerboard patterns, and the 
    // distortion array calculated from those images, undistort the images so it's
    // a TRUE camera projection with no induced curves.  We use our own undistort
    // so that we can get the maximum amount of pixels.
    UndistortBigImage(cvimgGrayBefore, cvimgGray);
    UndistortBigImage(cvimgColorBefore, cvimgColor);

    // Store a gray result image
    /*************************************************/
    cv::cvtColor(cvimgColor, cvimgGrayBefore, COLOR_BGR2GRAY);
    detector.detectMarkers(cvimgGrayBefore, myMarkerCorners, myMarkerIds, myRejects);

    if (myMarkerCorners.size() == 4)
    {
        // We're looking for the corners with the following IDs, in the following order:
        cv::aruco::drawDetectedMarkers(cvimgGrayBefore, myMarkerCorners, myMarkerIds, cv::Scalar(255, 0, 255));
        //Emgu.CV.Aruco.


            
        std::map<int, int> mymap = {
            {430, 0},
            {219, 0},
            {338, 1},
            {908, 2}
        };

        std::map<int, cv::Point2f> mappts;

        float dScaleFactor = 32.0F;

        // The dimensions must not include the white border, which is 1/2"

        float dLength = 133.25F;      // 134.25" - 1"
        float dLength2 = 130.0F;        // 131" - 1"
        float dWidth = 83.9375F;      //  84-15/16" - 1"


        /*
        float dLength = 134.25F;
        float dLength2 = 131F;
        float dWidth = 84.9375F;
        */
        mappts.insert({ 430, cv::Point2f(dLength2 * dScaleFactor, dWidth * dScaleFactor) });
        mappts.insert({ 219, cv::Point2f(0.0F, dWidth * dScaleFactor) });
        mappts.insert({ 338, cv::Point2f(0.0F, 0.0F) });
        mappts.insert({ 908, cv::Point2f(dLength * dScaleFactor, 0.0F) });

            
        CLineSegmentF line1(myMarkerCorners[0][mymap[myMarkerIds[0]]], myMarkerCorners[1][mymap[myMarkerIds[1]]]);
        CLineSegmentF line2(myMarkerCorners[1][mymap[myMarkerIds[1]]], myMarkerCorners[2][mymap[myMarkerIds[2]]]);
        CLineSegmentF line3(myMarkerCorners[2][mymap[myMarkerIds[2]]], myMarkerCorners[3][mymap[myMarkerIds[3]]]);
        CLineSegmentF line4(myMarkerCorners[3][mymap[myMarkerIds[3]]], myMarkerCorners[0][mymap[myMarkerIds[0]]]);

        cv::line(cvimgColor, line1.P1, line1.P2, cv::Scalar(255, 0, 128), 1);
        cv::line(cvimgColor, line2.P1, line2.P2, cv::Scalar(255, 0, 128), 1);
        cv::line(cvimgColor, line3.P1, line3.P2, cv::Scalar(255, 0, 128), 1);
        cv::line(cvimgColor, line4.P1, line4.P2, cv::Scalar(255, 0, 128), 1);
            
        vector<Point2f> srcs;
        srcs.push_back(myMarkerCorners[0][mymap[myMarkerIds[0]]]);
        srcs.push_back(myMarkerCorners[1][mymap[myMarkerIds[1]]]);
        srcs.push_back(myMarkerCorners[2][mymap[myMarkerIds[2]]]);
        srcs.push_back(myMarkerCorners[3][mymap[myMarkerIds[3]]]);
                
        vector<Point2f> dsts;
        dsts.push_back(mappts[myMarkerIds[0]]);
        dsts.push_back(mappts[myMarkerIds[1]]);
        dsts.push_back(mappts[myMarkerIds[2]]);
        dsts.push_back(mappts[myMarkerIds[3]]);

        cv::Mat homog = findHomography(srcs, dsts, noArray());
        invert(homog, invHomographyMatrix);

        std::string tstr;
        std::string lookat;

        lookat = "camera mat\n";
        lookat << cameraMatrix;
        tstr = lookat.c_str();

        lookat = "homography mat\n";
        lookat << homog;
        tstr += "\n";
        tstr += lookat.c_str();

        lookat = "distortion mat\n";
        lookat << distortionMatrix;
        tstr += "\n";
        tstr += lookat.c_str();

            
        if (homographyMatrix.empty()==false)
        {
            homog.copyTo(homographyMatrix);
        }

        m_dResultScale = dScaleFactor;
        m_dResultLength = dLength;
        m_dResultWidth = dWidth;
        m_dBorderBottom = 9.6875;
        m_dBorderTop = 10.75;
        m_dBorderLeft = 6.75;
        m_dBorderRight = 2.5;

        SaveDefaults();

        cv::Mat cvimgDewarped;
        warpPerspective(cvimgGray, cvimgDewarped, homog, cv::Size((int)(dLength * dScaleFactor), (int)(dWidth * dScaleFactor)), InterpolationFlags::INTER_CUBIC, BorderTypes::BORDER_DEFAULT, cv::Scalar(0, 0, 0));

        //gdiGreyImage.Source = ToBitmapSource();
    }
    else
    if (myMarkerCorners.size() > 4)
    {
        // Need to throw up a warning at this point
        // 
        //AfxGetMainWnd()->MessageBox("Warning:  Found more aruco markers than expected");
        // We're looking for the corners with the following IDs, in the following order:
        //ArucoInvoke.DrawDetectedMarkers(imgInputAruco, myMarkerCorners, myMarkerIds, new MCvScalar(255, 0, 255));

        /*************************************************/
        //gdiGreyImage.Source = ToBitmapSource(imgInputAruco);
    }

	
}