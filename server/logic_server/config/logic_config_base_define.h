#pragma once

#include <cstdint>
#include <set>
#include <map>
#include <vector>
#include <string>

#define RECORD_ARRAY_MEM_HANDLE3(value_type, table_type, name, name1, name2, name3)   \
std::vector<std::function<value_type(table_type*)>> stGet##name##FuncVec = {  \
    nullptr,                                                                \
    std::bind(&table_type::Get##name1, std::placeholders::_1),              \
    std::bind(&table_type::Get##name2, std::placeholders::_1),              \
    std::bind(&table_type::Get##name3, std::placeholders::_1)               \
};                                                                          \
std::vector<std::function<void(table_type*, value_type)>> stSet##name##FuncVec = {      \
    nullptr,                                                                            \
    std::bind(&table_type::Set##name1, std::placeholders::_1, std::placeholders::_2),   \
    std::bind(&table_type::Set##name2, std::placeholders::_1, std::placeholders::_2),   \
    std::bind(&table_type::Set##name3, std::placeholders::_1, std::placeholders::_2)    \
};

#define RECORD_ARRAY_MEM_HANDLE4(value_type, table_type, name, name1, name2, name3, name4)   \
std::vector<std::function<value_type(table_type*)>> stGet##name##FuncVec = {  \
    nullptr,                                                                \
    std::bind(&table_type::Get##name1, std::placeholders::_1),              \
    std::bind(&table_type::Get##name2, std::placeholders::_1),              \
    std::bind(&table_type::Get##name3, std::placeholders::_1),              \
    std::bind(&table_type::Get##name4, std::placeholders::_1)               \
};                                                                          \
std::vector<std::function<void(table_type*, value_type)>> stSet##name##FuncVec = {      \
    nullptr,                                                                            \
    std::bind(&table_type::Set##name1, std::placeholders::_1, std::placeholders::_2),   \
    std::bind(&table_type::Set##name2, std::placeholders::_1, std::placeholders::_2),   \
    std::bind(&table_type::Set##name3, std::placeholders::_1, std::placeholders::_2),   \
    std::bind(&table_type::Set##name4, std::placeholders::_1, std::placeholders::_2)    \
};
