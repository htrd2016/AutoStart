// AutoStart.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>
#include <Iphlpapi.h>
#include "Utils.h"

bool exeCmd(char *cmd, char **out, size_t maxLen);
bool getComputerName(char **out, size_t maxLen);
bool setIP(char *ip, char *mask, char *gateway, char* name);
bool setComputerName(char *newName);
void reboot();
bool runClient(char *folder, char *serverIP, char *serverPort);


static char* getMAC(){
	PIP_ADAPTER_INFO AdapterInfo;
	DWORD dwBufLen = sizeof(AdapterInfo);
	char *mac_addr = (char*)malloc(17);

	AdapterInfo = (IP_ADAPTER_INFO *)malloc(sizeof(IP_ADAPTER_INFO));
	if (AdapterInfo == NULL) {
		printf("Error allocating memory needed to call GetAdaptersinfo\n");

	}

	// Make an initial call to GetAdaptersInfo to get the necessary size into the dwBufLen     variable
	if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == ERROR_BUFFER_OVERFLOW) {

		AdapterInfo = (IP_ADAPTER_INFO *)malloc(dwBufLen);
		if (AdapterInfo == NULL) {
			printf("Error allocating memory needed to call GetAdaptersinfo\n");
		}
	}

	if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == NO_ERROR) {
		PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;// Contains pointer to current adapter info
		do {
			_snprintf_s(mac_addr, 17, _TRUNCATE, "%02X%02X%02X%02X%02X%02X",
				pAdapterInfo->Address[0], pAdapterInfo->Address[1],
				pAdapterInfo->Address[2], pAdapterInfo->Address[3],
				pAdapterInfo->Address[4], pAdapterInfo->Address[5]);
			//printf("Address: %s, mac: %s\n", pAdapterInfo->IpAddressList.IpAddress.String, mac_addr);
			return mac_addr;

			//printf("\n");
			pAdapterInfo = pAdapterInfo->Next;
		} while (pAdapterInfo);
	}
	free(AdapterInfo);


	return 0;
}

int main(int argc, char* argv[])
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

	char* mac = getMAC();
	strcpy_s(minion_name, 128, mac);
	free(mac);
	strcpy_s(ini_file, 512, "C:\\AutoStart\\config.ini");

    DWORD len = GetPrivateProfileString(minion_name, "hostname", "", hostName, 512, ini_file);
	if(len<=0)
	{
		printf("len=%d, hostname=%s %d\n", len, hostName, GetLastError());
		return -1;
	}

	len = GetPrivateProfileString(minion_name, "ip", "", ip, 64, ini_file);
	if(len<=0)
	{
		printf("len=%d, ip=%s %d\n", len, ip, GetLastError());
		return -1;
	}

	len = GetPrivateProfileString(minion_name, "mask", "", mask, 64, ini_file);
	if(len<=0)
	{
		printf("len=%d, %s %d\n", len, mask, GetLastError());
		return -1;
	}

	len = GetPrivateProfileString(minion_name, "gateway", "", gateway, 64, ini_file);
    /*if(len<=0)
	{
		printf("len=%d, %s %d\n", len, gateway, GetLastError());
		return -1;
	}*/

	len = GetPrivateProfileString(minion_name, "dns", "", dns, 64, ini_file);
    /*if(len<=0)
	{
		printf("len=%d, %s %d\n", len, dns, GetLastError());
		return -1;
	}*/

	len = GetPrivateProfileString(minion_name, "to_run_app_count", "", to_run_app_count, 10, ini_file);
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


	printf("mac %s, hostname %s, ip %s, mask %s, gateway %s, dns %s, app %d\n",
		minion_name, hostName, ip, mask, gateway, dns, int_to_run_app_count);

	if(false == setIP(ip, mask, gateway, local_name))
	{
		printf("error:set ip failed!!!");
		return -1;
	}

	char cmd[1024];
	_snprintf_s(cmd, 1024, _TRUNCATE,
		"netsh interface IP set dns \"%s\" static %s",
		local_name, dns);
	
	char out[1024];
	DWORD dwlen = sizeof(out);
	if (false == exeCmd(cmd, (char**)(&out), sizeof(out)))
	{
		char err[1024] = "error:";
		strcat_s(err, 1024, cmd);
		printf("%s", err);
		return -1;
	}
	
	if(TRUE == GetComputerName(out, &dwlen))
	{
		printf("Host Name is %s and %s\n", out, hostName);
		if(_stricmp(out, hostName) != 0)
		{
			if(true == setComputerName(hostName))
			{
				printf("about to reboot!!");
				reboot(); //重启
				return 0;
			}
			else
			{
				printf("error:set computer name failed!!");
				return -1;
			}
		}
	}
	//printf("%s", out);

	Utils::killProcessFromName("Guardian.exe");
	Utils::killProcessFromName("Client.exe");

	if ((strncmp(hostName, "new", 3) == 0) || (strncmp(hostName, "NEW", 3) == 0))
	{
		strcpy_s(main_selection, 128, "MainNew");
	}
	else
	{
		strcpy_s(main_selection, 128, "MainOld");
	}

	printf("main_selection %s\n", main_selection);

	len = GetPrivateProfileString(main_selection, "host", "", server_ip, 64, ini_file);
    printf("len=%d, ip: %s %d\n", len, server_ip, GetLastError());

	len = GetPrivateProfileString(main_selection, "port", "", server_port, 20, ini_file);
    printf("len=%d, port: %s %d\n", len, server_port, GetLastError());

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
		Sleep(10000);
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
	if(FALSE == WritePrivateProfileString("Main", "host", serverIP, webIni))
	{
		printf("error:write %s", webIni);
		return false;
	}
	if(FALSE == WritePrivateProfileString("Main", "port", serverPort, webIni))
	{
		printf("error:write %s", webIni);
		return false;
	}

	if(FALSE == ::DeleteFile(runFile))
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

