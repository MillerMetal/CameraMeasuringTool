#include "MyController.hpp"
//#include "framework.h"
//#include "SampleMFCOpenCV.h"
//#include "SampleMFCOpenCVDlg.h"
//#include "afxdialogex.h"

//#include <shlwapi.h>

#include <opencv2\calib3d.hpp>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace cv;
using namespace cv::cuda;

bool MyController::UndistortBigImage(
          cv::Mat &cvimgBefore
        , cv::Mat &cvimgUndistorted
        )
{
    cv::Size orgsz(cvimgBefore.size());
    cv::Size newsz(orgsz.width * 3 / 2, orgsz.height * 3 / 2);

    // Might be a little hokey
    Mat cvimgBigColor(newsz, cvimgBefore.type() );
    cvimgBigColor.setTo(cv::Scalar(0,0,0));

    // Do it!
    long nDiffHeight = newsz.height - orgsz.height;
    long nDiffWidth = newsz.width - orgsz.width;
    cvimgBigColor.adjustROI(0,-nDiffHeight,0,-nDiffWidth);
    cvimgBefore.copyTo(cvimgBigColor);
    cvimgBigColor.adjustROI(0, nDiffHeight, 0, nDiffWidth);

    cv::Rect validROI;

    newCameraMat = cv::getOptimalNewCameraMatrix(cameraMatrix, distortionMatrix, orgsz, 1, newsz, &validROI);
    cvimgUndistorted.create(newsz, cvimgBefore.type());

    cv::undistort(cvimgBigColor, cvimgUndistorted, cameraMatrix, distortionMatrix, newCameraMat);

    // Search the four sides for where the black stops
    // We loop in a line from the center of each side until
    // we see a pixel channel value that is nonzero.
    int nLeft, nRight, nTop, nBottom, nTempY, nTempX;

    nTempY = cvimgUndistorted.rows/ 2;

    int nPixelType = cvimgUndistorted.type();

    uchar* rowPtr = cvimgUndistorted.ptr(nTempY);
    int channels = cvimgUndistorted.channels();
    int num_rows = cvimgUndistorted.rows;
    int num_cols = (cvimgUndistorted.cols * channels);
    int j;
    nTempX = cvimgUndistorted.cols / 2 * channels;

    for (nLeft = 0; nLeft < num_cols; nLeft+=channels)
    {
        // Go through all the channels.  If there's a 
        // nonzero channel, we'll break before j gets
        // to the number of channels.
        for (j = 0; j < channels; j++)
            if (rowPtr[nLeft + j] != 0) break;
        // If we did a break before we got to the number
        // of channels, we can stop the outer loop.
        if (j < channels) break;
    }
    if (nLeft > num_cols-channels)
        nLeft = num_cols-channels;
    nLeft /= channels;

    for (nRight = num_cols - channels; nRight >= 0; nRight-=channels)
    {
        // Go through all the channels.  If there's a 
        // nonzero channel, we'll break before j gets
        // to the number of channels.
        for (j = 0; j < channels; j++)
            if (rowPtr[nRight + j] != 0) break;
        // If we did a break before we got to the number
        // of channels, we can stop the outer loop.
        if (j < channels) break;
    }
    if (nRight < 0) 
        nRight = 0;
    else
    {
        nRight = num_cols - nRight;
        nRight /= channels;
    }

    for (nTop = 0; nTop < num_rows; nTop++)
    {
        rowPtr = cvimgUndistorted.ptr(nTop);
        for (j=0; j<channels; j++)
            if (rowPtr[nTempX+j]) break;
        if (j < channels)
            break;
    }
    if (nTop >= num_rows)
        nTop = num_rows - 1;
    
    for (nBottom = num_rows - 1; nBottom >= 0; nBottom--)
    {
        rowPtr = cvimgUndistorted.ptr(nBottom);
        for (j = 0; j < channels; j++)
            if (rowPtr[nTempX+j]) break;
        if (j < channels)
            break;
    }
    if (nBottom < 0)
        nBottom = 0;
    else
        nBottom = num_rows - nBottom;


    //cv::Size roisz(nRight-nLeft,nBottom-nTop);
    //cv::Point roipt(nLeft,nTop);
    //cvimgUndistorted.locateROI(roisz, roipt);
    cvimgUndistorted.adjustROI(-nTop, -nBottom, -nLeft, -nRight);

    return true;
}

