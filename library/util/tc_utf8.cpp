#include "tc_utf8.h"

#include <set>

const std::string CTCUTF8Handle::CHAR_ENCODING_TYPE_NAME = "UTF-8";

int 
CTCUTF8Handle::CheckEncodingTypeCorrect(const std::string& strStr)
{
    int iCount = 0;

    size_t uiIndex = 0;

	while(uiIndex < strStr.size())
	{
		int iByteNum = GetByteNumOFWord((unsigned char)strStr[uiIndex]);
		if(iByteNum <= 0)
		{
			return (-1);
		}
		else if(2 == iByteNum)
        {
            int iResult = CheckNotHeadByteCorrect((unsigned char)strStr[uiIndex + 1]);
            if(0 != iResult)
            {
                return (-2);
            }
        }
        else if(3 == iByteNum)
        {
            int iResult = CheckNotHeadByteCorrect((unsigned char)strStr[uiIndex + 1]);
            iResult |= CheckNotHeadByteCorrect((unsigned char)strStr[uiIndex + 2]);
            if(0 != iResult)
            {
                return (-3);
            }
        }
        else if(4 == iByteNum)
        {
            int iResult = CheckNotHeadByteCorrect((unsigned char)strStr[uiIndex + 1]);
            iResult |= CheckNotHeadByteCorrect((unsigned char)strStr[uiIndex + 2]);
            iResult |= CheckNotHeadByteCorrect((unsigned char)strStr[uiIndex + 3]);
            if(0 != iResult)
            {
                return (-4);
            }
        }
		
        uiIndex += (size_t)iByteNum;
        iCount ++;
	}

	return (iCount);
}

