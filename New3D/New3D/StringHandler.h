#pragma once
#include <string>
#include <vector>
#include <Windows.h>
#include <fstream>
using namespace std;

class DXStringHandler
{
public:
	static void DXExtractFilePath(string src, string& dest);
	static void DXExtractFilename(string src, string& dest);
	static void DXExtractNameFromFile(string src, string& dest);
	static void DXExtractFileExt(string src, string &dest);
	static bool DXDoesFileExist(string src);
	static bool DXLoadTextFile(string src, vector<string>& lines);
	static void DXConvertFromStrToWStr(string str, wstring& out_str);
	static void DXCreateIndexedNameString(string name, int index, string& result);
	static WCHAR* WCharTranslate(WCHAR* strMsg, ...)
	{
		WCHAR strBuffer[512] = { '\0' };
		va_list args;
		va_start(args, strMsg);
		vswprintf_s(strBuffer, 512, strMsg, args);
		strBuffer[511] = L'\0';
		va_end(args);

		return (strBuffer);
	}
	static void Tokenize(vector<std::string>* tokens, string s, string delimiter);
	static void DXFloatPercisionWString(std::wstring& str, const float val,const int percision);
	static void DXFloatPercisionString(std::string& str, const float val, const int percision);
};