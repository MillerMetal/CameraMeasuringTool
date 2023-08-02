#include "MyController.hpp"
//#include "framework.h"
//#include "SampleMFCOpenCV.h"
//#include "SampleMFCOpenCVDlg.h"
//#include "afxdialogex.h"

//#include <shlwapi.h>
#include <sstream>
#include <string>
#include <format>
#include "json.hpp"

using json = nlohmann::json;

class MyResult {
    public:
        MyResult(std::string wd, std::string ht, std::int32_t nx, std::int32_t ny, std::int32_t nw, std::int32_t nh) {
            width = wd;
            height = ht;
            x = nx;
            y = ny;
            w = nw;
            h = nh;
        }

    public:
        std::string width;
        std::string height;
        std::int32_t x;
        std::int32_t y;
        std::int32_t w;
        std::int32_t h;
    };

    void to_json(json& j, const MyResult& p) {
        j = json{ {"width", p.width}, {"height", p.height}, {"x", p.x}, {"y", p.y}, {"w", p.w}, {"h", p.h} };
}


struct PointSortUpClass {
    bool operator() (cv::Point pt1, cv::Point pt2)
    {
        if (pt1.y == pt2.y)
            return (pt1.x < pt2.x);
        else
            return (pt1.y < pt2.y);
    }
} PointSortUp;

struct PointSortAcrossClass {
    bool operator() (cv::Point pt1, cv::Point pt2)
    {
        if (pt1.x == pt2.x)
            return (pt1.y < pt2.y);
        else
            return (pt1.x < pt2.x);
    }
} PointSortAcross;

struct PointSortDownClass {
    bool operator() (cv::Point pt1, cv::Point pt2) { return (pt1.y > pt2.y); }
} PointSortDown;

struct Point3dSortZUpClass {
    bool operator() (cv::Point3d pt1, cv::Point3d pt2) { return (pt1.z < pt2.z); }
} Point3dSortZUp;

typedef struct LineSegmentStruct {
    cv::Point P1;
    cv::Point P2;
} LineSegment;

bool operator< (LineSegment const& a, LineSegment const& b) {
    return a.P1.y < b.P1.y;
}


class CReturnValue {
public:
    void to_json(json& j, const CReturnValue& p) {
        j = json{ {"width", p.width}, {"height", p.height}, {"x", p.x}, {"y", p.y}, {"w", p.w}, {"h", p.h} };
    }

public:
    std::string width;
    std::string height;
    std::int32_t x;
    std::int32_t y;
    std::int32_t w;
    std::int32_t h;
};