/*

public Image<TColor, TDepth> UndistortBigImage<TColor, TDepth>(Image<TColor, TDepth> cvimgBefore)
where TColor : struct, Emgu.CV.IColor
where TDepth : new()
{
X    System.Drawing.Size orgImageSize = new System.Drawing.Size(cvimgBefore.Size.Width, cvimgBefore.Size.Height);

X    System.Drawing.Size newImageSize = new System.Drawing.Size(cvimgBefore.Size.Width * 3 / 2, cvimgBefore.Size.Height * 3 / 2);
X    Emgu.CV.Image<TColor, TDepth> cvimgBigColor = new Image<TColor, TDepth>(newImageSize.Width, newImageSize.Height);

X    cvimgBigColor.ROI = new System.Drawing.Rectangle(0, 0, orgImageSize.Width, orgImageSize.Height);
x    cvimgBefore.CopyTo(cvimgBigColor);
x    cvimgBigColor.ROI = new System.Drawing.Rectangle(0, 0, newImageSize.Width, newImageSize.Height);

x    System.Drawing.Rectangle validROI = new Rectangle();

x    Emgu.CV.Mat newCameraMat = CvInvoke.GetOptimalNewCameraMatrix(cameraMatrix.Mat
                                                   , distortionMatrix.Mat
                                                   , orgImageSize
                                                   , 1
                                                   , newImageSize
                                                   , ref validROI);

x    Emgu.CV.Image<TColor, TDepth> cvimgUndistorted = new Emgu.CV.Image<TColor, TDepth>(newImageSize.Width, newImageSize.Height);

    // Undistort the image using the stored camera parameters
x    CvInvoke.Undistort(cvimgBigColor, cvimgUndistorted, cameraMatrix, distortionMatrix, newCameraMat);

    // Search the four sides for where the black stops
    int nLeft, nRight, nTop, nBottom, nTempY, nTempX;
    Object pixelZeroColor;

    if (typeof(TColor) == typeof(Bgr))
    {
        pixelZeroColor = new Bgr(0, 0, 0);
    }
    else
    if (typeof(TColor) == typeof(Bgra))
    {
        pixelZeroColor = new Bgra(0, 0, 0, 0);
    }
    else
    {
        pixelZeroColor = new Gray(0);
    }

    nTempY = cvimgUndistorted.Height / 2;
    nTempX = cvimgUndistorted.Width / 2;

    for (nLeft = 0; nLeft < cvimgUndistorted.Width; nLeft++)
    {
        if (cvimgUndistorted[nTempY, nLeft].Equals(pixelZeroColor) == false)
            break;
    }

    for (nRight = cvimgUndistorted.Width - 1; nRight >= 0; nRight--)
    {
        if (cvimgUndistorted[nTempY, nRight].Equals(pixelZeroColor) == false)
            break;
    }

    for (nTop = 0; nTop < cvimgUndistorted.Height; nTop++)
    {
        if (cvimgUndistorted[nTop, nTempX].Equals(pixelZeroColor) == false)
            break;
    }

    for (nBottom = cvimgUndistorted.Height - 1; nBottom >= 0; nBottom--)
    {
        if (cvimgUndistorted[nBottom, nTempX].Equals(pixelZeroColor) == false)
            break;
    }

    nTempY = 0;

    cvimgUndistorted.ROI = new Rectangle(nLeft, nTop, nRight - nLeft, nBottom - nTop);

    return cvimgUndistorted;
}
*/