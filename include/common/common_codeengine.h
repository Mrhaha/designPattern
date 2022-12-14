#pragma once

#include <cstdint>
#include <string>

//´Ó*pOutÌø¹ýiByteCount¸ö×Ö½Ú
void skip_byte_of(char** pOut, int32_t iByteCount);

class CCodeEngine
{
public:

	static int32_t encode_int8(char **pOut,  uint8_t Src);
	static int32_t decode_int8(char **pIn,  uint8_t *pOut);
	static int32_t encode_int16(char **pOut,  uint16_t Src);
	static int32_t decode_int16(char **pIn,  uint16_t* pOut);

	static int32_t encode_int32(char **pOut,  uint32_t Src);
	static int32_t decode_int32(char **pIn,  uint32_t *pOut);

	static int32_t encode_string(char **pOut, const char *pSrc, const int16_t nMaxStringLength);
	static int32_t decode_string(char **pIn, char *pOut, const int16_t nMaxStringLength);

	static int32_t encode_memory(char **pOut, char *pSrc, const int32_t iMemorySize);
	static int32_t decode_memory(char **pIn, char *pOut, const int32_t iMemorySize);

	static int32_t encode_int64(char **pOut, uint64_t Src);
	static int32_t decode_int64(char **pIn, uint64_t *pOut);

	static int32_t encode_string(char **pOut, const std::string& str);
	static int32_t decode_string(char **pIn, std::string& out);
};
