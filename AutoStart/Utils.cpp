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
//结束进程
//kill进程from名字
bool Utils::killProcessFromName(const string &strProcessName)
{
	bool bRet = FALSE;
    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);
    if (!Process32First(hSnapShot, &pe))
    {
        return bRet;
    }
	string strProcessNameTemp = strProcessName;
	transform(strProcessNameTemp.begin(), strProcessNameTemp.end(), strProcessNameTemp.begin(), ::tolower);

    while (Process32Next(hSnapShot, &pe))
    {
        string scTmp = pe.szExeFile;
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


