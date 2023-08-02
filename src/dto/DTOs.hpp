#ifndef DTOs_hpp
#define DTOs_hpp

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

/**
 *  Data Transfer Object. Object containing fields only.
 *  Used in API for serialization/deserialization and validation
 */
class MyDto : public oatpp::DTO {
  DTO_INIT(MyDto, DTO)
  DTO_FIELD(Int32, statusCode              );
  DTO_FIELD(String, message              );
};

class ImageDto : public oatpp::DTO {
	DTO_INIT(ImageDto, DTO)
	DTO_FIELD(String,  image               );
	DTO_FIELD(String,  description               );
	DTO_FIELD(String,  fullfilename               );
	DTO_FIELD(String,  szURL               );
	DTO_FIELD(Float64, dDiffOfGaussRadius1               );
	DTO_FIELD(Float64, dDiffOfGaussSigma1               );
	DTO_FIELD(Float64, dDiffOfGaussRadius2               );
	DTO_FIELD(Float64, dDiffOfGaussSigma2               );
	DTO_FIELD(Float64, dResultScale               );
	DTO_FIELD(Float64, dResultLength               );
	DTO_FIELD(Float64, dResultWidth               );
	DTO_FIELD(Float64, dBorderBottom               );
	DTO_FIELD(Float64, dBorderTop               );
	DTO_FIELD(Float64, dBorderLeft               );
	DTO_FIELD(Float64, dBorderRight               );
	DTO_FIELD(String,  szCameraMatrix               );
	DTO_FIELD(String,  szDistortionMatrix               );
	DTO_FIELD(String,  szHomographyMatrix               );
	DTO_FIELD(Int32,   nFillToleranceLow               );
	DTO_FIELD(Int32,   nFillToleranceHigh               );
	DTO_FIELD(Int32,   nImageIndex               );
	DTO_FIELD(Int32,   nMSSNumIterations);
	DTO_FIELD(Int32,   nMSSSegmentLength);
	DTO_FIELD(Int32,   nMSSColorRadius  );
	DTO_FIELD(Int32,   nMSSSpatialRadius);
	DTO_FIELD(String,  szMeasurements);
};

#include OATPP_CODEGEN_END(DTO)

#endif /* DTOs_hpp */
