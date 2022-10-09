#pragma once

#include <stdint.h>

#include <string>
#include <vector>

#include "tc_singleton.h"

class CTCUTF8Handle
{
public:
    typedef uint32_t char_encoding_type;
    
    typedef struct char_encoding_data_s
    {
        uint8_t                 m_ucCharByteNum;
        char_encoding_type      m_uiEncodingINT;
    }char_encoding_data_t;
    
public:
    static const std::string CHAR_ENCODING_TYPE_NAME;
    
public:
    /**
     * @desc:获取字符编码名称
     * @return:const std::string,字符编码的名称 
     */
    static const std::string GetHandleName()
    {
        return (CHAR_ENCODING_TYPE_NAME);
    }
    
    /**
     * @desc:通过UTF-8字符的第一个字节判断怎么字符所占字节数
     * @param ucChar,字符第一个字节
     * @return int,字符所占字节数
     */
    static int GetByteNumOFWord(unsigned char ucChar)
    {
        if((ucChar & 0x80) != 0)
        {
            if(((ucChar & 0xF0) ^ 0xF0) == 0)
            {
                return (4);
            }

            if(((ucChar & 0xE0) ^ 0xE0) == 0)
            {
                return (3);
            }

            if(((ucChar & 0xC0) ^ 0xC0) == 0)
            {
                return (2);
            }

            return (-1);
        }
        else
        {
            return (1);
        }
    }
    
    /**
     * 检测UTF-8字符的除第一个字节的字符的正确性
     * @param ucChar,字符字节
     * @return int,0:正确,其他:错误
     */
    static int CheckNotHeadByteCorrect(unsigned char ucChar)
    {
        return ((ucChar & 0xC0) ^ 0x80);
    }
    
    /**
     * 检测UTF-8字符序列是否有乱码
     * @param strStr,UTF-8字符序列
     * @return int,UTF-8字符个数 
     */
    static int CheckEncodingTypeCorrect(const std::string& strStr);
    
    /**
     * 把UTF-8字符序列编码成整形序列
     * @param strStr,UTF-8字符序列
     * @param stEncodedArray,整形序列
     * @return int,0:正确,其他:错误
     */
    static void EncodeWordsToIntArray(const std::string& strStr, std::vector<char_encoding_data_t>& stEncodedArray);
};

class CIllegalChineseUTF8Char
{
public:
    typedef struct unicode_illegal_char_s
    {
        unsigned int uiStart;
        unsigned int uiEnd;	
    }chinese_illegal_unicode_char_t;

    static chinese_illegal_unicode_char_t ILLEGAL_CHAR_SET[];
    
    typedef struct illegal_utf8_char_s
    {
        illegal_utf8_char_s()
        {
            m_uiStart = 0;
            m_uiEnd = 0;
        }
        
        unsigned int        m_uiStart;
        unsigned int        m_uiEnd;
    }illegal_utf8_char_t;
    
public:
    void Init();
    
    bool HasIllegalChar(unsigned int uiUTF8Char);
    
private:
    int ConvUnicode2UTF8Encoding(unsigned int uiUnicode, unsigned int& uiUTF8);
    
private:
    std::vector<illegal_utf8_char_t>        m_stIllegalCharSet;
};

template <class T = CIllegalChineseUTF8Char>
class CIllegalChineseUTF8CharCreator
{
public:
    static CIllegalChineseUTF8Char* create()
    {
        CIllegalChineseUTF8Char* pIns = new CIllegalChineseUTF8Char();
        pIns->Init();
        return (pIns);
    }
    
    static void destroy(CIllegalChineseUTF8Char* pIns)
    {
        if(NULL != pIns)
        {
            delete pIns;
        }
    }
};

typedef taf::TC_Singleton<CIllegalChineseUTF8Char, CIllegalChineseUTF8CharCreator> ILLEGAL_CHINESE_UTF8_CHAR_INS_TYPE;

#define ILLEGAL_CHINESE_UTF8_CHAR_INS ILLEGAL_CHINESE_UTF8_CHAR_INS_TYPE::getInstance()
