#include "gy_xtea.h"

#include <string.h>
#include <stdlib.h>

/*
@desc xtea round N crypt
@param v,int* 64BITS of DATA[in/out]
@param o,int* 64BITS of DATA[out]
@param k,int* 128BIT key[in]
@param N,int Routine rounds[in]
*/
void xtea(int* v, int* o, const int* k, int N) 
{
	unsigned int y = v[0];
	unsigned int z = v[1];
	unsigned int DELTA = 0x9e3779b9; // 0x00000000 - 0x61C88647 == 0x9e3779b9
	if(N > 0)
	{
		// Encoding
		unsigned int uiLimit = DELTA * N;
		unsigned int uiSum = 0;
		while(uiSum != uiLimit)
		{
			y += ((z << 4) ^ (z >> 5)) + (z ^ uiSum) + k[uiSum & 3];
			uiSum += DELTA;
			z += ((y << 4) ^ (y >> 5)) + (y ^ uiSum) + k[(uiSum >> 11) & 3];
		}
	}
	else
	{
		// Decoding
		unsigned int uiSum = DELTA * (-N);
		while(uiSum)
		{
			z -= ((y << 4) ^ (y >> 5)) + (y ^ uiSum) + k[(uiSum >> 11) & 3];
			uiSum -= DELTA;
			y -= ((z << 4) ^ (z >> 5)) + (z ^ uiSum) + k[uiSum & 3];
		}
	}

	o[0] = y;
	o[1] = z;
}

/*****************************************************************************
//	加密后的BUFFER结构
//  ------------------------------------------------------------
//  @¦ PadLength   ¦  Padded Random BYTEs  ¦  Data   ¦ Zero s  ¦
//  @-----------------------------------------------------------
//  @¦    1Byte    ¦    PadLength Bytes    ¦ N Bytes ¦ 7Bytes  ¦
//  @-----------------------------------------------------------
// Pad部分将整个BUFFER补齐到8字节对齐
******************************************************************************/

#define ZERO_LENGTH 7
#define ENCRYPT_ROUNDS (32)
#define DECRYPT_ROUNDS (-32)
#define ENCRYPT_BLOCK_LENGTH_IN_BYTE (8)

