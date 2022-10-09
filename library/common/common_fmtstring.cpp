#include "common_fmtstring.h"

enum class FORMAT_TEMPLATE_FSM
{
    START,
    NORMAL_STR,
    VAR_STR
};

struct format_template_sentence_t
{
    format_template_sentence_t()
    : m_strSentence(), m_bInEscape(false) {}
    
    void Clear()
    {
        m_strSentence.clear();
        m_bInEscape = false;
    }
    
    std::string     m_strSentence;
    bool            m_bInEscape;
};

CFormatString CFormatString::Compile(const std::string& strCompileStr)
{
	auto enuState = FORMAT_TEMPLATE_FSM::START;
    
    CFormatString stObj;
    format_template_sentence_t stSentence;
    for(auto cChar : strCompileStr)
    {
        switch(enuState)
        {
            case FORMAT_TEMPLATE_FSM::START:
                stSentence.Clear();
                if(cChar == '$')
                {
                    enuState = FORMAT_TEMPLATE_FSM::VAR_STR;
                }
                else
                {
                    enuState = FORMAT_TEMPLATE_FSM::NORMAL_STR;
                    if(cChar == '\\')
                    {
                        stSentence.m_bInEscape = true;
                    }
                    else
                    {
                        stSentence.m_bInEscape = false;
                        stSentence.m_strSentence.push_back(cChar);
                    }
                }
                break;
            case FORMAT_TEMPLATE_FSM::NORMAL_STR:
                if(cChar == '$')
                {
                    if(stSentence.m_bInEscape)
                    {
                        stSentence.m_strSentence.push_back(cChar);
                        stSentence.m_bInEscape = false;
                    }
                    else
                    {
                        enuState = FORMAT_TEMPLATE_FSM::VAR_STR;
                        template_section_t stSection;
                        stSection.m_bIsVar = false;
                        stSection.m_strStr = stSentence.m_strSentence;
                        stObj.m_stSectionList.push_back(stSection);
                        
                        stSentence.Clear();
                    }
                }
                else if(cChar == '\\')
                {
                    if(stSentence.m_bInEscape)
                    {
                        stSentence.m_strSentence.push_back(cChar);
                        stSentence.m_bInEscape = false;
                    }
                    else
                    {
                        stSentence.m_bInEscape = true;
                    }
                }
                else
                {
                    stSentence.m_bInEscape = false;
                    stSentence.m_strSentence.push_back(cChar);
                }
                break;
            case FORMAT_TEMPLATE_FSM::VAR_STR:
                if(cChar == '$')
                {
                    if(stSentence.m_bInEscape)
                    {
                        stSentence.m_strSentence.push_back(cChar);
                        stSentence.m_bInEscape = false;
                    }
                    else
                    {
                        enuState = FORMAT_TEMPLATE_FSM::START;
                        template_section_t stSection;
                        stSection.m_bIsVar = true;
                        stSection.m_strStr = stSentence.m_strSentence;
                        stObj.m_stSectionList.push_back(stSection);
                        
                        stSentence.Clear();
                    }
                }
                else if(cChar == '\\')
                {
                    if(stSentence.m_bInEscape)
                    {
                        stSentence.m_strSentence.push_back(cChar);
                        stSentence.m_bInEscape = false;
                    }
                    else
                    {
                        stSentence.m_bInEscape = true;
                    }
                }
                else
                {
                    stSentence.m_bInEscape = false;
                    stSentence.m_strSentence.push_back(cChar);
                }
                
                break;
            default:
                break;
        };
    }

    if(enuState == FORMAT_TEMPLATE_FSM::NORMAL_STR)
    {
        template_section_t stSection;
        stSection.m_bIsVar = false;
        stSection.m_strStr = stSentence.m_strSentence;
        stObj.m_stSectionList.push_back(stSection);
    }
    
    return (stObj);
}

std::string CFormatString::Replace(const std::map<std::string, std::string>& stValMap) const
{
    std::string strNewStr;
    for(auto stIter = m_stSectionList.begin(); stIter != m_stSectionList.end(); ++stIter)
    {
        if(stIter->m_bIsVar)
        {
            auto stVarIter = stValMap.find(stIter->m_strStr);
            if(stVarIter != stValMap.end()) strNewStr += stVarIter->second;
        }
        else
        {
            strNewStr += stIter->m_strStr;
        }
    }
    
    return (strNewStr);
}

bool CFormatString::empty() const
{
	return m_stSectionList.empty();
}
