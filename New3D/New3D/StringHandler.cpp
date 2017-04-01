#include "PercompIncludes.h"
#include "StringHandler.h"
#include <sstream>
#include <iomanip>
void DXStringHandler::DXExtractFilePath(string src, string& dest)
{

	size_t index = src.find_last_of("/\\");
	dest = src.substr(0, index);
	dest += "\\";
}
void DXStringHandler::DXExtractNameFromFile(string src, string& dest)
{
	size_t index = src.find_last_of("/\\");
	size_t fnSize = src.size();
	dest = src.substr((index + 1), (fnSize - index));
	index = dest.find_last_of(".");
	dest = dest.substr(0, index);

}
void DXStringHandler::DXExtractFilename(string src, string& dest)
{
	size_t index = src.find_last_of("/\\");
	size_t fnSize = src.size();
	dest = src.substr((index + 1), (fnSize - index));
}
void DXStringHandler::DXExtractFileExt(string src, string &dest)
{
	size_t srcSize = src.size();
	size_t index = src.find_last_of(".");
	dest = src.substr((index), (srcSize - index));
}
bool DXStringHandler::DXDoesFileExist(string src)
{

	struct stat buffer;
	return (stat(src.c_str(), &buffer) == 0);

}
bool DXStringHandler::DXLoadTextFile(string src, vector<string>& lines)
{
	bool result = DXDoesFileExist(src);
	if (result)
	{
		std::ifstream file;
		file.open(src.c_str());
		if (!file.good())
			return false;

		string str;
		while (std::getline(file, str))
		{
			lines.push_back(str);
		}
		file.close();
	}
	return result;
}

void DXStringHandler::DXConvertFromStrToWStr(string str, wstring& out_str)
{
	WCHAR wcStr[MAX_PATH];
	size_t convertedChars = 0;
	size_t newsize = strlen(str.c_str()) + 1;
	mbstowcs_s(&convertedChars, wcStr, newsize, str.c_str(), _TRUNCATE);
	out_str = std::wstring(wcStr);

}

void DXStringHandler::DXCreateIndexedNameString(string name, int index, string& result)
{
	string delim;
	(index > 9) ? delim = "0" : delim = "00";
	result = name + delim + to_string(index);

}
void DXStringHandler::Tokenize(vector<std::string>* tokens, string s, string delimiter)
{
	// check that is still valid
	while (s.find(delimiter, 0) != std::string::npos) {
		// get pos of next delimiter
		size_t pos = s.find(delimiter, 0);
		// create a token
		std::string token = s.substr(0, pos);
		// erase the current token 
		s.erase(0, pos + 1);
		// add the token to  vector
		tokens->push_back(token);
	}
	// push the last token
	tokens->push_back(s);
}

void DXStringHandler::DXFloatPercisionWString(std::wstring & str, const float val, const int percision)
{
	std::wstringstream stream;
	stream << fixed << setprecision(percision) << val;
	str = stream.str();
	stream.clear();
}

void DXStringHandler::DXFloatPercisionString(std::string & str, const float val, const int percision)
{
	std::stringstream stream;
	stream << fixed << setprecision(percision) << val;
	str = stream.str();
	stream.clear();
}