size_t 
CXTEA::Encrypt(char* pbyInBuffer, size_t nInBufferLength, char* pbyOutBuffer, size_t nOutBufferLength, const char arrbyKey[16])
{
	if(pbyInBuffer == NULL || nInBufferLength <= 0)
	{
		return 0;
	}

	//计算输出BUFFER大小
	size_t nPadDataZero = 1/*Pad Length*/ + nInBufferLength + ZERO_LENGTH;
	size_t nPadLength = nPadDataZero % ENCRYPT_BLOCK_LENGTH_IN_BYTE;

	if(nPadLength != 0)
	{
		nPadLength = ENCRYPT_BLOCK_LENGTH_IN_BYTE - nPadLength;
	}

	size_t nTotalLength = nPadDataZero + nPadLength;
	if(nTotalLength > nOutBufferLength || pbyOutBuffer == NULL)
	{
		return 0;
	}

	char* pbyInCursor = pbyInBuffer;
	char* pbyOutCurosr = pbyOutBuffer;

	memset(pbyOutBuffer, 0, nOutBufferLength);

	char arrbyFirst8Bytes[ENCRYPT_BLOCK_LENGTH_IN_BYTE] = {0};
	// Pad length，只使用最低三位，高5位用随机数填充
	arrbyFirst8Bytes[0] = (((char)rand()) & 0xf8) | ((char)nPadLength);

	// 用随机数填充pad区
	for(size_t i =1; i<=nPadLength; ++i)
	{
		arrbyFirst8Bytes[i] = (char)rand();
	}

	// 用待加密数据补满第一块明文
	for(size_t i = 1+nPadLength; i<ENCRYPT_BLOCK_LENGTH_IN_BYTE; ++i)
	{
		arrbyFirst8Bytes[i] = *pbyInCursor++;
	}

	// 上一个加密块的明文与密文，用于后面的异或操作
	char* pbyLast8BytesCryptedData = pbyOutBuffer;
	char* pbyLast8BytesPlainData = arrbyFirst8Bytes;

	// 第一段Buffer，不需要异或操作
	xtea((int*)arrbyFirst8Bytes, (int*)pbyOutCurosr, (int*)arrbyKey, ENCRYPT_ROUNDS);
	pbyOutCurosr += ENCRYPT_BLOCK_LENGTH_IN_BYTE;

	// 下面这段是是用于不更改InBuffer的加密过程
	char arrbySrcBuffer[ENCRYPT_BLOCK_LENGTH_IN_BYTE] = {0};
	while((int)(pbyInCursor - pbyInBuffer) < (int)(nInBufferLength - 1))
	{
		memcpy(arrbySrcBuffer, pbyInCursor, ENCRYPT_BLOCK_LENGTH_IN_BYTE);
		// 和上一块密文异或
		for(int i=0; i<ENCRYPT_BLOCK_LENGTH_IN_BYTE; ++i)
		{
			arrbySrcBuffer[i] ^= pbyLast8BytesCryptedData[i];
		}
		xtea((int*)arrbySrcBuffer, (int*)pbyOutCurosr, (int*)arrbyKey, ENCRYPT_ROUNDS);
		// 和上一块明文异或
		for(int i=0; i<ENCRYPT_BLOCK_LENGTH_IN_BYTE; ++i)
		{
			pbyOutCurosr[i] ^= pbyLast8BytesPlainData[i];
		}

		pbyLast8BytesCryptedData = pbyOutCurosr;
		pbyLast8BytesPlainData = pbyInCursor;

		pbyOutCurosr += ENCRYPT_BLOCK_LENGTH_IN_BYTE;
		pbyInCursor += ENCRYPT_BLOCK_LENGTH_IN_BYTE;
	}

	// 结尾的 1Byte数据 + 7Byte 校验
	char arrbyLast8Bytes[ENCRYPT_BLOCK_LENGTH_IN_BYTE] = {0};
	arrbyLast8Bytes[0] = *pbyInCursor;

	// 和上一块密文异或
	for(int i=0; i<ENCRYPT_BLOCK_LENGTH_IN_BYTE; ++i)
	{
		arrbyLast8Bytes[i] ^= pbyLast8BytesCryptedData[i];
	}
	xtea((int*)arrbyLast8Bytes, (int*)pbyOutCurosr, (int*)arrbyKey, ENCRYPT_ROUNDS);
	// 和上一块明文异或
	for(int i=0; i<ENCRYPT_BLOCK_LENGTH_IN_BYTE; ++i)
	{
		pbyOutCurosr[i] ^= pbyLast8BytesPlainData[i];
	}

	return (nTotalLength);
}

