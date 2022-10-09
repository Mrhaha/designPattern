#include "frame_md5.h"

#define MD5_LBLOCK	16

#ifndef MD5_ULONG
typedef unsigned int MD5_ULONG;
#endif

////////////////////////////////////////////////////////////////////////
  
/* POINTER defines a generic pointer type */
typedef unsigned char *POINTER;

/* UINT2 defines a two byte word */
typedef unsigned short int UINT2;

/* UINT4 defines a four byte word */
typedef unsigned int UINT4;


typedef struct MD5state_st
{
	MD5_ULONG A,B,C,D;
	MD5_ULONG Nl,Nh;
	MD5_ULONG data[MD5_LBLOCK];
	int num;
} MD5_CTX;

void md5_init(MD5_CTX *c);
void md5_update_ex(MD5_CTX *c, const unsigned char *data, MD5_ULONG len);
void md5_final(unsigned char *md, MD5_CTX *c);

#ifndef L_ENDIAN
#define L_ENDIAN
#endif

#define INIT_DATA_A (MD5_ULONG)0x67452301L
#define INIT_DATA_B (MD5_ULONG)0xefcdab89L
#define INIT_DATA_C (MD5_ULONG)0x98badcfeL
#define INIT_DATA_D (MD5_ULONG)0x10325476L

void md5_init(MD5_CTX *c)
{
	memset(c,0,sizeof(MD5_CTX));
	c->A=INIT_DATA_A;
	c->B=INIT_DATA_B;
	c->C=INIT_DATA_C;
	c->D=INIT_DATA_D;
	c->Nl=0;
	c->Nh=0;
	c->num=0;
}

#define MD5_CBLOCK	64

#define c2l(c,l)	(l = ((MD5_ULONG)(*((c)++))), l|=(((MD5_ULONG)(*((c)++)))<< 8), l|=(((MD5_ULONG)(*((c)++)))<<16), l|=(((MD5_ULONG)(*((c)++)))<<24))

#define p_c2l(c,l,n)	{ switch (n) { case 0: l =((MD5_ULONG)(*((c)++))); case 1: l|=((MD5_ULONG)(*((c)++)))<< 8; case 2: l|=((MD5_ULONG)(*((c)++)))<<16; case 3: l|=((MD5_ULONG)(*((c)++)))<<24; } }

#define p_c2l_p(c,l,sc,len) { switch (sc) { case 0: l =((MD5_ULONG)(*((c)++))); if (--len == 0) break; case 1: l|=((MD5_ULONG)(*((c)++)))<< 8; if (--len == 0) break; case 2: l|=((MD5_ULONG)(*((c)++)))<<16; } }

#define c2l_p(c,l,n)	{ l=0; (c)+=n; switch (n) { case 3: l =((MD5_ULONG)(*(--(c))))<<16; case 2: l|=((MD5_ULONG)(*(--(c))))<< 8; case 1: l|=((MD5_ULONG)(*(--(c))))    ; } }


#define	F(b,c,d)	((((c) ^ (d)) & (b)) ^ (d))
#define	G(b,c,d)	((((b) ^ (c)) & (d)) ^ (c))
#define	H(b,c,d)	((b) ^ (c) ^ (d))
#define	I(b,c,d)	(((~(d)) | (b)) ^ (c))

//#if defined(WIN32)
//#define ROTATE(a,n)     (((a)<<(n))|((a)>>(32-(n))))
//#else
#define ROTATE(a,n)     (((a)<<(n))|(((a)&0xffffffffL)>>(32-(n))))
//#endif


#define R0(a,b,c,d,k,s,t) { a+=((k)+(t)+F((b),(c),(d))); a=ROTATE(a,s); a+=b; };
#define R1(a,b,c,d,k,s,t) { a+=((k)+(t)+G((b),(c),(d))); a=ROTATE(a,s); a+=b; };

#define R2(a,b,c,d,k,s,t) { a+=((k)+(t)+H((b),(c),(d))); a=ROTATE(a,s); a+=b; };