void 
CTCUTF8Handle::EncodeWordsToIntArray(const std::string& strStr, std::vector<char_encoding_data_t>& stEncodedArray)
{
    size_t uiIndex = 0;

    char_encoding_data_t stEncodingData;
	while(uiIndex < strStr.size())
	{
        uint32_t uiCode = 0;
		int iByteNum = GetByteNumOFWord((unsigned char)strStr[uiIndex]);
		if(iByteNum <= 0)
		{
            uiIndex ++;
            continue;
		}
        else if(1 == iByteNum)
        {
            uiCode = (uiCode << 8) + ((unsigned int)strStr[uiIndex] & 0xFF);
        }
		else if(2 == iByteNum)
        {
            uiCode = (uiCode << 8) + ((unsigned int)strStr[uiIndex] & 0xFF);
            uiCode = (uiCode << 8) + ((unsigned int)strStr[uiIndex + 1] & 0xFF);
        }
        else if(3 == iByteNum)
        {
            uiCode = (uiCode << 8) + ((unsigned int)strStr[uiIndex] & 0xFF);
            uiCode = (uiCode << 8) + ((unsigned int)strStr[uiIndex + 1] & 0xFF);
            uiCode = (uiCode << 8) + ((unsigned int)strStr[uiIndex + 2] & 0xFF);
        }
        else if(4 == iByteNum)
        {
            uiCode = (uiCode << 8) + ((unsigned int)strStr[uiIndex] & 0xFF);
            uiCode = (uiCode << 8) + ((unsigned int)strStr[uiIndex + 1] & 0xFF);
            uiCode = (uiCode << 8) + ((unsigned int)strStr[uiIndex + 2] & 0xFF);
            uiCode = (uiCode << 8) + ((unsigned int)strStr[uiIndex + 3] & 0xFF);
        }
		
        stEncodingData.m_ucCharByteNum = iByteNum;
        stEncodingData.m_uiEncodingINT = uiCode;
        stEncodedArray.push_back(stEncodingData);
        uiIndex += (size_t)iByteNum;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
CIllegalChineseUTF8Char::chinese_illegal_unicode_char_t CIllegalChineseUTF8Char::ILLEGAL_CHAR_SET[] = 
{
    { 0x00A9, 0x00A9 },
    { 0x00AE, 0x00AE },
    { 0x203C, 0x203C },
    { 0x2049, 0x2049 },
    { 0x2122, 0x2122 },
    { 0x2139, 0x2139 },
    { 0x2194, 0x2199 },
    { 0x21A9, 0x21AA },
    { 0x231A, 0x231B },
    { 0x23E9, 0x23EC },
    { 0x23F0, 0x23F0 },
    { 0x23F3, 0x23F3 },
    { 0x25AA, 0x25AB },
    { 0x25B6, 0x25B6 },
    { 0x25C0, 0x25C0 },
    { 0x25FB, 0x2601 },
    { 0x260E, 0x260E },
    { 0x2611, 0x2611 },
    { 0x2614, 0x2615 },
    { 0x261D, 0x261D },
    { 0x263A, 0x263A },
    { 0x2648, 0x2653 },
    { 0x2660, 0x2660 },
    { 0x2663, 0x2663 },
    { 0x2665, 0x2666 },
    { 0x2668, 0x2668 },
    { 0x267B, 0x267B },
    { 0x267F, 0x267F },
    { 0x2693, 0x2693 },
    { 0x26A0, 0x26A1 },
    { 0x26AA, 0x26AB },
    { 0x26BD, 0x26BE },
    { 0x26C4, 0x26C5 },
    { 0x26CE, 0x26CE },
    { 0x26D4, 0x26D4 },
    { 0x26EA, 0x26EA },
    { 0x26F2, 0x26F3 },
    { 0x26F5, 0x26F5 },
    { 0x26FA, 0x26FA },
    { 0x26FD, 0x26FD },
    { 0x2934, 0x2935 },
    { 0x2B05, 0x2B07 },
    { 0x2B1B, 0x2B1C },
    { 0x2B50, 0x2B50 },
    { 0x2B55, 0x2B55 },
    { 0x3030, 0x3030 },
    { 0x303D, 0x303D },
    { 0x3297, 0x3297 },
    { 0x3299, 0x3299 },
    { 0x1F004, 0x1F004 },
    { 0x1F0CF, 0x1F0CF },
    { 0x1F300, 0x1F5FF },
    
    { 0xFF00, 0xFFEF },
    { 0x2E80, 0x2EFF },
    { 0x3000, 0x303F },
    { 0x31C0, 0x31EF },
    { 0x2F00, 0x2FDF },
    { 0x2FF0, 0x2FFF },
    { 0x3100, 0x312F },
    { 0x31A0, 0x31BF },
    { 0x3040, 0x309F },
    { 0x30A0, 0x30FF },
    { 0x31F0, 0x31FF },
    { 0xAC00, 0xD7AF },
    { 0x1100, 0x11FF },
    { 0x3130, 0x318F },
    { 0x4DC0, 0x4DFF },
    { 0xA000, 0xA4CF },
    { 0x2800, 0x28FF },
    { 0x3200, 0x33FF },
    { 0x2600, 0x27BF },
    { 0xFE10, 0xFE1F },
    
    { 0xFE30, 0xFE4F },
    { 0x24C2, 0x24C2 },
    { 0x2702, 0x27B0 },
    { 0x1F170, 0x1F251 },
    { 0x1F601, 0x1F64F },
    { 0x1F680, 0x1F6C0 },
    { 0x1F600, 0x1F636 },
    { 0x1F681, 0x1F6C5 },
    { 0x1F30D, 0x1F567 }
};

void 
CIllegalChineseUTF8Char::Init()
{
    m_stIllegalCharSet.clear();
    
    std::set<unsigned int> stIllegalCharMap;
    size_t uiIllegalWordSet = sizeof(ILLEGAL_CHAR_SET) / sizeof(chinese_illegal_unicode_char_t);
	for(size_t i = 0; i < uiIllegalWordSet; i ++)
	{
		for(unsigned int j = ILLEGAL_CHAR_SET[i].uiStart; j <= ILLEGAL_CHAR_SET[i].uiEnd; j ++)
		{
            unsigned int uiCharEncoding;
			int iRet = ConvUnicode2UTF8Encoding(j, uiCharEncoding);
			if(0 == iRet)
			{
				continue;
			}
			
            stIllegalCharMap.insert(uiCharEncoding);
		}
	}
    
    unsigned int uiPrev = 0;
    illegal_utf8_char_t stIllegalUTF8Set;
    for(std::set<unsigned int>::const_iterator stIT = stIllegalCharMap.begin(); stIT != stIllegalCharMap.end(); stIT ++)
    {
        if(stIT == stIllegalCharMap.begin())
        {
            stIllegalUTF8Set.m_uiStart = *stIT;
            uiPrev = *stIT;
        }
        else
        {
            if(*stIT != (uiPrev + 1))
            {
                stIllegalUTF8Set.m_uiEnd = uiPrev;
                m_stIllegalCharSet.push_back(stIllegalUTF8Set);

                stIllegalUTF8Set.m_uiStart = *stIT;
            }

            uiPrev = *stIT;
        }
    }
    
    stIllegalUTF8Set.m_uiEnd = uiPrev;
    m_stIllegalCharSet.push_back(stIllegalUTF8Set);
}

bool 
CIllegalChineseUTF8Char::HasIllegalChar(unsigned int uiUTF8Char)
{
    if(m_stIllegalCharSet.empty() == true)
    {
        return (false);
    }
    
    size_t uiStart = 0, uiEnd = m_stIllegalCharSet.size() - 1;
    while(1)
    {
        if(uiStart == uiEnd)
        {
            if((uiUTF8Char >= m_stIllegalCharSet[uiStart].m_uiStart) && (uiUTF8Char <= m_stIllegalCharSet[uiStart].m_uiEnd))
            {
                return (true);
            }
            
            return (false);
        }
        else if((uiStart + 1) == uiEnd)
        {
            if((uiUTF8Char >= m_stIllegalCharSet[uiStart].m_uiStart) && (uiUTF8Char <= m_stIllegalCharSet[uiStart].m_uiEnd))
            {
                return (true);
            }
            else if((uiUTF8Char >= m_stIllegalCharSet[uiEnd].m_uiStart) && (uiUTF8Char <= m_stIllegalCharSet[uiEnd].m_uiEnd))
            {
                return (true);
            }
            
            return (false);
        }
        else
        {
            size_t uiIndex = (size_t)((uiStart + uiEnd) / 2);
            if(uiUTF8Char > m_stIllegalCharSet[uiIndex].m_uiEnd)
            {
                uiStart = uiIndex + 1;
            }
            else if(uiUTF8Char < m_stIllegalCharSet[uiIndex].m_uiStart)
            {
                uiEnd = uiIndex - 1;
            }
            else
            {
                return (true);
            }
        }
    }
}

int 
CIllegalChineseUTF8Char::ConvUnicode2UTF8Encoding(unsigned int uiUnicode, unsigned int& uiUTF8)
{
    if(uiUnicode <= 0x0000007F)
    {
        // * U-00000000 - U-0000007F:  0xxxxxxx
        uiUTF8 = uiUnicode & 0x7F;

        return (1);
    }
    else if((uiUnicode >= 0x00000080) && (uiUnicode <= 0x000007FF))
    {
        // * U-00000080 - U-000007FF:  110xxxxx 10xxxxxx
        uiUTF8 = ((uiUnicode >> 6) & 0x1F) | 0xC0;

        uiUTF8 = (uiUTF8 << 8) + ((uiUnicode & 0x3F) | 0x80);	

        return (2);
    }
    else if((uiUnicode >= 0x00000800) && (uiUnicode <= 0x0000FFFF))
    {
        // * U-00000800 - U-0000FFFF:  1110xxxx 10xxxxxx 10xxxxxx
        uiUTF8 = ((uiUnicode >> 12) & 0x0F) | 0xE0;

        uiUTF8 = (uiUTF8 << 8) + (((uiUnicode >>  6) & 0x3F) | 0x80);

        uiUTF8 = (uiUTF8 << 8) + ((uiUnicode & 0x3F) | 0x80);

        return (3);
    }
    else if((uiUnicode >= 0x00010000) && (uiUnicode <= 0x001FFFFF))
    {
        // * U-00010000 - U-001FFFFF:  11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
        uiUTF8 = ((uiUnicode >> 18) & 0x07) | 0xF0;

        uiUTF8 = (uiUTF8 << 8) + (((uiUnicode >> 12) & 0x3F) | 0x80);

        uiUTF8 = (uiUTF8 << 8) + (((uiUnicode >>  6) & 0x3F) | 0x80);

        uiUTF8 = (uiUTF8 << 8) + ((uiUnicode & 0x3F) | 0x80);	

        return (4);
    }

    return 0;
}



