#include "MyController.hpp"
//#include "framework.h"
//#include "SampleMFCOpenCV.h"
//#include "SampleMFCOpenCVDlg.h"
//#include "afxdialogex.h"

//#include <shlwapi.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

bool MyController::DrawRectDetectMaskPolys(cv::Mat& img, cv::Mat& rval)
{
	std::vector<cv::Point2f> dstBorders;
	Mat rvalLocal;
	float dScaleFactor = (float)m_dResultScale;

	rval.create(cv::Size(img.cols, img.rows), CV_16UC1);
	rval.setTo(cv::Scalar(0, 0, 0));

	GetDefaultBorders(dstBorders);

	for (int i = 0; i < m_vecMaskPolys.size(); i++)
		cv::polylines(rval, m_vecMaskPolys[i], true, cv::Scalar(65535));

	return true;
}