#define R3(a,b,c,d,k,s,t) { a+=((k)+(t)+I((b),(c),(d))); a=ROTATE(a,s); a+=b; };

static void md5_block(MD5_CTX *c, register MD5_ULONG *X, int num)
{
	register MD5_ULONG A,B,C,D;

	A=c->A;
	B=c->B;
	C=c->C;
	D=c->D;
	for (;;)
	{
		num-=64;
		if (num < 0) break;
		/* Round 0 */
		R0(A,B,C,D,X[ 0], 7,0xd76aa478L);
		R0(D,A,B,C,X[ 1],12,0xe8c7b756L);
		R0(C,D,A,B,X[ 2],17,0x242070dbL);
		R0(B,C,D,A,X[ 3],22,0xc1bdceeeL);
		R0(A,B,C,D,X[ 4], 7,0xf57c0fafL);
		R0(D,A,B,C,X[ 5],12,0x4787c62aL);
		R0(C,D,A,B,X[ 6],17,0xa8304613L);
		R0(B,C,D,A,X[ 7],22,0xfd469501L);
		R0(A,B,C,D,X[ 8], 7,0x698098d8L);
		R0(D,A,B,C,X[ 9],12,0x8b44f7afL);
		R0(C,D,A,B,X[10],17,0xffff5bb1L);
		R0(B,C,D,A,X[11],22,0x895cd7beL);
		R0(A,B,C,D,X[12], 7,0x6b901122L);
		R0(D,A,B,C,X[13],12,0xfd987193L);
		R0(C,D,A,B,X[14],17,0xa679438eL);
		R0(B,C,D,A,X[15],22,0x49b40821L);
		/* Round 1 */
		R1(A,B,C,D,X[ 1], 5,0xf61e2562L);
		R1(D,A,B,C,X[ 6], 9,0xc040b340L);
		R1(C,D,A,B,X[11],14,0x265e5a51L);
		R1(B,C,D,A,X[ 0],20,0xe9b6c7aaL);
		R1(A,B,C,D,X[ 5], 5,0xd62f105dL);
		R1(D,A,B,C,X[10], 9,0x02441453L);
		R1(C,D,A,B,X[15],14,0xd8a1e681L);
		R1(B,C,D,A,X[ 4],20,0xe7d3fbc8L);
		R1(A,B,C,D,X[ 9], 5,0x21e1cde6L);
		R1(D,A,B,C,X[14], 9,0xc33707d6L);
		R1(C,D,A,B,X[ 3],14,0xf4d50d87L);
		R1(B,C,D,A,X[ 8],20,0x455a14edL);
		R1(A,B,C,D,X[13], 5,0xa9e3e905L);
		R1(D,A,B,C,X[ 2], 9,0xfcefa3f8L);
		R1(C,D,A,B,X[ 7],14,0x676f02d9L);
		R1(B,C,D,A,X[12],20,0x8d2a4c8aL);
		/* Round 2 */
		R2(A,B,C,D,X[ 5], 4,0xfffa3942L);
		R2(D,A,B,C,X[ 8],11,0x8771f681L);
		R2(C,D,A,B,X[11],16,0x6d9d6122L);
		R2(B,C,D,A,X[14],23,0xfde5380cL);
		R2(A,B,C,D,X[ 1], 4,0xa4beea44L);
		R2(D,A,B,C,X[ 4],11,0x4bdecfa9L);
		R2(C,D,A,B,X[ 7],16,0xf6bb4b60L);
		R2(B,C,D,A,X[10],23,0xbebfbc70L);
		R2(A,B,C,D,X[13], 4,0x289b7ec6L);
		R2(D,A,B,C,X[ 0],11,0xeaa127faL);
		R2(C,D,A,B,X[ 3],16,0xd4ef3085L);
		R2(B,C,D,A,X[ 6],23,0x04881d05L);
		R2(A,B,C,D,X[ 9], 4,0xd9d4d039L);
		R2(D,A,B,C,X[12],11,0xe6db99e5L);
		R2(C,D,A,B,X[15],16,0x1fa27cf8L);
		R2(B,C,D,A,X[ 2],23,0xc4ac5665L);
		/* Round 3 */
		R3(A,B,C,D,X[ 0], 6,0xf4292244L);
		R3(D,A,B,C,X[ 7],10,0x432aff97L);
		R3(C,D,A,B,X[14],15,0xab9423a7L);
		R3(B,C,D,A,X[ 5],21,0xfc93a039L);
		R3(A,B,C,D,X[12], 6,0x655b59c3L);
		R3(D,A,B,C,X[ 3],10,0x8f0ccc92L);
		R3(C,D,A,B,X[10],15,0xffeff47dL);
		R3(B,C,D,A,X[ 1],21,0x85845dd1L);
		R3(A,B,C,D,X[ 8], 6,0x6fa87e4fL);
		R3(D,A,B,C,X[15],10,0xfe2ce6e0L);
		R3(C,D,A,B,X[ 6],15,0xa3014314L);
		R3(B,C,D,A,X[13],21,0x4e0811a1L);
		R3(A,B,C,D,X[ 4], 6,0xf7537e82L);
		R3(D,A,B,C,X[11],10,0xbd3af235L);
		R3(C,D,A,B,X[ 2],15,0x2ad7d2bbL);
		R3(B,C,D,A,X[ 9],21,0xeb86d391L);

		A+=c->A&0xffffffffL;
		B+=c->B&0xffffffffL;
		c->A=A;
		c->B=B;
		C+=c->C&0xffffffffL;
		D+=c->D&0xffffffffL;
		c->C=C;
		c->D=D;
		X+=16;
	}
}