//bool getComputerName(char **out, size_t maxLen)
//{
//	bool bRet = exeCmd("hostname", out, maxLen);
//	char *p = (char*)out;
//	return bRet;
//}

bool setIP(char *ip, char *mask, char *gateway, char* name)
{
	char szGateWay[512] = {0};
	strcpy_s(szGateWay, 512, gateway );
	Utils::trim(szGateWay);
	if(strlen(szGateWay) == 0)
		strcpy_s(szGateWay, 512, "none");
	else
		_snprintf_s(szGateWay, 512, "gateway= %s gwmetric=1", szGateWay);

	//param = 'netsh interface ip set address name="'+name+'" source=static addr='+ ip +' mask='+mask+' ' + gateway
	char cmd[1024] = {0};
	_snprintf_s(cmd, 1024, _TRUNCATE, "netsh interface ip set address name=\"%s\""
		" source=static addr=\"%s\""
		" mask=\"%s\""
		" %s", name, ip, mask, szGateWay);

	printf("SetIP: %s\n", cmd);

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
	char command[1024];    //长达1K的命令行，够用了吧  
	strcpy_s(command,1024,"cmd.exe /C ");  
	strcat_s(command,1024,cmd);  
	STARTUPINFOA si;  
	PROCESS_INFORMATION pi;   
	si.cb = sizeof(STARTUPINFO);  
	GetStartupInfoA(&si);   
	si.hStdError = hWrite;            //把创建进程的标准错误输出重定向到管道输入  
	si.hStdOutput = hWrite;           //把创建进程的标准输出重定向到管道输入  
	si.wShowWindow = SW_HIDE;  
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;  
	//关键步骤，CreateProcess函数参数意义请查阅MSDN  
	if (!CreateProcessA(NULL, command,NULL,NULL,TRUE,NULL,NULL,NULL,&si,&pi))   
	{  
		CloseHandle(hWrite);  
		CloseHandle(hRead);  
		return FALSE;  
	}  
	CloseHandle(hWrite);  
	//char buffer[4096] = {0};          //用4K的空间来存储输出的内容，只要不是显示文件内容，一般情况下是够用了。  
	DWORD bytesRead;   
	int nLen = 0;
	while (true)   
	{  
		if (ReadFile(hRead,out+nLen,maxLen-nLen,&bytesRead,NULL) == NULL)  
			break;
		nLen += bytesRead;
	}  
	
	CloseHandle(hRead);   
	return true;  
}  

void reboot()
{
	OSVERSIONINFO osv;
    osv.dwOSVersionInfoSize = sizeof OSVERSIONINFO;
    GetVersionEx(&osv);//获取操作系统的版本
    if(VER_PLATFORM_WIN32_NT == osv.dwPlatformId)
    {
        // 如果运行在NT/XP平台，必须先取得权限
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
    ExitWindowsEx(EWX_REBOOT,0);//重新启动计算机
}
