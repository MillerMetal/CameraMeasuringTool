#include "pch.h"
#include "MyController.hpp"
//#include "framework.h"
//#include "SampleMFCOpenCV.h"
//#include "SampleMFCOpenCVDlg.h"
//#include "afxdialogex.h"

//#include <shlwapi.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

bool MyController::GetDefaultBorders(std::vector<cv::Point2f>& defborder)
{
	ReadDefaults(false);

	defborder.clear();

	defborder.push_back(cv::Point2f((float)(m_dBorderLeft * m_dResultScale), (float)(m_dBorderTop * m_dResultScale)));
	defborder.push_back(cv::Point2f((float)((m_dResultLength - m_dBorderRight) * m_dResultScale), (float)(m_dBorderTop * m_dResultScale)));
	defborder.push_back(cv::Point2f((float)((m_dResultLength - m_dBorderRight) * m_dResultScale), (float)((m_dResultWidth - m_dBorderBottom) * m_dResultScale)));
	defborder.push_back(cv::Point2f((float)(m_dBorderLeft * m_dResultScale), (float)((m_dResultWidth - m_dBorderBottom) * m_dResultScale)));

	return true;
}