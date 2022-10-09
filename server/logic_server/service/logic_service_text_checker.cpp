#include "logic_service_text_checker.h"
#include "tc_utf8.h"
#include "common_utf8handle.h"
#include "logic_service_dirty_word_checker.h"
#include "../config/logic_config_base.h"

bool 
CLogicTextChecker::IsDigital(char cChar)
{
    if((cChar >= '0') && (cChar <= '9'))
    {
        return (true);
    }

    return (false);
}

bool 
CLogicTextChecker::IsAlphabet(char cChar)
{
    if(((cChar >= 'A') && (cChar <= 'Z')) || ((cChar >= 'a') && (cChar <= 'z')))
    {
        return (true); 
    }

    return (false);
}

bool CLogicTextChecker::IsDisplayChar(char cChar)
{
    return cChar == ' ';
}

std::string CLogicTextChecker::FilterEmoji(const std::string& strContent)
{
	if (strContent.empty())
	{
		return strContent;
	}
	std::string filter;
	for (size_t i = 0; i < strContent.size(); i++)
	{
		const char codePoint = strContent[i];
		if (codePoint & 0x80)
		{
			switch (static_cast<unsigned>(codePoint & 0xf0))
			{
			case 0xf0:
				if (i + 1 < strContent.size())
				{
					if (static_cast<unsigned>(strContent[i + 1] & 0x9f) != 0x9f && i+1 < strContent.size())
					{
						for (int j = 0; j < 4; ++j)
						{
							filter.push_back(strContent[i + j]);
						}
						i += 3;
					}
				}				
				break;
			case 0xe0:
				if (i +2 < strContent.size())
				{
					for (int j = 0; j < 3; ++j)
					{
						filter.push_back(strContent[i + j]);
					}
					i += 2;
				}				
				break;
			case 0xc0:
				if (i+1 < strContent.size())
				{
					for (int j = 0; j < 2; ++j)
					{
						filter.push_back(strContent[i + j]);
					}
					i += 1;
				}				
				break;
			}
		}
		else
		{
			filter.push_back(strContent[i]);
		}
	}
	return filter;
}

std::string CLogicTextChecker::Skip4ByteChars(const std::string& strContent)
{
	// 字符转ascii码，返回值为无符号int
	const auto ord = [](int ch) -> unsigned char{
		unsigned char ret;
		ret = ch & 0xff;
		return ret;
	};

	std::string out;
	for (size_t i = 0; i < strContent.length(); )
	{
		const unsigned char v = ord(strContent[i]);
		if (v == 0x09 || v == 0x0A || (v < 0x80 && v >= 0x20)) { // 单字节
			out += v;
			i += 1;
		}
		else if (v >= 0xC2 && v <= 0xDF && i + 1 < strContent.length()) { // 双字节
			const unsigned char v2 = ord(strContent[i + 1]);
			if (v2 >= 80 && v2 <= 0xBF) {
				out += v;
				out += v2;
				i += 2;
			}
			else {
				++i;
			}
		}
		else if (v == 0xE0 && i + 2 < strContent.length()) { // 三字节
			const unsigned char v2 = ord(strContent[i + 1]);
			const unsigned char v3 = ord(strContent[i + 2]);
			if (v2 >= 0xA0 && v2 <= 0xBF && v3 >= 0x80 && v3 <= 0xBF) {
				out += v;
				out += v2;
				out += v3;
				i += 3;
			}
			else {
				++i;
			}
		}
		else if (v == 0xED && i + 2 < strContent.length()) { // 三字节
			const unsigned char v2 = ord(strContent[i + 1]);
			const unsigned char v3 = ord(strContent[i + 2]);
			if (v2 >= 0x80 && v2 <= 0x9F && v3 >= 0x80 && v3 <= 0xBF) {
				out += v;
				out += v2;
				out += v3;
				i += 3;
			}
			else {
				i++;
			}
		}
		else if (v >= 0xE1 && v <= 0xEF && v != 0xED && i + 2 < strContent.length()) { // 三字节
			const unsigned char v2 = ord(strContent[i + 1]);
			const unsigned char v3 = ord(strContent[i + 2]);
			if (v2 >= 0x80 && v2 <= 0xBF && v3 >= 0x80 && v3 <= 0xBF) {
				out += v;
				out += v2;
				out += v3;
				i += 3;
			}
			else {
				i++;
			}
		}
		else if (v >= 0xF1 && v <= 0xF4) { // 四字节
			i += 4;
		}
		else { // 四字节以上
			i++;
		}
	}

	return out;
}

int 
CLogicTextChecker::CheckNickCorrect(const std::string& strNick, int& iDisplayCharWidth)
{
    iDisplayCharWidth = 0;
    int iNumLen = 0;
    std::string::const_iterator stIT = strNick.begin();
    while(stIT != strNick.end())
    {
        int iCharNum = CTCUTF8Handle::GetByteNumOFWord(*stIT);
        if(0 > iCharNum)
        {
            return (-1);
        }
        else if(1 == iCharNum)
        {
            /*
            // 不能有空格
            if(IsDisplayChar(*stIT))
            {
                return (-4);
            }
             */

            if(!IsDigital(*stIT) && !IsAlphabet(*stIT))
            {
                return (-2);
            }

            if(IsDigital(*stIT))
            {
                ++iNumLen;
            }

            iDisplayCharWidth ++;
            stIT ++;
        }
        else
        {

            unsigned int uiChar = 0;
            for(int i = 0; i < iCharNum; i ++)
            {
                uiChar = (uiChar << 8) + (((unsigned int)*(stIT ++)) & 0xFF);
            }

/*          if(ILLEGAL_CHINESE_UTF8_CHAR_INS->HasIllegalChar(uiChar) == true)
            {
                return (-3);
            }
*/
            iDisplayCharWidth += 2;
        }
    }

    // 不能为全数字
    if(iNumLen == iDisplayCharWidth)
        return (-5);

    return (0);
}

bool
CLogicTextChecker::CheckNickHasDirtyWord(const std::string& strNick)
{
	if (Skip4ByteChars(strNick) != strNick)
	{
		return true;
	}
    std::vector<uint32_t> stNick;
    CUTF8Handle::Decode(strNick, stNick);
    return (SERVICE_DIRTY_WORD_FILTER_INS->Find(stNick,EnumFilterWordsType::EnumFilterWordsType_Name_Type));
}

bool
CLogicTextChecker::FilterTalkContent(string& strContent)
{
    std::vector<uint32_t> stContent;
    CUTF8Handle::Decode(strContent, stContent);
    SERVICE_DIRTY_WORD_FILTER_INS->Replace(stContent, '*',EnumFilterWordsType::EnumFilterWordsType_Chat_Type);
    strContent.clear();
    CUTF8Handle::Encode(stContent, strContent);
    return true;
}
