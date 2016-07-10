// AutoStart.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <Windows.h>
#include "Utils.h"

bool exeCmd(char *cmd, char **out, size_t maxLen);
bool getComputerName(char **out, size_t maxLen);
bool setIP(char *ip, char *mask, char *gateway, char* name);
bool setComputerName(char *newName);
void reboot();
bool runClient(char *folder, char *serverIP, char *serverPort);

void test()
{
	char data[1024] = {"   hello!!!"};
	Utils::trim(data);
	printf("%s", data);
}
int _tmain(int argc, _TCHAR* argv[])
{
	char minion_name[128];
	char ini_file[512] = "";
	char local_name[512] = "Local";
	char hostName[512] = "";
	char ip[64] = "";
	char mask[64] = "";
	char gateway[64] = "";
	char dns[64] = "";
	char to_run_app_count[10] = "";
	int int_to_run_app_count = 0;
	char main_selection[128];
	char server_ip[64] = "";
	char server_port[20] = "";

	//minion_id,config.ini
	if (argc<4)
	{
		printf("<exe path><minion_id><config.ini><new:0,old:1><LocalName=Local>");
		return -1;
	}

	strcpy_s(minion_name, 128, Utils::wstringTostring(argv[1]).c_str());
	strcpy_s(ini_file, 512, Utils::wstringTostring(argv[2]).c_str());

	if (argc>=5)
	{
		strcpy_s(local_name, 512, Utils::wstringTostring(argv[4]).c_str());
	}

    DWORD len = GetPrivateProfileStringA(minion_name, "hostname", "", hostName, 512, ini_file);
	if(len<=0)
	{
		printf("len=%d, hostname=%s %d\n", len, hostName, GetLastError());
		return -1;
	}

	len = GetPrivateProfileStringA(minion_name, "ip", "", ip, 64, ini_file);
	if(len<=0)
	{
		printf("len=%d, ip=%s %d\n", len, ip, GetLastError());
		return -1;
	}

	len = GetPrivateProfileStringA(minion_name, "mask", "", mask, 64, ini_file);
	if(len<=0)
	{
		printf("len=%d, %s %d\n", len, mask, GetLastError());
		return -1;
	}

	len = GetPrivateProfileStringA(minion_name, "gateway", "", gateway, 64, ini_file);
    /*if(len<=0)
	{
		printf("len=%d, %s %d\n", len, gateway, GetLastError());
		return -1;
	}*/

	len = GetPrivateProfileStringA(minion_name, "dns", "", dns, 64, ini_file);
    /*if(len<=0)
	{
		printf("len=%d, %s %d\n", len, dns, GetLastError());
		return -1;
	}*/

	len = GetPrivateProfileStringA(minion_name, "to_run_app_count", "", to_run_app_count, 10, ini_file);
    if(len<0)
	{
		printf("len=%d, %s %d\n", len, to_run_app_count, GetLastError());
		return -1;
	}

	int_to_run_app_count = atoi(to_run_app_count);
	if(int_to_run_app_count<=0)
	{
		printf("error:get int_to_run_app_count(%d)\n",  GetLastError());
		return -1;
	}

	if(false == setIP(ip, mask, gateway, local_name))
	{
		printf("error:set ip failed!!!");
		return -1;
	}

	char cmd[1024] = {"netsh interface IP set dns \""};
	strcat_s(cmd, 1024, local_name);
	strcat_s(cmd, 1024, "\" static ");
	strcat_s(cmd, dns);
	
	char out[1024];
	if (false == exeCmd(cmd, (char**)(&out), sizeof(out)))
	{
		char err[1024] = "error:";
		strcat_s(err, 1024, cmd);
		printf("%s", err);
		return -1;
	}
	
	if(true == getComputerName((char**)(&out), sizeof(out)))
	{
		char hostNameTmp[50] = "";
		strcpy_s(hostNameTmp, 50, hostName);
		strcat_s(hostNameTmp, 50, "\r\n");

		if(strcmp(out, hostName)!=0 && strcmp(out,hostNameTmp)!=0 )
		{
			/*if(true == setComputerName(hostName))
			{
				printf("about to reboot!!");
				reboot(); //����
				return 0;
			}
			else
			{
				printf("error:set computer name failed!!");
				return -1;
			}*/
		}
	}
	//printf("%s", out);

	Utils::killProcessFromName(L"Guardian.exe");
	Utils::killProcessFromName(L"Client.exe");

	if(wcscmp(argv[3] ,L"1") == 0)
	{
		strcpy_s(main_selection, 128, "MainOld");
	}
	else
	{
		strcpy_s(main_selection, 128, "MainNew");
	}

	printf("%s", main_selection);

	len = GetPrivateProfileStringA(main_selection, "host", "", server_ip, 64, ini_file);
    printf("len=%d, %s %d\n", len, server_ip, GetLastError());

	len = GetPrivateProfileStringA(main_selection, "port", "", server_port, 20, ini_file);
    printf("len=%d, %s %d\n", len, server_port, GetLastError());

	for(int i=1;i<=int_to_run_app_count;i++)
	{
		char path[1024] = "C:\\hongt\\Client";
		if(i<=10)
		{
			char szIndex[20];
			_itoa_s(i, szIndex, 10);
			if(i<10)
			{
				strcat_s(path, 1024, "0");
			}
			strcat_s(path, 1024, szIndex);
			strcat_s(path, 1024, "\\");
		}
		runClient(path, server_ip, server_port);
		Sleep(1);
	}

	return 0;
}

