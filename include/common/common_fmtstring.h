#pragma once

#include <map>
#include <vector>
#include <string>

class CFormatString
{
public:
    struct template_section_t
    {
        template_section_t() : m_bIsVar(false), m_strStr() {}
        
        bool            m_bIsVar;
        std::string     m_strStr;
    };
    
public:
    static CFormatString Compile(const std::string& strCompileStr);
    
public:
    CFormatString() = default;
    
    std::string Replace(const std::map<std::string, std::string>& stValMap) const;

	bool empty() const;

private:
    std::vector<template_section_t>         m_stSectionList;
};
