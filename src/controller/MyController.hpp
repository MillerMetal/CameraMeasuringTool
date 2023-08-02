#ifndef MyController_hpp
#define MyController_hpp

#include "pch.h"
#include "dto/DTOs.hpp"
#include "oatpp/web/mime/multipart/InMemoryDataProvider.hpp"
#include "oatpp/web/mime/multipart/Reader.hpp"
#include "oatpp/web/mime/multipart/PartList.hpp"
#include "oatpp/encoding/Base64.hpp"
//#include "../build/base64.hpp"
#include "oatpp/core/data/resource/File.hpp"
#include <istream>

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController) //<-- Begin Codegen

namespace multipart = oatpp::web::mime::multipart;

/**
 * Sample Api Controller.
 */
class MyController : public oatpp::web::server::api::ApiController {
public:
  /**
   * Constructor with object mapper.
   * @param objectMapper - default object mapper used to serialize/deserialize DTOs.
   */
  MyController(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
    : oatpp::web::server::api::ApiController(objectMapper)
  {}
public:

	ENDPOINT_INFO(ProcessPicture) {
		info->summary = "ProcessPicture endpoint with 'Picture Processed' message";
		info->addResponse<Object<ImageDto>>(Status::CODE_200,"application/json");
	};

    //ENDPOINT("POST", "/ProcessPicture", ProcessPicture, BODY_STRING(String, body)) 
	//ENDPOINT("POST", "/ProcessPicture", ProcessPicture, BODY_DTO(Object<ImageDto>, dto))
	//{
	//	// Make a fake reply using a different image
	//	auto dtoResponse = ImageDto::createShared();

	//	auto s = oatpp::String::loadFromFile("c:\\users\\g.norkus\\Pictures\\testout.jpg");
	//	s.saveToFile("c:\\users\\g.norkus\\Pictures\\test1.bin");
	//	oatpp::String sb64 = oatpp::encoding::Base64::encode(s);

	//	dtoResponse->image = sb64 ;
	//	dtoResponse->description = "We processed this sample";
	//	dtoResponse->fullfilename = "c:\\users\\g.norkus\\testout.jpg";

	//	return createDtoResponse(Status::CODE_200, dtoResponse);
 //   };


	ENDPOINT("POST", "/ProcessPicture", ProcessPicture, BODY_DTO(Object<ImageDto>, dtoIncoming))
	{
		auto dtoResponse = ImageDto::createShared();

		LoadParams(dtoIncoming);

		// Get the image that was sent to us
		//auto s = oatpp::encoding::Base64::decode(dtoIncoming->image);

		DetectEdges(false);

		std::vector<uchar> retbuf;

		imencode(".png", TheImage, retbuf);
		oatpp::String sb64 = oatpp::encoding::Base64::encode(oatpp::String((const char*)retbuf.data(), retbuf.size()));

		dtoResponse->image = sb64;
		dtoResponse->description = "We processed this sample";
		dtoResponse->fullfilename = "testout.png";
		dtoResponse->szURL = dtoIncoming->szURL;
		dtoResponse->dDiffOfGaussRadius1 = dtoIncoming->dDiffOfGaussRadius1;
		dtoResponse->dDiffOfGaussSigma1 = dtoIncoming->dDiffOfGaussSigma1;
		dtoResponse->dDiffOfGaussRadius2 = dtoIncoming->dDiffOfGaussRadius2;
		dtoResponse->dDiffOfGaussSigma2 = dtoIncoming->dDiffOfGaussSigma2;
		dtoResponse->dResultScale = dtoIncoming->dResultScale;
		dtoResponse->dResultLength = dtoIncoming->dResultLength;
		dtoResponse->dResultWidth = dtoIncoming->dResultWidth;
		dtoResponse->dBorderBottom = dtoIncoming->dBorderBottom;
		dtoResponse->dBorderTop = dtoIncoming->dBorderTop;
		dtoResponse->dBorderLeft = dtoIncoming->dBorderLeft;
		dtoResponse->dBorderRight = dtoIncoming->dBorderRight;
		dtoResponse->szCameraMatrix = dtoIncoming->szCameraMatrix;
		dtoResponse->szDistortionMatrix = dtoIncoming->szDistortionMatrix;
		dtoResponse->szHomographyMatrix = dtoIncoming->szHomographyMatrix;
		dtoResponse->nFillToleranceLow = dtoIncoming->nFillToleranceLow;
		dtoResponse->nFillToleranceHigh = dtoIncoming->nFillToleranceHigh;
		dtoResponse->nImageIndex = dtoIncoming->nImageIndex;
		dtoResponse->nMSSNumIterations = dtoIncoming->nMSSNumIterations;
		dtoResponse->nMSSSegmentLength = dtoIncoming->nMSSSegmentLength;
		dtoResponse->nMSSColorRadius = dtoIncoming->nMSSColorRadius;
		dtoResponse->nMSSSpatialRadius = dtoIncoming->nMSSSpatialRadius;
		dtoResponse->szMeasurements = m_szMeasurements;

		return createDtoResponse(Status::CODE_200, dtoResponse);
	};


 
public:
	string szOrgImage;
	string szLastFileName;
	string m_strIncoming;

	bool UndistortBigImage(cv::Mat& cvimgBefore, cv::Mat& cvimgUndistorted);
	bool CheckMatricesLoaded(void) { return true; }
	//void DisplayMyData();
	bool EraseDrawingBorders(cv::Mat& cvimg);
	bool GetDefaultBorders(std::vector<cv::Point2f>& defborder);
	bool GenerateDetectMaskPolys(cv::Size sz);
	bool DrawRectDetectMaskPolys(cv::Mat& img, cv::Mat& rval);
	bool DrawRectDetectMask(cv::Mat& img, cv::Mat& rval);
	bool CalcEmptyPolys(cv::Mat& imgSrc, cv::Mat& imgResult, float** histIgnoreValsResult, int& nBins);
	void DisplayEmptyPolys(cv::Mat& img, float* histIgnoreVals, bool bUseRealRack = false);
	void ReadDefaults(bool bInit = true) {}
	void SaveDefaults() {}
	void DetectEdges(bool bDoFileOpen);
	//void DetectCorners(string& szFileName);
	void DetectCorners(InputArray buf);
	void LoadCalibrationImages();
	bool LoadParams(Object<ImageDto> dto);
	std::vector<Vec3f> CreateObjectPoints(cv::Size sz, float w = 1.0f, float h = 1.0f);
	long long SumData(Mat& img);

	// Member variables
	long m_nNumIterations;
	long m_nSegmentLength;
	long m_nColorRadius;
	long m_nSpatialRadius;

	// defaults
	cv::Mat TheImage;
	cv::Mat cameraMatrix;
	cv::Mat distortionMatrix;
	cv::Mat newCameraMat;
	cv::Mat homographyMatrix;
	cv::Mat invHomographyMatrix;
	cv::Mat m_matFillPoints;
	std::vector<std::vector<cv::Point2f>> m_vecRackPolys;
	std::vector<std::vector<cv::Point>> m_vecMaskPolys;

	double m_dDiffOfGaussRadius1;
	double m_dDiffOfGaussSigma1;
	double m_dDiffOfGaussRadius2;
	double m_dDiffOfGaussSigma2;
	double m_dResultScale;
	double m_dResultLength;
	double m_dResultWidth;
	double m_dBorderBottom;
	double m_dBorderTop;
	double m_dBorderLeft;
	double m_dBorderRight;

	string m_szCameraMatrix;
	string m_szDistortionMatrix;
	string m_szHomographyMatrix;

	long m_nFillToleranceLow;
	long m_nFillToleranceHigh;
	long m_nImageIndex;

	string m_szMeasurements;

	string m_str;
};

#include OATPP_CODEGEN_END(ApiController) //<-- End Codegen

#endif /* MyController_hpp */
