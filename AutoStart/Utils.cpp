#include "StdAfx.h"
#include "Utils.h"
#include <string.h>
#include <Windows.h>
#include<tlhelp32.h>
#include <algorithm>

void Utils::trim(char * str)
{
    int i , len ;

    //先去除左边的空格
    len = strlen(str) ;
    for( i=0; i<len; i++)
    {
        if( str[i] != ' ') break ;
    }
    memmove(str,str+i,len-i+1);

    //再去除右边的空格
    len = strlen( str ) ;
    for(i = len-1; i>=0; i--)
    {
        if(str[i] != ' ') break ;
    }
    str[i+1] = 0 ;
}

wstring Utils::stringTowstring(const string &strSrc)
{
	wstring wstr = _T("");
	int nLen = (int)strSrc.length();    
	wstr.resize(nLen, _T(' '));

	int nResult = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)strSrc.c_str(), nLen, (LPWSTR)wstr.c_str(), nLen);
	if (nResult == 0)
	{
		return _T("");
	}

	wstring strRet = wstr;
	wstr.erase(wstr.begin(),wstr.end());
	return strRet;
}

string Utils::wstringTostring(const wstring &wstrSrc)
{
	size_t size;
	string curLocale = setlocale(LC_ALL, NULL);
	setlocale(LC_ALL, "chs"); 
	const wchar_t* _Source = wstrSrc.c_str();
	size_t _Dsize = 2 * wstrSrc.size() + 1;
	char *_Dest = new char[_Dsize];
	memset(_Dest, 0, _Dsize);
	wcstombs_s(&size,_Dest, _Dsize, _Source, _Dsize);
	std::string result = _Dest;
	delete []_Dest;
	setlocale(LC_ALL, curLocale.c_str());
	return result;
}

//结束进程
//kill进程from名字
bool Utils::killProcessFromName(const wstring &strProcessName)
{
	bool bRet = FALSE;
    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);
    if (!Process32First(hSnapShot, &pe))
    {
        return bRet;
    }
	wstring strProcessNameTemp = strProcessName;
	transform(strProcessNameTemp.begin(), strProcessNameTemp.end(), strProcessNameTemp.begin(), ::tolower);

    while (Process32Next(hSnapShot, &pe))
    {
        wstring scTmp = pe.szExeFile;
		transform(scTmp.begin(), scTmp.end(), scTmp.begin(), ::tolower);
		if (!scTmp.compare(strProcessNameTemp))
        {
            DWORD dwProcessID = pe.th32ProcessID;
            HANDLE hProcess = ::OpenProcess(PROCESS_TERMINATE,FALSE, dwProcessID);
            ::TerminateProcess(hProcess, 0);
            CloseHandle(hProcess);
            bRet = TRUE;
        }
    }
    return bRet;
}