bool runClient(char *folder, char *serverIP, char *serverPort)
{
	char webIni[1024] = "";
	char runFile[1024] = "";
	char exeFile[1024] = "";
	char cmd[1024] = "";
	char out[1024] = "";
	strcpy_s(webIni, 1024, folder);
	strcat_s(webIni, 1024, "WebConfig.ini");

	strcpy_s(runFile, 1024, folder);
	strcat_s(runFile, 1024, "Client.run");

	strcpy_s(exeFile, 1024, folder);
	strcat_s(exeFile, 1024, "Client.exe");

	printf("%s\n%s", webIni, runFile);
	if(FALSE == WritePrivateProfileStringA("Main", "host", serverIP, webIni))
	{
		printf("error:write %s", webIni);
		return false;
	}
	if(FALSE == WritePrivateProfileStringA("Main", "port", serverPort, webIni))
	{
		printf("error:write %s", webIni);
		return false;
	}

	if(FALSE == ::DeleteFileA(runFile))
	{
		printf("failed delete %s", runFile);
	}
	return WinExec(exeFile, SW_SHOWNORMAL)>31;
}

bool setComputerName(char *newName)
{
	char out[1024] = "";
	//wmic computersystem where name="%COMPUTERNAME%" call rename name="'+hostname_from_ini+'"
	char cmd[1024] = "wmic computersystem where name=\"%COMPUTERNAME%\" call rename name=\"";
	strcat_s(cmd, 1024, newName);
	strcat_s(cmd, 1024, "\"");

	printf("%s", cmd);

	bool bRet = exeCmd(cmd, (char**)(&out), 1024);
	printf("%s", out);
	return bRet;
}

bool getComputerName(char **out, size_t maxLen)
{
	bool bRet = exeCmd("hostname", out, maxLen);
	char *p = (char*)out;
	return bRet;
}

bool setIP(char *ip, char *mask, char *gateway, char* name)
{
	char szGateWay[512] = {0};
	strcpy_s(szGateWay, 512, gateway );
	Utils::trim(szGateWay);
	if(strlen(szGateWay) == 0)
	{
		strcpy_s(szGateWay, 512, "none");
	}
	else
	{
		strcpy_s(szGateWay, 512, "gateway=");
		strcat_s(szGateWay, 512, gateway);
		strcat_s(szGateWay, 512, " gwmetric=1");
	}

	//param = 'netsh interface ip set address name="'+name+'" source=static addr='+ ip +' mask='+mask+' ' + gateway
	char cmd[1024] = {0};
	strcpy_s(cmd, 1024, "netsh interface ip set address name=\"");
	strcat_s(cmd, 1024, name);
	strcat_s(cmd, 1024, "\" source=static addr=");
	strcat_s(cmd, 1024, ip);
	strcat_s(cmd, 1024, " mask=");
	strcat_s(cmd, 1024, mask);
	strcat_s(cmd, 1024, " ");
	strcat_s(cmd, 1024, szGateWay);

	printf("%s", cmd);

	char out[1024];
	return exeCmd(cmd, (char**)(&out), sizeof(out));
}

bool exeCmd(char *cmd, char** out, size_t maxLen)  
{
	memset(out, 0, maxLen);
	SECURITY_ATTRIBUTES sa;  
	HANDLE hRead,hWrite;  
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);  
	sa.lpSecurityDescriptor = NULL;  
	sa.bInheritHandle = TRUE;  
	if (!CreatePipe(&hRead,&hWrite,&sa,0))   
	{  
		return false;  
	}   
	char command[1024];    //����1K�������У������˰�  
	strcpy_s(command,1024,"cmd.exe /C ");  
	strcat_s(command,1024,cmd);  
	STARTUPINFOA si;  
	PROCESS_INFORMATION pi;   
	si.cb = sizeof(STARTUPINFO);  
	GetStartupInfoA(&si);   
	si.hStdError = hWrite;            //�Ѵ������̵ı�׼��������ض��򵽹ܵ�����  
	si.hStdOutput = hWrite;           //�Ѵ������̵ı�׼����ض��򵽹ܵ�����  
	si.wShowWindow = SW_HIDE;  
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;  
	//�ؼ����裬CreateProcess�����������������MSDN  
	if (!CreateProcessA(NULL, command,NULL,NULL,TRUE,NULL,NULL,NULL,&si,&pi))   
	{  
		CloseHandle(hWrite);  
		CloseHandle(hRead);  
		return FALSE;  
	}  
	CloseHandle(hWrite);  
	//char buffer[4096] = {0};          //��4K�Ŀռ����洢��������ݣ�ֻҪ������ʾ�ļ����ݣ�һ��������ǹ����ˡ�  
	DWORD bytesRead;   
	int nLen = 0;
	while (true)   
	{  
		if (ReadFile(hRead,out+nLen,maxLen-nLen,&bytesRead,NULL) == NULL)  
			break;
	}  
	nLen += bytesRead;
	CloseHandle(hRead);   
	return true;  
}  

void reboot()
{
	OSVERSIONINFO osv;
    osv.dwOSVersionInfoSize = sizeof OSVERSIONINFO;
    GetVersionEx(&osv);//��ȡ����ϵͳ�İ汾
    if(VER_PLATFORM_WIN32_NT == osv.dwPlatformId)
    {
        // ���������NT/XPƽ̨��������ȡ��Ȩ��
        HANDLE hProcess, hToken;
        TOKEN_PRIVILEGES Privileges;
        LUID luid;
        hProcess=GetCurrentProcess();
        OpenProcessToken(hProcess, TOKEN_ADJUST_PRIVILEGES, &hToken);
        Privileges.PrivilegeCount = 1;
        LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &luid);
        Privileges.Privileges[0].Luid = luid;
        Privileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
        AdjustTokenPrivileges(hToken, FALSE, &Privileges, NULL, NULL, NULL);
    }
    ExitWindowsEx(EWX_REBOOT,0);//�������������
}