void md5_update_ex(MD5_CTX *c, const unsigned char *data, MD5_ULONG len)
{
	register MD5_ULONG *p;
	int sw,sc;
	MD5_ULONG l;

	if (len == 0) return;

	l=(c->Nl+(len<<3))&0xffffffffL;
	/* 95-05-24 eay Fixed a bug with the overflow handling, thanks to
	* Wei Dai <weidai@eskimo.com> for pointing it out. */
	if (l < c->Nl) /* overflow */
		c->Nh++;
	c->Nh+=(len>>29);
	c->Nl=l;

	if (c->num != 0)
	{
		p=c->data;
		sw=c->num>>2;
		sc=c->num&0x03;

		if ((c->num+len) >= MD5_CBLOCK)
		{
			l= p[sw];
			p_c2l(data,l,sc);
			p[sw++]=l;
			for (; sw<MD5_LBLOCK; sw++)
			{
				c2l(data,l);
				p[sw]=l;
			}
			len-=(MD5_CBLOCK-c->num);

			md5_block(c,p,64);
			c->num=0;
			/* drop through and do the rest */
		}
		else
		{
			int ew,ec;

			c->num+=(int)len;
			if ((sc+len) < 4) /* ugly, add char's to a word */
			{
				l= p[sw];
				p_c2l_p(data,l,sc,len);
				p[sw]=l;
			}
			else
			{
				ew=(c->num>>2);
				ec=(c->num&0x03);
				l= p[sw];
				p_c2l(data,l,sc);
				p[sw++]=l;
				for (; sw < ew; sw++)
				{ c2l(data,l); p[sw]=l; }
				if (ec)
				{
					c2l_p(data,l,ec);
					p[sw]=l;
				}
			}
			return;
		}
	}

	/* we now can process the input data in blocks of MD5_CBLOCK
	* chars and save the leftovers to c->data. */
#ifdef L_ENDIAN
	if ((((unsigned long)data)%sizeof(MD5_ULONG)) == 0)
	{
		sw=len/MD5_CBLOCK;
		if (sw > 0)
		{
			sw*=MD5_CBLOCK;
			md5_block(c,(MD5_ULONG *)data,sw);
			data+=sw;
			len-=sw;
		}
	}
#endif
	p=c->data;
	while (len >= MD5_CBLOCK)
	{
#if defined(L_ENDIAN) || defined(B_ENDIAN)
		if (p != (MD5_ULONG *)data)
			memcpy(p,data,MD5_CBLOCK);
		data+=MD5_CBLOCK;
#ifdef B_ENDIAN
		for (sw=(MD5_LBLOCK/4); sw; sw--)
		{
			Endian_Reverse32(p[0]);
			Endian_Reverse32(p[1]);
			Endian_Reverse32(p[2]);
			Endian_Reverse32(p[3]);
			p+=4;
		}
#endif
#else
		for (sw=(MD5_LBLOCK/4); sw; sw--)
		{
			c2l(data,l); *(p++)=l;
			c2l(data,l); *(p++)=l;
			c2l(data,l); *(p++)=l;
			c2l(data,l); *(p++)=l; 
		} 
#endif
		p=c->data;
		md5_block(c,p,64);
		len-=MD5_CBLOCK;
	}
	sc=(int)len;
	c->num=sc;
	if (sc)
	{
		sw=sc>>2;	/* words to copy */
#ifdef L_ENDIAN
		p[sw]=0;
		memcpy(p,data,sc);
#else
		sc&=0x03;
		for ( ; sw; sw--)
		{ c2l(data,l); *(p++)=l; }
		c2l_p(data,l,sc);
		*p=l;
#endif
	}
}

