#include "MyController.hpp"
//#include "framework.h"
//#include "SampleMFCOpenCV.h"
//#include "SampleMFCOpenCVDlg.h"
//#include "afxdialogex.h"

//#include <shlwapi.h>

void findAndReplaceAll(std::string& data, std::string toSearch, std::string replaceStr)
{
    // Get the first occurrence
    size_t pos = data.find(toSearch);
    // Repeat till end is reached
    while (pos != std::string::npos)
    {
        // Replace this occurrence of Sub String
        data.replace(pos, toSearch.size(), replaceStr);
        // Get the next occurrence from the current position
        pos = data.find(toSearch, pos + replaceStr.size());
    }
}

bool MyController::GenerateDetectMaskPolys(cv::Size sz) 
{
	vector<cv::Point2f> dstBorders;
    float dScaleFactor = (float)m_dResultScale;
    float x, y, x2, y2;
    float xinc, yinc;
    ushort nPolyCnt;
    vector<cv::Point> tpoly; // = new VectorOfPoint(4);
    Mat2f srcPoly;
    vector<cv::Point> dSrcPoly;
    vector<cv::Point> dDestPoly;
    stringstream ssRack;
    stringstream ssMask;

	GetDefaultBorders(dstBorders);
    dDestPoly.reserve(4);

    m_vecRackPolys.clear();
    m_vecMaskPolys.clear();

    xinc = 3.0F;  // increment of 2.5"
    yinc = 1.5F;  // increment of 2.5"

    nPolyCnt = 1;

    // Figure out how many neighbors connected points
    // have.  We use the flood fill method to join points.  
    // Of course, this is not optimized for video purposes.
    int tw = (int)floor(dstBorders[1].x - dstBorders[0].x);
    int txinc = (int)floor(dScaleFactor * xinc);
    int w = tw / txinc + (((tw % txinc) > 0) ? 1 : 0);

    int th = (int)floor(dstBorders[3].y - dstBorders[0].y);
    int tyinc = (int)floor(dScaleFactor * yinc);
    int h = th / tyinc + (((th % tyinc) > 0) ? 1 : 0);

    m_matFillPoints.create(h, w, CV_32FC1);

    srcPoly.create(1,4);

    // Loop around to get the polygon borders.
    // This will need some testing with debug mode because
    // we're not sure how the vector stuff works, whether it's
    // really copying the data or whatever else it's doing.
    for (y = dstBorders[0].y; y < dstBorders[3].y; y += dScaleFactor * yinc)
    {
        for (x = dstBorders[0].x; x < dstBorders[1].x; x += dScaleFactor * xinc)
        {
            x2 = x + dScaleFactor * xinc;
            if (x2 > dstBorders[1].x)
                x2 = dstBorders[1].x;

            y2 = y + dScaleFactor * yinc;
            if (y2 > dstBorders[3].y)
                y2 = dstBorders[3].y;

            srcPoly.at<cv::Point2f>(0,0)  = cv::Point2f(x,y);
            srcPoly.at<cv::Point2f>(0, 1) = cv::Point2f(x2-1, y);
            srcPoly.at<cv::Point2f>(0, 2) = cv::Point2f(x2-1, y2-1);
            srcPoly.at<cv::Point2f>(0, 3) = cv::Point2f(x, y2-1);

            dSrcPoly.clear();
            dSrcPoly.push_back(cv::Point2f(x, y));
            dSrcPoly.push_back(cv::Point2f(x2 - 1, y));
            dSrcPoly.push_back(cv::Point2f(x2 - 1, y2 - 1));
            dSrcPoly.push_back(cv::Point2f(x, y2 - 1));

            ssRack << dSrcPoly << ",\n";

            //ssRack << "{" << srcPoly.at<cv::Point2f>(0, 0) << ",";
            //ssRack << srcPoly.at<cv::Point2f>(0, 1) << ",";
            //ssRack << srcPoly.at<cv::Point2f>(0, 2) << ",";
            //ssRack << srcPoly.at<cv::Point2f>(0, 3) << "},\n";

            m_vecRackPolys.push_back(srcPoly);
            
            // push_back(const std::vector<cv::Point2f> &Val)
            // Transform and convert to ints
            Mat dpoly;

            cv::perspectiveTransform(srcPoly, dpoly, invHomographyMatrix);

            dDestPoly.clear();
            for (int i = 0; i < 4; i++)
            {
                dDestPoly.push_back(dpoly.at<cv::Point2f>(0,i));
            }

            ssMask << dDestPoly << ",\n";
            //ssMask << "{" << dpoly.at<cv::Point2f>(0,0) << ",";
            //ssMask << dpoly.at<cv::Point2f>(0, 1) << ",";
            //ssMask << dpoly.at<cv::Point2f>(0, 2) << ",";
            //ssMask << dpoly.at<cv::Point2f>(0, 3) << "},\n";

            m_vecMaskPolys.push_back(  dDestPoly);
        }
    }

    string tstr(ssRack.str().c_str());
    string mstr(ssMask.str().c_str());

    findAndReplaceAll(tstr, "\n", " ");
    findAndReplaceAll(tstr, "]", "]\n");
    findAndReplaceAll(mstr, "\n", " ");
    findAndReplaceAll(mstr, "]", "]\n");

    return true;
}