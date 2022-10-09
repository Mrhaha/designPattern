#pragma once

#include "string"

class CLogicTextChecker
{
public:
    static bool IsDigital(char cChar);
    
    static bool IsAlphabet(char cChar);

    static bool IsDisplayChar(char cChar);

	static std::string FilterEmoji(const std::string& strContent);

	static std::string Skip4ByteChars(const std::string& strContent);

    static bool CheckNickHasDirtyWord(const std::string& strNick);

    static bool FilterTalkContent(std::string& strContent);
    
    static int CheckNickCorrect(const std::string& strNick, int& iDisplayCharWidth);
};



