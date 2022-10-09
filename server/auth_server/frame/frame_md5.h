#pragma once

#include "frame_common.h"

#define MD5_DIGEST_LENGTH	16
#define MD5_MAX_LEN         33

void md5_hash_buffer(unsigned char *dst, // 16Bytes
		const char *src,
		int len);

/************************************************************
功能：
	对字符串做MD5,生成两个8B的无符号整数
输入：
	unsigned char *str			--	字符串
	int nInBufferLength			--	字符串长度
输出：
	unsigned long long &digest1	--	无符号整数
	unsigned long long &digest2	--	无符号整数
*************************************************************/
void md5_int64(uint64_t *digest1, uint64_t *digest2, const char *str, int len);

#ifndef HASH_LEN
#define HASH_LEN 16
#endif

#ifndef uint8
#define uint8  unsigned char
#endif

#ifndef uint32
//#define uint32 unsigned long int
#define uint32 unsigned int
#endif

typedef struct
{
	uint32 total[2];
	uint32 state[4];
	uint8 buffer[64];
} md5_context;

void md5_starts( md5_context *ctx );
void md5_update( md5_context *ctx, uint8 *input, uint32 length );
void md5_finish( md5_context *ctx, uint8 digest[16] );
char *md5_string(const unsigned char *in, char *out, uint32 in_len);
char *md5_convert(char* in, char out[16]);