// TODO: OutBufferLengthÅÐ¶Ï
size_t 
CXTEA::Decrypt(char* pbyInBuffer, size_t nInBufferLength, char* pbyOutBuffer, size_t nOutBufferLength, const char arrbyKey[16])
{
	if(pbyInBuffer == NULL || nInBufferLength <= 0)
	{
		return 0;
	}

	// Buffer长度应该是能被 ENCRYPT_BLOCK_LENGTH_IN_BYTE 整除的
	if(nInBufferLength%ENCRYPT_BLOCK_LENGTH_IN_BYTE || nInBufferLength <= 8)
	{
		return 0;
	}

	char* pbyInCursor = pbyInBuffer;
	char* pbyOutCursor = pbyOutBuffer;

	// 先解出最前面包含Pad的ENCRYPT_BLOCK_LENGTH_IN_BYTE个Byte
	char arrbyFirst8Bytes[ENCRYPT_BLOCK_LENGTH_IN_BYTE] = {0};
	xtea((int*)pbyInCursor, (int*)arrbyFirst8Bytes, (int*)arrbyKey, DECRYPT_ROUNDS);
	pbyInCursor += ENCRYPT_BLOCK_LENGTH_IN_BYTE;
	// Pad长度只是用了第一个字节的最低3bit，高5bit是随机数
	int nPadLength = arrbyFirst8Bytes[0] & 0x07;

	// 计算原始数据的长度
	size_t nPlainDataLength = nInBufferLength - 1/*PadLength Length*/ - nPadLength - ZERO_LENGTH;
	if(nPlainDataLength <= 0 || pbyOutBuffer == NULL)
	{
		return 0;
	}

	// OutBuffer不够
	if(nPlainDataLength > nOutBufferLength)
	{
		return 0;
	}

	// 前一块的明文和密文，用于后面的异或操作
	char* pbyLast8BytesCryptedData = pbyInBuffer;
	char* pbyLast8BytesPlainData = arrbyFirst8Bytes;

	// 将第一块里Pad信息之后的数据移到输出Buffer
	for(int i=0; i < 7/*ENCRYPT_BLOCK_LENGTH_IN_BYTE - 1*/ - nPadLength; ++i)
	{
		*pbyOutCursor++ = arrbyFirst8Bytes[1+nPadLength + i];
	}

	// 解密除了最后一块以外的所有块
	// 同加密过程，下面注释掉的，是不改动InBuffer的

	char arrbySrcBuffer[8] = {0};
	//for(int i=8; i<nInBufferLength-8; i+=8)
	while((int)(pbyInCursor - pbyInBuffer) < (int)(nInBufferLength - 8))
	{
		memcpy(arrbySrcBuffer, pbyInCursor, 8);
		// 和上一个8char明文异或
		for(int i=0; i<8; ++i)
		{
			arrbySrcBuffer[i] ^= pbyLast8BytesPlainData[i];
		}
		xtea((int*)arrbySrcBuffer, (int*)pbyOutCursor, (int*)arrbyKey, DECRYPT_ROUNDS);
		// 和上一个8char密文异或
		for(int i=0; i<8; ++i)
		{
			pbyOutCursor[i] ^= pbyLast8BytesCryptedData[i];
		}

		pbyLast8BytesCryptedData = pbyInCursor;
		pbyLast8BytesPlainData = pbyOutCursor;

		pbyInCursor += 8;
		pbyOutCursor += 8;
	}

	// 最后8Byte， 最后有7Byte的校验
	char arrbyLast8Bytes[ENCRYPT_BLOCK_LENGTH_IN_BYTE] = {0};
	// 和上一个8Byte明文异或
	memcpy(arrbySrcBuffer, pbyInCursor, 8);
	for(int i=0; i<ENCRYPT_BLOCK_LENGTH_IN_BYTE; ++i)
	{
		arrbySrcBuffer[i] ^= pbyLast8BytesPlainData[i];
	}
	xtea((int*)arrbySrcBuffer, (int*)arrbyLast8Bytes, (int*)arrbyKey, DECRYPT_ROUNDS);
	// 和上一个8Byte密文异或
	for(int i=0; i<ENCRYPT_BLOCK_LENGTH_IN_BYTE; ++i)
	{
		arrbyLast8Bytes[i] ^= pbyLast8BytesCryptedData[i];
	}

	// 校验最后的0
	for(int i=1; i<ENCRYPT_BLOCK_LENGTH_IN_BYTE; ++i)
	{
		if(arrbyLast8Bytes[i] != 0)
		{
			return 0;
		}
	}
	*pbyOutCursor = arrbyLast8Bytes[0];

	return (nPlainDataLength);
}


