#include "MyController.hpp"
//#include "framework.h"
//#include "SampleMFCOpenCV.h"
//#include "SampleMFCOpenCVDlg.h"
//#include "afxdialogex.h"

//#include <shlwapi.h>

bool MyController::CalcEmptyPolys(cv::Mat& imgSrc, cv::Mat& imgResult, float** histIgnoreValsResult, int& nBins)
{
    int x, y;
    int i;
    Mat mask;
    int n;
    string tstr;
    int ix, iy;
    int histSize = 65536;
    float range[] = { 0, 65536.0 };
    const float* histRange[] = { range };
    cv::Mat Histo;
    vector<cv::Point> vop;
    vector<cv::Point> vopr;
    vector<unsigned long> voi;
    vector<unsigned long> voir;
    //CArray<cv::Point, cv::Point&> vop;
    //CArray<cv::Point, cv::Point&> vopr;
    //CDWordArray voi;
    //CDWordArray voir;
    cv::Point pt;

    DrawRectDetectMask(imgSrc, imgResult);
    cv::bitwise_and(imgSrc, imgResult, imgResult);

    cv::calcHist(&imgResult, 1, 0, Mat(), Histo, 1, &histSize, histRange);
    if (*histIgnoreValsResult != 0) 
    {
        delete *histIgnoreValsResult;
        *histIgnoreValsResult = NULL;
    }

    *histIgnoreValsResult = new float[Histo.rows];
    memcpy(*histIgnoreValsResult, (float*)Histo.data, sizeof(float)*Histo.rows);
    m_matFillPoints.setTo(cv::Scalar::all(0));

    i = 1;
    for (y = 0; y < m_matFillPoints.rows; y++)
    {
        for (x = 0; x < m_matFillPoints.cols; x++)
        {
            float& gray = m_matFillPoints.at<float>(y, x);
            gray = ((*histIgnoreValsResult)[i] == 0) ? (float)32767 : (float)0;
            i++;
        }
    }

    i = 1;
    for (pt.y = 0; pt.y < m_matFillPoints.rows; pt.y++)
    {
        for (pt.x = 0; pt.x < m_matFillPoints.cols; pt.x++)
        {
            float& gray = m_matFillPoints.at<float>(pt.y, pt.x);

            if (gray > 0)
            {
                voi.push_back(cv::floodFill( m_matFillPoints, pt, cv::Scalar((double)gray), 0, cv::Scalar(), cv::Scalar(), 8 + CV_FLOODFILL_FIXED_RANGE));
                vop.push_back(pt);
                i++;
            }
        }
    }

    for (i = 0; i < voi.size(); i++)
    {
        if (voi[i] <= 2)
        {
            voir.push_back(cv::floodFill(m_matFillPoints, vop[i], cv::Scalar(), 0, cv::Scalar(), cv::Scalar(), 8 + CV_FLOODFILL_FIXED_RANGE));
            vopr.push_back(vop[i]);
        }
    }

    i = 1;
    for (iy = 0; iy < m_matFillPoints.rows; iy++)
    {
        for (ix = 0; ix < m_matFillPoints.cols; ix++)
        {
            n = (int&) m_matFillPoints.at<float>(iy, ix);
            if ((n == 0) && ((*histIgnoreValsResult)[i] == 0))
                (*histIgnoreValsResult)[i] = 1.0F;
            i++;
        }
    }

	return true;
}