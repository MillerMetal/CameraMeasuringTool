#ifndef MyController_hpp
#define MyController_hpp

#include "pch.h"
#include "dto/DTOs.hpp"

#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController) //<-- Begin Codegen

/**
 * Sample Api Controller.
 */
class MyController : public oatpp::web::server::api::ApiController 
{
	//protected:
	//  MyController(const std::shared_ptr<ObjectMapper>& objectMapper) : oatpp::web::server::api::ApiController(objectMapper)
	//  {}
	public:

	  /**
	   *  Inject @objectMapper component here as default parameter
	   *  Do not return bare Controllable* object! use shared_ptr!
	   */
	  static std::shared_ptr<MyController> createShared(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>,
																		objectMapper))
	  {
		  return std::shared_ptr<MyController>(new MyController(objectMapper));
	  }


	ENDPOINT_INFO(root) {
		info->summary = "Root endpoint with 'Hello World!!!' message";
		info->addResponse<Object<MyDto>>(
			Status::CODE_200,
			"application/json"
			);
	};

	ENDPOINT_INFO(ProcessPicture) {
		info->summary = "ProcessPicture endpoint with 'Picture Processed' message";
		info->addResponse<Object<MyDto>>(
			Status::CODE_200,
			"application/json"
			);
	};

	ENDPOINT_INFO(TransferPicture) {
		info->summary = "TransferPicture endpoint with 'Picture Transferred' message";
		info->addResponse<Object<MyDto>>(
			Status::CODE_200,
			"application/json"
			);
	};


	//ENDPOINT("GET", "/", root) {
	//	auto dto = MyDto::createShared();
	//	dto->statusCode = 200;
	//	dto->message = "Hello World!";
	//	return createDtoResponse(Status::CODE_200, dto);
	//};

	//ENDPOINT("POST", "/ProcessPicture", ProcessPicture) {
	//	auto dto = MyDto::createShared();
	//	dto->statusCode = 200;
	//	dto->message = "Greetings World!";
	//	return createDtoResponse(Status::CODE_200, dto);
	//};

	//ENDPOINT("POST", "/TransferPicture", TransferPicture) {
	//	auto dto = MyDto::createShared();
	//	dto->statusCode = 200;
	//	dto->message = "Hello World!";
	//	return createDtoResponse(Status::CODE_200, dto);
	//};


	ENDPOINT_ASYNC("GET", "/", root) 
	{
		ENDPOINT_ASYNC_INIT(root)
			
		Action act() override 
		{
			auto dto = MyDto::createShared();
			dto->statusCode = 200;
			dto->message = "Hello World!";
			return _return(controller->createDtoResponse(Status::CODE_200, dto));
		}
	};


	// TODO Insert Your endpoints here !!!
	ENDPOINT_ASYNC("POST", "/ProcessPicture", ProcessPicture) 
	{
		ENDPOINT_ASYNC_INIT(ProcessPicture)

		Action act() override 
		{
			return request->readBodyToDtoAsync<oatpp::Object<ImageDto>>(controller->getDefaultObjectMapper())
					.callbackTo(&ProcessPicture::onBodyObtained);
		}

		Action onBodyObtained(const oatpp::Object<ImageDto>& dto) 
		{
			//dto->statusCode = 200;
			//dto->message = "Hello World!";
			return _return(controller->createDtoResponse(Status::CODE_200, dto));
		}
	};

	ENDPOINT_ASYNC("POST", "/TransferPicture", TransferPicture) 
	{
		ENDPOINT_ASYNC_INIT(TransferPicture)

		Action act() override 
		{
			return request->readBodyToDtoAsync<oatpp::Object<ImageDto>>(controller->getDefaultObjectMapper())
				.callbackTo(&TransferPicture::onBodyObtained);
		}

		Action onBodyObtained(const oatpp::Object<ImageDto>& dto) 
		{
			dto->statusCode = 200;
			dto->message = "Hello World!";
			return _return(controller->createDtoResponse(Status::CODE_200, dto));
		}
	};

public:
	string szOrgImage;
	string szLastFileName;

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
	void DetectCorners(string& szFileName);
	void LoadCalibrationImages();
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
};

#include OATPP_CODEGEN_END(ApiController) //<-- End Codegen

#endif /* MyController_hpp */