size_t 
CXTEA::EncryptRoundN(char* pbyInBuffer, size_t nInBufferLength, char* pbyOutBuffer, size_t nOutBufferLength, const char arrbyKey[16], int n)
{
	if(pbyInBuffer == NULL || nInBufferLength <= 0)
	{
		return (0);
	}

	// 计算需要的输出Buffer大小
	size_t nPadDataZero = 1/*Pad Length*/ + nInBufferLength + ZERO_LENGTH;
	size_t nPadLength = nPadDataZero % ENCRYPT_BLOCK_LENGTH_IN_BYTE;

	if(nPadLength != 0)
	{
		nPadLength = ENCRYPT_BLOCK_LENGTH_IN_BYTE - nPadLength;
	}

	size_t nTotalLength = nPadDataZero + nPadLength;

	if(nTotalLength > nOutBufferLength || pbyOutBuffer == NULL)
	{
		return (0);
	}

	char* pbyInCursor = pbyInBuffer;
	char* pbyOutCurosr = pbyOutBuffer;

	memset(pbyOutBuffer, 0, nOutBufferLength);

	char arrbyFirst8Bytes[ENCRYPT_BLOCK_LENGTH_IN_BYTE] = {0};
	// Pad length, 只使用最低三位，高5位用随机数填充
	arrbyFirst8Bytes[0] = (((char)rand()) & 0xf8) | ((char)nPadLength);

	// 用随机数填充Pad区
	for(size_t i =1; i<=nPadLength; ++i)
	{
		arrbyFirst8Bytes[i] = (char)rand();
	}

	// 用待加密数据补满第一块明文
	for(size_t i = 1+nPadLength; i<ENCRYPT_BLOCK_LENGTH_IN_BYTE; ++i)
	{
		arrbyFirst8Bytes[i] = *pbyInCursor++;
	}

	// 上一个加密块的明文与密文，用于后面的异或操作
	char* pbyLast8BytesCryptedData = pbyOutBuffer;
	char* pbyLast8BytesPlainData = arrbyFirst8Bytes;

	// 第一段Buffer，不需要异或操作
	xtea((int*)arrbyFirst8Bytes, (int*)pbyOutCurosr, (int*)arrbyKey, n);
	pbyOutCurosr += ENCRYPT_BLOCK_LENGTH_IN_BYTE;

	// 下面这段是是用于不更改InBuffer的加密过程
	char arrbySrcBuffer[ENCRYPT_BLOCK_LENGTH_IN_BYTE] = {0};
	while((int)(pbyInCursor - pbyInBuffer) < (int)(nInBufferLength - 1))
	{
		memcpy(arrbySrcBuffer, pbyInCursor, ENCRYPT_BLOCK_LENGTH_IN_BYTE);
		// 和上一块密文异或
		for(int i=0; i<ENCRYPT_BLOCK_LENGTH_IN_BYTE; ++i)
		{
			arrbySrcBuffer[i] ^= pbyLast8BytesCryptedData[i];
		}
		xtea((int*)arrbySrcBuffer, (int*)pbyOutCurosr, (int*)arrbyKey, n);
		// 和上一块明文异或
		for(int i=0; i<ENCRYPT_BLOCK_LENGTH_IN_BYTE; ++i)
		{
			pbyOutCurosr[i] ^= pbyLast8BytesPlainData[i];
		}

		pbyLast8BytesCryptedData = pbyOutCurosr;
		pbyLast8BytesPlainData = pbyInCursor;

		pbyOutCurosr += ENCRYPT_BLOCK_LENGTH_IN_BYTE;
		pbyInCursor += ENCRYPT_BLOCK_LENGTH_IN_BYTE;
	}

	// 结尾的 1Byte数据 + 7Byte 校验
	char arrbyLast8Bytes[ENCRYPT_BLOCK_LENGTH_IN_BYTE] = {0};
	arrbyLast8Bytes[0] = *pbyInCursor;

	// 和上一块密文异或
	for(int i=0; i<ENCRYPT_BLOCK_LENGTH_IN_BYTE; ++i)
	{
		arrbyLast8Bytes[i] ^= pbyLast8BytesCryptedData[i];
	}
	xtea((int*)arrbyLast8Bytes, (int*)pbyOutCurosr, (int*)arrbyKey, n);
	// 和上一块明文异或
	for(int i=0; i<ENCRYPT_BLOCK_LENGTH_IN_BYTE; ++i)
	{
		pbyOutCurosr[i] ^= pbyLast8BytesPlainData[i];
	}

	return (nTotalLength);
}

