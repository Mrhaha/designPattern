#pragma once

#include <cstddef>

class CXTEA
{
public:
    enum
    {
		MAX_ENCRYPT_PADDING_LENGTH = 16,
	};

public:
	//返回操作后的buffer大小，失败为0
    static size_t Encrypt(char* pbyInBuffer, size_t uiInBufferLength, char* pbyOutBuffer, size_t iOutBufferLength, const char szKey[16]);
	static size_t Decrypt(char* pbyInBuffer, size_t uiInBufferLength, char* pbyOutBuffer, size_t iOutBufferLength, const char szKey[16]);

	static size_t EncryptRoundN(char* pbyInBuffer, size_t nInBufferLength, char* pbyOutBuffer, size_t nOutBufferLength, const char arrbyKey[16], int n);
	static size_t DecryptRoundN(char* pbyInBuffer, size_t nInBufferLength, char* pbyOutBuffer, size_t nOutBufferLength, const char arrbyKey[16], int n);

	//获取足够的加解密buffer大小
	static size_t GetSafeEncryptOutBufferLength(size_t nInBufferLength);
	static size_t GetSafeDecryptOutBufferLength(size_t nInBufferLength);	
};
