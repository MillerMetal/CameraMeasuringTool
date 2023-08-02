#include "MyController.hpp"
//#include "framework.h"
//#include "SampleMFCOpenCV.h"
//#include "SampleMFCOpenCVDlg.h"
//#include "afxdialogex.h"

//#include <shlwapi.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

bool MyController::LoadParams(Object<ImageDto> dto)
{
	auto s = oatpp::encoding::Base64::decode(dto->image);
	m_str = s.getValue("null");

	FileStorage fs;
	std::string tstr = dto->szCameraMatrix;
	if (tstr.length() > 0)
	{
		fs.open(tstr, FileStorage::READ | FileStorage::MEMORY, cv::String("UTF-8"));
		fs["Camera"] >> cameraMatrix;
		fs.release();
	}

	tstr = dto->szHomographyMatrix;
	if (tstr.length() > 0)
	{
		fs.open(tstr, FileStorage::READ | FileStorage::MEMORY, cv::String("UTF-8"));
		fs["Homography"] >> homographyMatrix;
		fs.release();
	}

	tstr = dto->szDistortionMatrix;
	if (tstr.length() > 0)
	{
		fs.open(tstr, FileStorage::READ | FileStorage::MEMORY, cv::String("UTF-8"));
		fs["Distortion"] >> distortionMatrix;
		fs.release();
	}

	if (homographyMatrix.rows > 0)
		invert(homographyMatrix, invHomographyMatrix);
	else
		invHomographyMatrix.create(3, 3, CV_64F);


	if (cameraMatrix.rows == 0 || cameraMatrix.cols == 0)
		cameraMatrix.create(3, 3, CV_64F);
	if (homographyMatrix.rows == 0 || homographyMatrix.cols == 0)
		homographyMatrix.create(3, 3, CV_64F);
	if (distortionMatrix.rows == 0 || distortionMatrix.cols == 0)
		distortionMatrix.create(5, 1, CV_64F);

	m_dDiffOfGaussRadius1 = dto->dDiffOfGaussRadius1;
	m_dDiffOfGaussSigma1 = dto->dDiffOfGaussSigma1;
	m_dDiffOfGaussRadius2 = dto->dDiffOfGaussRadius2;
	m_dDiffOfGaussSigma2 = dto->dDiffOfGaussSigma2;

	m_dResultScale = dto->dResultScale;
	m_dResultLength = dto->dResultLength;
	m_dResultWidth = dto->dResultWidth;
	m_dBorderBottom = dto->dBorderBottom;
	m_dBorderTop = dto->dBorderTop;
	m_dBorderLeft = dto->dBorderLeft;
	m_dBorderRight = dto->dBorderRight;

	m_nFillToleranceLow = dto->nFillToleranceLow;
	m_nFillToleranceHigh = dto->nFillToleranceHigh;
	m_nImageIndex = dto->nImageIndex;
	m_nNumIterations = dto->nMSSNumIterations;
	m_nSegmentLength = dto->nMSSSegmentLength;
	m_nColorRadius = dto->nMSSColorRadius;
	m_nSpatialRadius = dto->nMSSSpatialRadius;

	return true;
}