size_t 
CXTEA::DecryptRoundN(char* pbyInBuffer, size_t nInBufferLength, char* pbyOutBuffer, size_t nOutBufferLength, const char arrbyKey[16], int n)
{
	if(pbyInBuffer == NULL || nInBufferLength <= 0)
	{
		return (0);
	}

	// Buffer长度应该是能被 ENCRYPT_BLOCK_LENGTH_IN_BYTE 整除的
	if(nInBufferLength%ENCRYPT_BLOCK_LENGTH_IN_BYTE || nInBufferLength <= 8)
	{
		return (0);
	}

	char* pbyInCursor = pbyInBuffer;
	char* pbyOutCursor = pbyOutBuffer;

	// 先解出最前面包含Pad的ENCRYPT_BLOCK_LENGTH_IN_BYTE个Byte
	char arrbyFirst8Bytes[ENCRYPT_BLOCK_LENGTH_IN_BYTE] = {0};
	xtea((int*)pbyInCursor, (int*)arrbyFirst8Bytes, (int*)arrbyKey, n);
	pbyInCursor += ENCRYPT_BLOCK_LENGTH_IN_BYTE;
	// Pad长度只是用了第一个字节的最低3bit，高5bit是随机数
	int nPadLength = arrbyFirst8Bytes[0] & 0x07;

	// 计算原始数据的长度
	size_t nPlainDataLength = nInBufferLength - 1/*PadLength Length*/ - nPadLength - ZERO_LENGTH;
	if(nPlainDataLength <= 0 || pbyOutBuffer == NULL)
	{
		return (0);
	}

	// OutBuffer不够
	if(nPlainDataLength > nOutBufferLength)
	{
		return (0);
	}

	// 前一块的明文和密文，用于后面的异或操作
	char* pbyLast8BytesCryptedData = pbyInBuffer;
	char* pbyLast8BytesPlainData = arrbyFirst8Bytes;

	// 将第一块里Pad信息之后的数据移到输出Buffer
	for(int i=0; i < 7/*ENCRYPT_BLOCK_LENGTH_IN_BYTE - 1*/ - nPadLength; ++i)
	{
		*pbyOutCursor++ = arrbyFirst8Bytes[1+nPadLength + i];
	}

	// 解密除了最后一块以外的所有块
	// 同加密过程，下面注释掉的，是不改动InBuffer的

	char arrbySrcBuffer[8] = {0};
	while((int)(pbyInCursor - pbyInBuffer) < (int)(nInBufferLength - 8))
	{
		memcpy(arrbySrcBuffer, pbyInCursor, 8);
		// 和上一个8char明文异或
		for(int i=0; i<8; ++i)
		{
			arrbySrcBuffer[i] ^= pbyLast8BytesPlainData[i];
		}
		xtea((int*)arrbySrcBuffer, (int*)pbyOutCursor, (int*)arrbyKey, n);
		// 和上一个8char密文异或
		for(int i=0; i<8; ++i)
		{
			pbyOutCursor[i] ^= pbyLast8BytesCryptedData[i];
		}

		pbyLast8BytesCryptedData = pbyInCursor;
		pbyLast8BytesPlainData = pbyOutCursor;

		pbyInCursor += 8;
		pbyOutCursor += 8;
	}

	// 最后8Byte， 最后有7Byte的校验
	char arrbyLast8Bytes[ENCRYPT_BLOCK_LENGTH_IN_BYTE] = {0};
	// 和上一个8Byte明文异或
	memcpy(arrbySrcBuffer, pbyInCursor, 8);
	for(int i=0; i<ENCRYPT_BLOCK_LENGTH_IN_BYTE; ++i)
	{
		arrbySrcBuffer[i] ^= pbyLast8BytesPlainData[i];
	}
	xtea((int*)arrbySrcBuffer, (int*)arrbyLast8Bytes, (int*)arrbyKey, n);
	// 和上一个8Byte密文异或
	for(int i=0; i<ENCRYPT_BLOCK_LENGTH_IN_BYTE; ++i)
	{
		arrbyLast8Bytes[i] ^= pbyLast8BytesCryptedData[i];
	}

	// 校验最后的0
	for(int i=1; i<ENCRYPT_BLOCK_LENGTH_IN_BYTE; ++i)
	{
		if(arrbyLast8Bytes[i] != 0)
		{
			return (0);
		}
	}
	*pbyOutCursor = arrbyLast8Bytes[0];

	return (nPlainDataLength);
}

size_t CXTEA::GetSafeEncryptOutBufferLength(size_t nInBufferLength)
{
	return (nInBufferLength + 16);
}

size_t CXTEA::GetSafeDecryptOutBufferLength(size_t nInBufferLength)
{
	return (nInBufferLength);
}

