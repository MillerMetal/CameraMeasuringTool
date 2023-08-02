#include "MyController.hpp"
//#include "framework.h"
//#include "SampleMFCOpenCV.h"
//#include "SampleMFCOpenCVDlg.h"
//#include "afxdialogex.h"

//#include <shlwapi.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


bool MyController::EraseDrawingBorders(cv::Mat& cvimg)
{
	cv::Scalar black;
	vector<cv::Point> polytop;
	vector<cv::Point> polyright;
	vector<cv::Point> polybottom;
	vector<cv::Point> polyleft;
	std::vector<cv::Point2f> dstBorders;
	std::vector<cv::Point2f> srcBorders;

	if (cvimg.channels() == 3)
		black = cv::Scalar(0.0, 0.0, 0.0);
	else
	if (cvimg.channels() == 4)
		black = cv::Scalar(0.0, 0.0, 0.0, 255.0);

	if (GetDefaultBorders(dstBorders) == false)
		return false;

	cv::perspectiveTransform(dstBorders, srcBorders, invHomographyMatrix);

	polytop.push_back(cv::Point(0,0));
	polytop.push_back(srcBorders[0]);
	polytop.push_back(srcBorders[1]);
	polytop.push_back(cv::Point(cvimg.cols, 0));

	polyright.push_back(cv::Point(cvimg.cols, 0));
	polyright.push_back(srcBorders[1]);
	polyright.push_back(srcBorders[2]);
	polyright.push_back(cv::Point(cvimg.cols, cvimg.rows));

	polybottom.push_back(cv::Point(cvimg.cols, cvimg.rows));
	polybottom.push_back(srcBorders[2]);
	polybottom.push_back(srcBorders[3]);
	polybottom.push_back(cv::Point(0, cvimg.rows));

	polyleft.push_back(cv::Point(0, cvimg.rows));
	polyleft.push_back(srcBorders[3]);
	polyleft.push_back(srcBorders[0]);
	polyleft.push_back(cv::Point(0, 0));

	cv::fillConvexPoly(cvimg, polytop, black);
	cv::fillConvexPoly(cvimg, polyright, black);
	cv::fillConvexPoly(cvimg, polybottom, black);
	cv::fillConvexPoly(cvimg, polyleft, black);

	return true;
}