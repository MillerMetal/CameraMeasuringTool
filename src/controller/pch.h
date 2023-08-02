// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

// add headers that you want to pre-compile here
//#include "framework.h"
//#include "Resource.h"
#include <math.h>

#define IMAGE_WIDTH 128
#define IMAGE_HEIGHT 128
#define IMAGE_CHANNELS 3

#ifndef _OPENCV_INCLUDES
#define _OPENCV_INCLUDES
#include "opencv2\core.hpp"
#include "opencv2\highgui.hpp"
#include "opencv2\imgcodecs.hpp"
#include "opencv2\imgproc.hpp"
#include "opencv2\cudaimgproc.hpp"
#include "opencv2\xfeatures2d\cuda.hpp"
#include "opencv2\core\types_c.h"
#include "opencv2\calib3d.hpp"
#include "opencv2/objdetect/aruco_detector.hpp"
#include "opencv2\core\cuda.hpp"
#endif

using namespace std;
using namespace cv;
using namespace cv::cuda;

#define CV_FLOODFILL_FIXED_RANGE (1 << 16)


#endif //PCH_H
