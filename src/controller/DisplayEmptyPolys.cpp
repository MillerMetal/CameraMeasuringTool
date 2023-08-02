#include "MyController.hpp"

//#include "framework.h"
//#include "SampleMFCOpenCV.h"
//#include "SampleMFCOpenCVDlg.h"
//#include "afxdialogex.h"

//#include <shlwapi.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


void MyController::DisplayEmptyPolys(cv::Mat& img, float* histIgnoreVals, bool bUseRealRack)
{
	int nFillVal = (int) m_vecMaskPolys.size();
	int nNumChannels = img.channels();
	Mat rval;

	cv::Scalar white(255, 255, 255, 255);
	if (nNumChannels == 1) {
		if (img.depth() == CV_8UC1)
			white = cv::Scalar(nFillVal);
		else
			white = cv::Scalar(255);
	}


	if (bUseRealRack) 
	{
		//std::stringstream ss;
		for (int i = 0; i < m_vecRackPolys.size(); i++) 
		{
			if (histIgnoreVals[i + 1] == 0) 
			{
				std::vector<cv::Point2f> vpf = m_vecRackPolys[i];
				std::vector<cv::Point> vpi = std::vector<cv::Point>(vpf.begin(), vpf.end());
				//CString tstr;

				//ss.clear();
				//ss << vpf << "\0";
				//tstr = ss.str().c_str();

				//nval = vpm.checkVector(2, CV_32S);
				cv::fillConvexPoly(img, vpi, white);
			}
		}
	}
	else
	{
		for (int i = 0; i < m_vecMaskPolys.size(); i++)
		{
			if (histIgnoreVals[i + 1] == 0)
			{
				std::vector<cv::Point2f> vpf = m_vecRackPolys[i];
				std::vector<cv::Point> vp;

				cv::Point2f* pfa = m_vecRackPolys[i].data();
				int pfasz = (int)m_vecRackPolys[i].size();

				// The conversion rounds to nearest integer, not truncate...
				std::vector<cv::Point2f> srcBorders;
				cv::fillConvexPoly(img, m_vecMaskPolys[i], white);
			}
		}
	}
}