#define MD5_LAST_BLOCK  56

#define l2c(l,c)	(*((c)++)=(unsigned char)(((l)     )&0xff), *((c)++)=(unsigned char)(((l)>> 8L)&0xff), *((c)++)=(unsigned char)(((l)>>16L)&0xff), *((c)++)=(unsigned char)(((l)>>24L)&0xff))


void md5_final(unsigned char *md, MD5_CTX *c)
{
	register int i,j;
	register MD5_ULONG l;
	register MD5_ULONG *p;
	static unsigned char end[4]={0x80,0x00,0x00,0x00};
	unsigned char *cp=end;

	/* c->num should definitly have room for at least one more byte. */
	p=c->data;
	j=c->num;
	i=j>>2;

	/* purify often complains about the following line as an
	* Uninitialized Memory Read.  While this can be true, the
	* following p_c2l macro will reset l when that case is true.
	* This is because j&0x03 contains the number of 'valid' bytes
	* already in p[i].  If and only if j&0x03 == 0, the UMR will
	* occur but this is also the only time p_c2l will do
	* l= *(cp++) instead of l|= *(cp++)
	* Many thanks to Alex Tang <altitude@cic.net> for pickup this
	* 'potential bug' */
#ifdef PURIFY
	if ((j&0x03) == 0) p[i]=0;
#endif
	l=p[i];
	p_c2l(cp,l,j&0x03);
	p[i]=l;
	i++;
	/* i is the next 'undefined word' */
	if (c->num >= MD5_LAST_BLOCK)
	{
		for (; i<MD5_LBLOCK; i++)
			p[i]=0;
		md5_block(c,p,64);
		i=0;
	}
	for (; i<(MD5_LBLOCK-2); i++)
		p[i]=0;
	p[MD5_LBLOCK-2]=c->Nl;
	p[MD5_LBLOCK-1]=c->Nh;
	md5_block(c,p,64);
	cp=md;
	l=c->A; l2c(l,cp);
	l=c->B; l2c(l,cp);
	l=c->C; l2c(l,cp);
	l=c->D; l2c(l,cp);

	/* clear stuff, md5_block may be leaving some stuff on the stack
	* but I'm not worried :-) */
	c->num=0;
	/*	memset((char *)&c,0,sizeof(c));*/
}

void md5_hash_buffer(unsigned char *dst, // 16Bytes
				   const char *src,
				   int len)
{
	/*MD5_CTX *md5Info = ( MD5_CTX * ) hashInfo, md5InfoBuffer;*/
	MD5_CTX *md5Info, md5InfoBuffer;
	md5Info = &md5InfoBuffer;


	/* If the user has left it up to us to allocate the hash context buffer,
	use the internal buffer */
	/*
	if( md5Info == NULL )
	md5Info = &md5InfoBuffer;
	*/

	md5_init( md5Info );
	md5_update_ex( md5Info, (const unsigned char*)src, len );
	md5_final( dst, md5Info );


	/* Clean up */
	/*zeroise( &md5InfoBuffer, sizeof( MD5_CTX ) );*/
}

