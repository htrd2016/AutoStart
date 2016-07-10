#pragma once
#include <string>
using namespace std;

namespace Utils
{
	void trim(char *pData);
	wstring stringTowstring(const string &strSrc);
	string wstringTostring(const wstring &wstrSrc);
	bool killProcessFromName(const wstring &strProcessName);
};

