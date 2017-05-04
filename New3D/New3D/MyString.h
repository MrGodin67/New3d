#pragma once
#include <string>

#include <sstream>
#include <iomanip>

struct FormatDesc
{
	float m_value;
	float m_percision;
	std::wstringstream stream;
	std::wstring str;
	FormatDesc(float val, float percision)
		:m_value(val), m_percision(percision)
	{
		stream << fixed << setprecision(percision) << val;
		str = stream.str();
		stream.clear();
	}
	inline std::wstring getStr() { return str; }

};

class wString : public std::wstring
{
	std::wstringstream stream;
public:
	wString() = default;
	wString(int val)
		:
		std::wstring(std::to_wstring(val))
	{
		
	};
	wString(unsigned int val)
		:
		std::wstring(std::to_wstring(val))
	{

	};
	wString(float val)
		:
		std::wstring(std::to_wstring(val))
	{

	};
	wString(double val)
		:
		std::wstring(std::to_wstring(val))
	{

	};
	wString(long val)
		:
		std::wstring(std::to_wstring(val))
	{

	};
	wString(wchar_t* val)
		:
		std::wstring(val)
	{

	};
	wString(size_t val)
		:
		std::wstring(std::to_wstring(val))
	{

	};
	wString(bool val)
		:
		std::wstring(std::to_wstring(val))
	{

	};
	
	wString(FormatDesc& desc)
		:
		std::wstring(desc.getStr())
	{
	};

	
	
	
};