void md5_int( uint32_t *digest1, uint32_t *digest2, const char *str, int len)
{
	unsigned char digest[16];
	
	md5_hash_buffer (&digest[0], str, len);
	*digest1 = ((uint32_t)digest[0] << 24) + ((uint32_t)digest[1] << 16) + ((uint32_t)digest[2] << 8) + (uint32_t)digest[3];
	*digest2 = ((uint32_t)digest[8] << 24) + ((uint32_t)digest[9] << 16) + ((uint32_t)digest[10] << 8) + (uint32_t)digest[11];
}

void md5_int64(uint64_t *digest1, uint64_t* digest2, const char *str, int len)
{
	unsigned char digest[16] = {0};
	uint64_t tmp1 = 0;
	uint64_t tmp2 = 0;

	md5_hash_buffer(&digest[0], str, len);
	
	tmp1 = ((unsigned int)digest[0] << 24) | ((unsigned int)digest[1] << 16) | ((unsigned int)digest[2] << 8) | (unsigned int)digest[3];
	tmp2 = ((unsigned int)digest[4] << 24) | ((unsigned int)digest[5] << 16) | ((unsigned int)digest[6] << 8) | (unsigned int)digest[7];
	*digest1 = (tmp1 << 32) | tmp2;

	tmp1 = ((unsigned int)digest[8] << 24) | ((unsigned int)digest[9] << 16) | ((unsigned int)digest[10] << 8) | (unsigned int)digest[11];
	tmp2 = ((unsigned int)digest[12] << 24) | ((unsigned int)digest[13] << 16) | ((unsigned int)digest[14] << 8) | (unsigned int)digest[15];
	*digest2 = (tmp1 << 32) | tmp2;
}

#define GET_UINT32(n,b,i)                       \
{                                               \
	(n) = ( (uint32) (b)[(i)    ]       )       \
	| ( (uint32) (b)[(i) + 1] <<  8 )       \
	| ( (uint32) (b)[(i) + 2] << 16 )       \
	| ( (uint32) (b)[(i) + 3] << 24 );      \
}

#define PUT_UINT32(n,b,i)                       \
{                                               \
	(b)[(i)    ] = (uint8) ( (n)       );       \
	(b)[(i) + 1] = (uint8) ( (n) >>  8 );       \
	(b)[(i) + 2] = (uint8) ( (n) >> 16 );       \
	(b)[(i) + 3] = (uint8) ( (n) >> 24 );       \
}

void md5_starts( md5_context *ctx )
{
	ctx->total[0] = 0;
	ctx->total[1] = 0;

	ctx->state[0] = 0x67452301;
	ctx->state[1] = 0xEFCDAB89;
	ctx->state[2] = 0x98BADCFE;
	ctx->state[3] = 0x10325476;
}

