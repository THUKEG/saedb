#include <cstdlib>
#include <string>
#include <cstring>
#include "TokenUtil.h"
using namespace std;

TokenUtil::TokenUtil(void)
{
}


TokenUtil::~TokenUtil(void)
{
}

string TokenUtil::wchar2s(wchar_t* _source, int length)
{
	wchar_t temp = _source[length];
	_source[length] = 0;

	size_t _Dsize = (length << 1) + 1;
	char *_Dest = new char[_Dsize];
    memset(_Dest,0,_Dsize);
    wcstombs(_Dest,_source,_Dsize);
	string result = _Dest;
    delete []_Dest;

	_source[length] = temp;

	return result;
}

string TokenUtil::ws2s(const wstring& ws)
{
	const wchar_t* _Source = ws.c_str();
    size_t _Dsize = 2 * ws.size() + 1;
	char *_Dest = new char[_Dsize];
    memset(_Dest,0,_Dsize);
    wcstombs(_Dest,_Source,_Dsize);
	string result = _Dest;
    delete []_Dest;

	return result;
}

wstring TokenUtil::s2ws(const string& s)
{
	const char* _Source = s.c_str();
    size_t _Dsize = s.size() + 1;
    wchar_t *_Dest = new wchar_t[_Dsize];
    wmemset(_Dest, 0, _Dsize);
    mbstowcs(_Dest,_Source,_Dsize);
    wstring result = _Dest;
    delete []_Dest;

	return result;
}

wchar_t* TokenUtil::s2wchar(const string& s)
{
	const char* _Source = s.c_str();
    size_t _Dsize = s.size() + 1;
    wchar_t *_Dest = new wchar_t[_Dsize];
    wmemset(_Dest, 0, _Dsize);
    mbstowcs(_Dest,_Source,_Dsize);
    //wstring result = _Dest;
    //delete []_Dest;

	return _Dest;
}