void MyController::DetectEdges(bool bDoFileOpen)
{
    CheckMatricesLoaded();

    szOrgImage = szLastFileName;
    if (szOrgImage.length())
    {
        return;
    }

    string szName = szLastFileName;
    
    Mat cvimgColor = cv::imdecode(cv::Mat(1, m_str.length(), CV_8UC1, (uchar*)m_str.c_str()), IMREAD_COLOR, NULL);
    Mat cvimgColorBgra;  //cvtColor(cvimgColor, cvimgColorBgra, cv::COLOR_BGR2BGRA);
    Mat cvimgOutput = cv::Mat::Mat(cvimgColor.rows, cvimgColor.cols, CV_8UC4);
    Mat cvimgUndistorted; // This is CV_8UC3
    Mat cvimgUndistortedColor;
    Mat cvimgUndistortTemp;
    Mat cvimgTemp;
    Mat cvimgTemp2;
    Mat g1;
    Mat g2;
    Mat thgArr[3];
    Mat thg;
    Mat cvimgUndistortedEmptyPolys;
    Mat timg;
    Mat timgOrg;
    Mat cvimgDewarpedOrg;
    Mat cvimgDewarped;
    Mat cvimgDewarpedSeg;
    Mat cvimgDewarpedShaded;
    Mat cvimgDewarpedThg;
    Mat mask;
    Mat thresh;
    Mat cvimgTestColumns;
    Mat threshDilateErode;
    Mat contours;
    float dMedianHeightF;
    float dMedianWidthF;
    string tstr;
    

    cv::cvtColor(cvimgColor, cvimgColorBgra, COLOR_BGR2BGRA);

    cv::String strBuildInfo = cv::getBuildInformation();


    int ndevcnt = cuda::getCudaEnabledDeviceCount();

    cv::cuda::GpuMat cudaImage(cvimgColorBgra);
    cv::cuda::GpuMat cudaImageOut(cvimgColorBgra);

    long long nSum = SumData(cvimgColorBgra);

    int nNumIterations = m_nNumIterations;
    int nSegmentLength = m_nSegmentLength;
    int nColorRadius = m_nColorRadius;
    int nSpatialRadius = m_nSpatialRadius;

    try
    {
        meanShiftSegmentation(cudaImage, cudaImageOut, nSpatialRadius, nColorRadius, nSegmentLength, cv::TermCriteria((cv::TermCriteria::MAX_ITER), nNumIterations, 1));
    }
    catch (cv::Exception e) 
    {
        string mstr;
        e.formatMessage();
        mstr = e.msg.c_str();

        // Need to throw up an status message here....
        //AfxGetMainWnd()->MessageBox(mstr);
    }

    cudaImageOut.download(cvimgOutput);
    nSum = SumData(cvimgOutput);

    UndistortBigImage(cvimgOutput, cvimgTemp);
    cv::cvtColor(cvimgTemp, cvimgUndistorted, COLOR_BGRA2BGR);

    UndistortBigImage(cvimgColor, cvimgUndistortedColor);

    EraseDrawingBorders(cvimgUndistorted);

    // Difference of gaussians math
    double grad1 = m_dDiffOfGaussRadius1 + 1.0;
    double gsigma1 = sqrt(-(grad1 * grad1) / (2.0 * log10(1.0 / 255.0)));
    double gsigmasq1 = 2 * gsigma1 * gsigma1;
    double gL1 = sqrt(-gsigmasq1 * log10(1.0 / 255.0));
    int n1 = (int)(ceil(gL1) * 2.0);
    int gw1 = (n1 % 2 == 1) ? n1 : n1 + 1;

    double grad2 = m_dDiffOfGaussRadius2 + 1.0;
    double gsigma2 = sqrt(-(grad2 * grad2) / (2.0 * log10(1.0 / 255.0)));
    double gsigmasq2 = 2 * gsigma2 * gsigma2;
    double gL2 = sqrt(-gsigmasq2 * log10(1.0 / 255.0));
    int n2 = (int)(ceil(gL2) * 2.0);
    int gw2 = (n2 % 2 == 1) ? n2 : n2 + 1;


    cv::medianBlur(cvimgUndistorted, cvimgTemp, 7);
    cv::GaussianBlur(cvimgTemp, g1, cv::Size(gw1, gw1), gsigma1);

    //cv::medianBlur(cvimgUndistorted, cvimgTemp, 7);
    cv::GaussianBlur(cvimgTemp, g2, cv::Size(gw2, gw2), gsigma2);

    // Most likely area for the need of debugging.
    cv::subtract(g1, g2, cvimgTemp);

    cv::threshold(cvimgTemp, cvimgTemp2, 0, 16777215, THRESH_BINARY); // last param 0=Binary
    cv::split(cvimgTemp2, thgArr);

    cv::bitwise_or(thgArr[0], thgArr[1], cvimgTemp);
    cv::bitwise_or(cvimgTemp, thgArr[2], cvimgTemp2);
    cvimgTemp2.convertTo(cvimgTemp, CV_16UC1);
    cv::threshold(cvimgTemp, thg, 1, 65535, 0); // last param 0=Binary

    GenerateDetectMaskPolys(cvimgUndistorted.size());

    // The following commands should work.  cvimgUndistorted is a 3 channel,
    // 1 bit arrangement of the pictures
    cvimgUndistorted.copyTo(cvimgUndistortedEmptyPolys);
    DrawRectDetectMask(thg, timg);
    DrawRectDetectMaskPolys(thg, timgOrg);
    cv::bitwise_and(thg, timg, timg);

    nSum = SumData(thg);
    nSum = SumData(timg);
    nSum = SumData(timgOrg);

    float *histIgnoreVals = NULL;

    int nBins = 0;

    CalcEmptyPolys(thg, timg, &histIgnoreVals, nBins);
    DisplayEmptyPolys(timg, histIgnoreVals);
    DisplayEmptyPolys(cvimgUndistortedEmptyPolys, histIgnoreVals);

    cv::warpPerspective(cvimgUndistorted, cvimgDewarpedOrg, homographyMatrix
        , cv::Size((int)((float)m_dResultLength* m_dResultScale), (int)((float)m_dResultWidth* m_dResultScale))
        , INTER_NEAREST, BORDER_DEFAULT, cv::Scalar(0, 0, 0));

    cvimgDewarpedOrg.copyTo(cvimgDewarped);
    DisplayEmptyPolys(cvimgDewarped, histIgnoreVals, true);
    cvimgDewarped.copyTo(cvimgDewarpedSeg);


    cv::warpPerspective(cvimgUndistortedColor, cvimgDewarpedShaded, homographyMatrix
        , cv::Size((int)((float)m_dResultLength* m_dResultScale), (int)((float)m_dResultWidth* m_dResultScale))
        , INTER_CUBIC, BORDER_DEFAULT, cv::Scalar(0, 0, 0));
    cv::warpPerspective(thg, cvimgDewarpedThg, homographyMatrix
        , cv::Size((int)((float)m_dResultLength* m_dResultScale), (int)((float)m_dResultWidth* m_dResultScale))
        , INTER_CUBIC, BORDER_DEFAULT, cv::Scalar(0, 0, 0));

    int nNumFills = 0;

    // m_vecRackPolys is a member value of the MainWindow class.
    // We may need to isolate the major objects before attempting to
    // do our adjustment algorithm.  The adjustment algorithm
    // will look at the 4 quadrants of the image area and
    // see if they can be adjusted individually.  They will also
    // determine if
    if (m_vecRackPolys.size() > 0)
    {
        cv::Point seedpt;
        cv::Point seedptd;
        cv::Size tl[4] = { cv::Size(-1,-1),cv::Size(1,-1),cv::Size(1,1),cv::Size(-1,1) };
        cv::Size tr = cv::Size(1, -1);
        cv::Size br = cv::Size(1, 1);
        cv::Size bl = cv::Size(-1, 1);
        cv::Size masksz = cvimgDewarpedOrg.size() + cv::Size(2, 2);
        mask.create(masksz, CV_8UC1);

        cv::Rect rc;
        int nFillToleranceLow = m_nFillToleranceLow;
        int nFillToleranceHigh = m_nFillToleranceHigh;

        for (int i = 0; i < m_vecRackPolys.size(); i++)
        {
            if (histIgnoreVals[i + 1] == 0)
            {
                std::vector<cv::Point2f> vpf = m_vecRackPolys[i];
                for (int k = 0; k < 4; k++)
                {
                    seedpt = m_vecRackPolys[i][k];
                    seedptd = seedpt + cv::Point(tl[k].width, tl[k].height);

                    // Is the point to be filled already white?  If so, ignore it.
                    uchar Red_val = cvimgDewarpedOrg.at<Vec3b>(seedptd.y, seedptd.x)[2];
                    uchar Green_val = cvimgDewarpedOrg.at<Vec3b>(seedptd.y, seedptd.x)[1];
                    uchar Blue_val = cvimgDewarpedOrg.at<Vec3b>(seedptd.y, seedptd.x)[0];

                    if (Red_val != 255 || Green_val != 255 || Blue_val != 255)
                    {
                        cv::floodFill(cvimgDewarpedOrg
                            , seedptd
                            , cv::Scalar(255, 255, 255)
                            , &rc
                            , cv::Scalar(nFillToleranceLow, nFillToleranceLow, nFillToleranceLow)
                            , cv::Scalar(nFillToleranceHigh, nFillToleranceHigh, nFillToleranceHigh)
                            , 8 + CV_FLOODFILL_FIXED_RANGE);

                        nNumFills++;
                    }
                }
            }
        }

        vector<Mat> splitted_frame;
        cv::split(cvimgDewarpedOrg, splitted_frame);
        for (size_t k = 0; k < splitted_frame.size(); k++)
        {
            threshold(splitted_frame[k], splitted_frame[k], 254, 255, cv::THRESH_BINARY);
            if (k)
                thresh |= splitted_frame[k];
            else
                thresh = splitted_frame[k].clone();
        }

        int nImageIndex = m_nImageIndex;
        cv::Rect2f rectangleF((float)1.0 * (float)m_dResultScale, (float)11.0 * (float)m_dResultScale, (float)1 * (float)m_dResultScale, (float)63.125 * (float)m_dResultScale);
        cv::Rect rectangle = rectangleF;

        cvimgTestColumns.create(cvimgDewarpedThg.size(), CV_8UC3);
        do
        {
            std::vector<cv::Point> pts;

            pts.push_back(rectangle.tl());
            pts.push_back(pts[0] + cv::Point(rectangle.width, 0));
            pts.push_back(pts[1] + cv::Point(0, rectangle.height));
            pts.push_back(pts[2] + cv::Point(-rectangle.width, 0));

            cv::fillConvexPoly(cvimgTestColumns, pts, cv::Scalar(0, 0, 160));
            cv::rectangle(cvimgTestColumns, rectangle, cv::Scalar(0, 0, 255), 1);

            rectangleF += cv::Point2f((float)2.629 * (float)m_dResultScale, 0);
            rectangle = rectangleF;
        } while (rectangle.x < cvimgDewarpedThg.cols);

        cv::dilate(thresh, threshDilateErode, Mat(),cv::Point(-1,-1),10);
        cv::erode(threshDilateErode, threshDilateErode, Mat(), cv::Point(-1, -1), 10);

        std::vector<std::vector<cv::Point>> contours;

        cv::findContours(threshDilateErode, contours, RETR_CCOMP, CHAIN_APPROX_NONE);
        int count = (int)contours.size();
        std::vector<cv::Point> approx;
        LineSegment widthLine;
        LineSegment heightLine;

        std::vector<MyResult> arrReturnValues;

        for (int i = 0; i < count; i++)
        {
            if (contours[i].size() > 150)
            {
                double epsilon = 0.05 * cv::arcLength(contours[i], true);
                cv::approxPolyDP(contours[i], approx, epsilon, true);

                double dArea = cv::contourArea(contours[i], false);

                if (approx.size() == 4 && dArea > 40000)
                {
                    cv::Point center;
                    cv::Size size;
                    string tstr;
                    int nBaseline;
                    cv::RotatedRect rotrc = cv::minAreaRect(contours[i]);
                    cv::Point2f rotptsf[4];

                    std::vector<cv::Point> vertices;
                    rotrc.points(rotptsf);
                    vertices.push_back(rotptsf[0]);
                    vertices.push_back(rotptsf[1]);
                    vertices.push_back(rotptsf[2]);
                    vertices.push_back(rotptsf[3]);

                    cv::polylines(cvimgDewarpedOrg, contours[i], true, cv::Scalar(0, 255, 255), 3);
                    cv::polylines(cvimgDewarpedOrg, vertices, true, cv::Scalar(0, 255, 0), 5);

                    std::vector<cv::Point3d> pmeasurelist;
                    std::vector<cv::Point> plist;

                    plist.insert(plist.end(), contours[i].begin(), contours[i].end());
                    std::sort(plist.begin(), plist.end(), PointSortUp);

                    std::vector<cv::Point> psortedlist(plist);
                    dMedianHeightF = dMedianWidthF = 0;

                    int nelem = (int)psortedlist.size();
                    if (nelem > 0)
                    {
                        vector<int> widths;
                        int nCurX = psortedlist[0].x;
                        int nCurY = psortedlist[0].y;
                        int nFirstX = nCurX;
                        int nWidth = 0;

                        for (int k = 0; k < psortedlist.size(); k++)
                        {
                            if (nCurY != psortedlist[k].y)
                            {
                                pmeasurelist.push_back(cv::Point3d(nCurX, nCurY, nCurX - nFirstX));
                                widths.push_back(nCurX - nFirstX);
                                nCurY = psortedlist[k].y;
                                nCurX = psortedlist[k].x;
                                nFirstX = nCurX;
                            }
                            else
                                nCurX = psortedlist[k].x;
                        }
                        widths.push_back(nCurX - nFirstX);
                        pmeasurelist.push_back(cv::Point3d(nCurX, nCurY, nCurX - nFirstX));
                        std::sort(pmeasurelist.begin(), pmeasurelist.end(), Point3dSortZUp);

                        cv::Point3d pLeft = pmeasurelist[pmeasurelist.size() / 2];
                        double nMedianWidth = pLeft.z;
                        dMedianWidthF = (float)((double)nMedianWidth / this->m_dResultScale);
                        widthLine.P1 = cv::Point((int)pLeft.x, (int)pLeft.y);
                        widthLine.P2 = cv::Point((int)(pLeft.x - pLeft.z), (int)pLeft.y);
                    }


                    std::sort(plist.begin(), plist.end(), PointSortAcross);
                    psortedlist.assign(plist.begin(), plist.end());
                    pmeasurelist.clear();

                    nelem = (int)psortedlist.size();
                    if (nelem > 0)
                    {
                        std::vector<int> heights;
                        int nCurX = psortedlist[0].x;
                        int nCurY = psortedlist[0].y;
                        int nFirstY = nCurY;

                        for (int k = 0; k < psortedlist.size(); k++)
                        {
                            if (nCurX != psortedlist[k].x)
                            {
                                // are we on a new line?
                                pmeasurelist.push_back(cv::Point3d(nCurX, nCurY, nCurY - nFirstY));
                                heights.push_back(nCurY - nFirstY);
                                nCurY = psortedlist[k].y;
                                nCurX = psortedlist[k].x;
                                nFirstY = nCurY;
                            }
                            else
                                nCurY = psortedlist[k].y;
                        }
                        heights.push_back(nCurY - nFirstY);
                        pmeasurelist.push_back(cv::Point3d(nCurX, nCurY, nCurY - nFirstY));
                        std::sort(pmeasurelist.begin(), pmeasurelist.end(), Point3dSortZUp);

                        Point3d pLeft(pmeasurelist[pmeasurelist.size() / 2]);
                        double nMedianHeight = pLeft.z;
                        dMedianHeightF = (float)nMedianHeight / (float)m_dResultScale;
                        heightLine.P1 = cv::Point((int)pLeft.x, (int)pLeft.y);
                        heightLine.P2 = cv::Point((int)(pLeft.x), (int)(pLeft.y - pLeft.z));
                    }

                    center = approx[0];
                    center += approx[1];
                    center += approx[2];
                    center += approx[3];
                    center.x /= 4;
                    center.y /= 4;

                    stringstream ss;
                    stringstream ssw;
                    stringstream ssh;
                    string strToken;

                    ss << "W=" << setprecision(2) << fixed << dMedianWidthF << " \n" << "H=" << setprecision(2) << fixed << dMedianHeightF;
                    ssw << setprecision(2) << fixed << dMedianWidthF;
                    ssh << setprecision(2) << fixed << dMedianHeightF;

                    arrReturnValues.push_back(MyResult(
                          ssw.str()
                        , ssh.str()
                        , widthLine.P1.x
                        , heightLine.P1.y
                        , widthLine.P1.x - widthLine.P2.x
                        , heightLine.P1.y - heightLine.P2.y
                        ));

                    nBaseline = 0;
                    int nTokenPos = 0;

                    getline(ss, strToken, '\n');
                    cv::Size sz = cv::getTextSize(cv::String(strToken), FONT_HERSHEY_PLAIN, 6, 3, &nBaseline);
                    size = cv::Size() - sz;
                    size.width /= 2;

                    cv::arrowedLine(cvimgDewarpedOrg, heightLine.P1, heightLine.P2, cv::Scalar(0, 0, 255), 3);
                    cv::arrowedLine(cvimgDewarpedOrg, widthLine.P1, widthLine.P2, cv::Scalar(0, 0, 255), 3);

                    nTokenPos = 0;
                    ss.seekg(0, std::ios_base::beg);

                    while (getline(ss, strToken, '\n')) {
                        cv::putText(cvimgDewarpedOrg
                            , cv::String(strToken)
                            , center + cv::Point(size.width, size.height)
                            , FONT_HERSHEY_PLAIN, 6, cv::Scalar(0, 0, 0), 3);

                        center.y += 100;
                    }

                }
                else
                if (dArea > 40000)
                {
                    cv::RotatedRect rcRotated = cv::minAreaRect(contours[i]);
                    vector<cv::Point> vertices;
                    cv::Point2f rotptsf[4];

                    rcRotated.points(rotptsf);
                    vertices.push_back(rotptsf[0]);
                    vertices.push_back(rotptsf[1]);
                    vertices.push_back(rotptsf[2]);
                    vertices.push_back(rotptsf[3]);

                    cv::polylines(cvimgDewarpedOrg, contours[i], true, cv::Scalar(0, 0, 255), 3);
                    cv::polylines(cvimgDewarpedOrg, vertices, true, cv::Scalar(0, 255, 0), 5);
                }
            }

        }
         
        switch (nImageIndex)
        {
        case 0:  thresh.copyTo(TheImage); break;
        case 1:  cvimgDewarpedOrg.copyTo(TheImage); break;
        default:
            break;
        }


        json jmeas;
        int i;

        jmeas = arrReturnValues;

        m_szMeasurements = jmeas.dump(4);
    }
}