void md5_process( md5_context *ctx, uint8 data[64] )
{
	uint32 X[16], A, B, C, D;

	GET_UINT32( X[0],  data,  0 );
	GET_UINT32( X[1],  data,  4 );
	GET_UINT32( X[2],  data,  8 );
	GET_UINT32( X[3],  data, 12 );
	GET_UINT32( X[4],  data, 16 );
	GET_UINT32( X[5],  data, 20 );
	GET_UINT32( X[6],  data, 24 );
	GET_UINT32( X[7],  data, 28 );
	GET_UINT32( X[8],  data, 32 );
	GET_UINT32( X[9],  data, 36 );
	GET_UINT32( X[10], data, 40 );
	GET_UINT32( X[11], data, 44 );
	GET_UINT32( X[12], data, 48 );
	GET_UINT32( X[13], data, 52 );
	GET_UINT32( X[14], data, 56 );
	GET_UINT32( X[15], data, 60 );

#define S(x,n) ((x << n) | ((x & 0xFFFFFFFF) >> (32 - n)))

#define P(a,b,c,d,k,s,t)                                \
	{                                                       \
	a += F(b,c,d) + X[k] + t; a = S(a,s) + b;           \
	}

	A = ctx->state[0];
	B = ctx->state[1];
	C = ctx->state[2];
	D = ctx->state[3];
#undef F
#define F(x,y,z) (z ^ (x & (y ^ z)))

	P( A, B, C, D,  0,  7, 0xD76AA478 );
	P( D, A, B, C,  1, 12, 0xE8C7B756 );
	P( C, D, A, B,  2, 17, 0x242070DB );
	P( B, C, D, A,  3, 22, 0xC1BDCEEE );
	P( A, B, C, D,  4,  7, 0xF57C0FAF );
	P( D, A, B, C,  5, 12, 0x4787C62A );
	P( C, D, A, B,  6, 17, 0xA8304613 );
	P( B, C, D, A,  7, 22, 0xFD469501 );
	P( A, B, C, D,  8,  7, 0x698098D8 );
	P( D, A, B, C,  9, 12, 0x8B44F7AF );
	P( C, D, A, B, 10, 17, 0xFFFF5BB1 );
	P( B, C, D, A, 11, 22, 0x895CD7BE );
	P( A, B, C, D, 12,  7, 0x6B901122 );
	P( D, A, B, C, 13, 12, 0xFD987193 );
	P( C, D, A, B, 14, 17, 0xA679438E );
	P( B, C, D, A, 15, 22, 0x49B40821 );

#undef F

#define F(x,y,z) (y ^ (z & (x ^ y)))

	P( A, B, C, D,  1,  5, 0xF61E2562 );
	P( D, A, B, C,  6,  9, 0xC040B340 );
	P( C, D, A, B, 11, 14, 0x265E5A51 );
	P( B, C, D, A,  0, 20, 0xE9B6C7AA );
	P( A, B, C, D,  5,  5, 0xD62F105D );
	P( D, A, B, C, 10,  9, 0x02441453 );
	P( C, D, A, B, 15, 14, 0xD8A1E681 );
	P( B, C, D, A,  4, 20, 0xE7D3FBC8 );
	P( A, B, C, D,  9,  5, 0x21E1CDE6 );
	P( D, A, B, C, 14,  9, 0xC33707D6 );
	P( C, D, A, B,  3, 14, 0xF4D50D87 );
	P( B, C, D, A,  8, 20, 0x455A14ED );
	P( A, B, C, D, 13,  5, 0xA9E3E905 );
	P( D, A, B, C,  2,  9, 0xFCEFA3F8 );
	P( C, D, A, B,  7, 14, 0x676F02D9 );
	P( B, C, D, A, 12, 20, 0x8D2A4C8A );

#undef F

#define F(x,y,z) (x ^ y ^ z)

	P( A, B, C, D,  5,  4, 0xFFFA3942 );
	P( D, A, B, C,  8, 11, 0x8771F681 );
	P( C, D, A, B, 11, 16, 0x6D9D6122 );
	P( B, C, D, A, 14, 23, 0xFDE5380C );
	P( A, B, C, D,  1,  4, 0xA4BEEA44 );
	P( D, A, B, C,  4, 11, 0x4BDECFA9 );
	P( C, D, A, B,  7, 16, 0xF6BB4B60 );
	P( B, C, D, A, 10, 23, 0xBEBFBC70 );
	P( A, B, C, D, 13,  4, 0x289B7EC6 );
	P( D, A, B, C,  0, 11, 0xEAA127FA );
	P( C, D, A, B,  3, 16, 0xD4EF3085 );
	P( B, C, D, A,  6, 23, 0x04881D05 );
	P( A, B, C, D,  9,  4, 0xD9D4D039 );
	P( D, A, B, C, 12, 11, 0xE6DB99E5 );
	P( C, D, A, B, 15, 16, 0x1FA27CF8 );
	P( B, C, D, A,  2, 23, 0xC4AC5665 );

#undef F

#define F(x,y,z) (y ^ (x | ~z))

	P( A, B, C, D,  0,  6, 0xF4292244 );
	P( D, A, B, C,  7, 10, 0x432AFF97 );
	P( C, D, A, B, 14, 15, 0xAB9423A7 );
	P( B, C, D, A,  5, 21, 0xFC93A039 );
	P( A, B, C, D, 12,  6, 0x655B59C3 );
	P( D, A, B, C,  3, 10, 0x8F0CCC92 );
	P( C, D, A, B, 10, 15, 0xFFEFF47D );
	P( B, C, D, A,  1, 21, 0x85845DD1 );
	P( A, B, C, D,  8,  6, 0x6FA87E4F );
	P( D, A, B, C, 15, 10, 0xFE2CE6E0 );
	P( C, D, A, B,  6, 15, 0xA3014314 );
	P( B, C, D, A, 13, 21, 0x4E0811A1 );
	P( A, B, C, D,  4,  6, 0xF7537E82 );
	P( D, A, B, C, 11, 10, 0xBD3AF235 );
	P( C, D, A, B,  2, 15, 0x2AD7D2BB );
	P( B, C, D, A,  9, 21, 0xEB86D391 );

#undef F

	ctx->state[0] += A;
	ctx->state[1] += B;
	ctx->state[2] += C;
	ctx->state[3] += D;
}

