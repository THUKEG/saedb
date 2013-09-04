#pragma once
#include <string>
using std::string;
using std::wstring;

class TokenUtil
{
public:
	TokenUtil(void);
	~TokenUtil(void);
	static string ws2s(const wstring& ws);
	static wstring s2ws(const string& s);
	static wchar_t* s2wchar(const string& s);
	static string wchar2s(wchar_t* _source, int length);
};

