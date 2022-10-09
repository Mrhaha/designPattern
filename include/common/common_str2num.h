#pragma once
#include <string>

template<typename T>
bool try_str2num(const std::string & from, T & to) = delete;

template<>
inline bool try_str2num(const std::string & from, int & to)
{
	try
	{
		to = std::stoi(from);
	}
	catch (...)
	{
		return false;
	}

	return true;
}

template<>
inline bool try_str2num(const std::string& from, long& to)
{
	try
	{
		to = std::stol(from);
	}
	catch (...)
	{
		return false;
	}

	return true;
}

template<>
inline bool try_str2num(const std::string& from, unsigned long& to)
{
	try
	{
		to = std::stoul(from);
	}
	catch (...)
	{
		return false;
	}

	return true;
}

template<>
inline bool try_str2num(const std::string& from, long long& to)
{
	try
	{
		to = std::stoll(from);
	}
	catch (...)
	{
		return false;
	}

	return true;
}

template<>
inline bool try_str2num(const std::string& from, unsigned long long& to)
{
	try
	{
		to = std::stoull(from);
	}
	catch (...)
	{
		return false;
	}

	return true;
}

template<>
inline bool try_str2num(const std::string& from, double& to)
{
	try
	{
		to = std::stod(from);
	}
	catch (...)
	{
		return false;
	}

	return true;
}


