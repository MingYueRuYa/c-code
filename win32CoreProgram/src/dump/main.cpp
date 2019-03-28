#include <windows.h>  
#include <DbgHelp.h>  
#include <tchar.h>

#include <string>

using std::wstring;

#include "Win7RunTool.h"

#pragma comment(lib, "dbghelp.lib")  

//���ǵĻص�����  
LONG __stdcall ExceptCallBack(EXCEPTION_POINTERS *pExcPointer)
{
    MessageBox(NULL, _T("��������������Ϣ��¼��E:\\Test.dmp�ļ��С�"), NULL, MB_OK);

    //����dump�ļ�  
    HANDLE hFile = CreateFile(_T("E:\\Test.dmp"), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    //���ļ�д�µ�ǰ������������Ϣ  
    MINIDUMP_EXCEPTION_INFORMATION loExceptionInfo;
    loExceptionInfo.ExceptionPointers = pExcPointer;
    loExceptionInfo.ThreadId = GetCurrentThreadId();
    loExceptionInfo.ClientPointers = TRUE;
    MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &loExceptionInfo, NULL, NULL);
    CloseHandle(hFile);

    return EXCEPTION_EXECUTE_HANDLER;
}

void WrongFun()
{
    //crash  
    int * p = NULL;
    *p = 1;
}

BOOL AddScheduledTask(HWND hwnd, LPCTSTR pszPath, LPCWSTR szTaskName, LPCWSTR szTaskDescription, BOOL bIsCheck , LPCTSTR pszParameters , LPCTSTR pszDirectory , BOOL bDeleteTask, BOOL bIsAdmin, LPCTSTR szWorkDir)
{
	CoInitialize(NULL);
	BOOL bRet = AddSystemTask(hwnd, pszPath, szTaskName, szTaskDescription, bIsCheck, pszParameters, pszDirectory, bDeleteTask, bIsAdmin, szWorkDir);
	CoUninitialize();
	return bRet;
}

int _tmain01(int argc, _TCHAR* argv[])
{
    //���ñ����ص�����  
//    SetUnhandledExceptionFilter(ExceptCallBack);
//
//    WrongFun();
//	AddScheduledTask(NULL, _T("c:\\sendhttp\\sendhttp.exe"), _T("Demo"), L"�����س�����Ƽ����޹�˾ copyright@2019", FALSE, NULL, NULL, FALSE, false, L"c:\\sendhttp");



	HMODULE hm = LoadLibraryA("HardwareSerialNumber.dll");
	if (NULL == hm) {
		return 0;
	}

	typedef wstring (*FUN)(void);
	FUN fun = (FUN)GetProcAddress(hm, "GetMacValueEx");
	DWORD error = GetLastError();
	fun();

    return 0;
}

/*
	Processauthority.cpp
	Author��3gstudent@3gstudent
	Look through all the process and detect whether the process runs as admin
*/

#include <windows.h>  
#include <TlHelp32.h>  
BOOL IsRunasAdmin(HANDLE hProcess)  
{  
    BOOL bElevated = FALSE;    
    HANDLE hToken = NULL;    
    if (!OpenProcessToken(hProcess,TOKEN_QUERY,&hToken))  
        return FALSE;   
    TOKEN_ELEVATION tokenEle;  
    DWORD dwRetLen = 0;    
    if ( GetTokenInformation(hToken,TokenElevation,&tokenEle,sizeof(tokenEle),&dwRetLen))  
    {    
        if (dwRetLen == sizeof(tokenEle))  
        {  
            bElevated = tokenEle.TokenIsElevated;    
        }  
    }    
    CloseHandle(hToken);    
    return bElevated;    
}  

int _tmain(int argc, _TCHAR* argv[])
{  

//	DeleteTaskForWin7(L"aaaa");

	AddScheduledTask(NULL, _T("c:\\sendhttp\\sendhttp.exe"), _T("Demo"), L"�����س�����Ƽ����޹�˾ copyright@2019", FALSE, NULL, NULL, true, false, L"c:\\sendhttp");

	return 0;

	SYSTEMTIME wtm;
    GetLocalTime(&wtm);

	PROCESSENTRY32 pinfo; 
	HANDLE hProcess,hModule;
	BOOL bRunAsAdmin;
	hModule = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);  
	BOOL report = Process32First(hModule, &pinfo);  
	printf("\n%-20s	 PID	Run as Admin\n","Process");
	printf("====================	====	============\n");
	while(report) 
	{ 
		printf("%-20s	%4d	",pinfo.szExeFile,pinfo.th32ProcessID); 
		hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION,FALSE,pinfo.th32ProcessID);
		bRunAsAdmin = IsRunasAdmin(hProcess);  
		bRunAsAdmin = true;
		if (bRunAsAdmin)  
			printf("%-12s\n","Yes");  
		else
			printf("\n");
		report=Process32Next(hModule, &pinfo);    
	}
	CloseHandle(hModule);  

	return 0;  
}  