void md5_update( md5_context *ctx, uint8 *input, uint32 length )
{
	uint32 left, fill;

	if( ! length ) return;

	left = ctx->total[0] & 0x3F;
	fill = 64 - left;

	ctx->total[0] += length;
	ctx->total[0] &= 0xFFFFFFFF;

	if( ctx->total[0] < length )
		ctx->total[1]++;

	if( left && length >= fill )
	{
		memcpy( (void *) (ctx->buffer + left),
			(void *) input, fill );
		md5_process( ctx, ctx->buffer );
		length -= fill;
		input  += fill;
		left = 0;
	}

	while( length >= 64 )
	{
		md5_process( ctx, input );
		length -= 64;
		input  += 64;
	}

	if( length )
	{
		memcpy( (void *) (ctx->buffer + left), (void *) input, length );
	}
}

static uint8 md5_padding[64] =
{
	0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

void md5_finish( md5_context *ctx, uint8 digest[16] )
{
	uint32 last, padn;
	uint32 high, low;
	uint8 msglen[8];

	high = ( ctx->total[0] >> 29 )
		| ( ctx->total[1] <<  3 );
	low  = ( ctx->total[0] <<  3 );

	PUT_UINT32( low,  msglen, 0 );
	PUT_UINT32( high, msglen, 4 );

	last = ctx->total[0] & 0x3F;
	padn = ( last < 56 ) ? ( 56 - last ) : ( 120 - last );

	md5_update( ctx, md5_padding, padn );
	md5_update( ctx, msglen, 8 );

	PUT_UINT32( ctx->state[0], digest,  0 );
	PUT_UINT32( ctx->state[1], digest,  4 );
	PUT_UINT32( ctx->state[2], digest,  8 );
	PUT_UINT32( ctx->state[3], digest, 12 );
}

char *md5_string(const unsigned char *in, char *out, uint32 in_len)
{
	int i;
	md5_context ctx;
	unsigned char md5sum[16];

	md5_starts(&ctx);
	md5_update(&ctx, (uint8 *)in, in_len);
	md5_finish(&ctx, md5sum);

	for (i = 0; i < 16; i++)
	{
		sprintf(out + i * 2, "%02x", md5sum[i]);
	}

	return(out);
}

int ascii_int(char c)
{
	if ( c >= 'a' && c <= 'f' )
    {
		return c-'a'+10;
    }
	else
    {
		return c-'0';
    }
}

char *md5_convert(char* in, char out[16])
{
	int i = 0;
	for (i=0; i<16; i++)
	{
		out[i] =ascii_int(in[2*i])*16+ascii_int(in[2*i+1]);
	}
	return out;
}
