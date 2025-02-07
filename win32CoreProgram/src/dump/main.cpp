#include <windows.h>  
#include <DbgHelp.h>  
#include <tchar.h>

#include <string>

using std::wstring;

#include "Win7RunTool.h"

#pragma comment(lib, "dbghelp.lib")  
#pragma comment(lib, "exception_handler.lib")  
#pragma comment(lib, "common.lib")  
#pragma comment(lib, "crash_generation_client.lib")  

//我们的回调函数  
LONG __stdcall ExceptCallBack(EXCEPTION_POINTERS *pExcPointer)
{
    MessageBox(NULL, _T("程序崩溃！相关信息记录在E:\\Test.dmp文件中。"), NULL, MB_OK);

    //创建dump文件  
    HANDLE hFile = CreateFile(_T("E:\\Test.dmp"), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    //向文件写下当前程序崩溃相关信息  
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
    //设置崩溃回调函数  
//    SetUnhandledExceptionFilter(ExceptCallBack);
//
//    WrongFun();
//	AddScheduledTask(NULL, _T("c:\\sendhttp\\sendhttp.exe"), _T("Demo"), L"重庆重橙网络科技有限公司 copyright@2019", FALSE, NULL, NULL, FALSE, false, L"c:\\sendhttp");



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
	Author：3gstudent@3gstudent
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

int _tmain03(int argc, _TCHAR* argv[])
{  

//	DeleteTaskForWin7(L"aaaa");

	AddScheduledTask(NULL, _T("c:\\sendhttp\\sendhttp.exe"), _T("Demo"), L"重庆重橙网络科技有限公司 copyright@2019", FALSE, NULL, NULL, true, false, L"c:\\sendhttp");

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

#include "breakpad/src/client/windows/handler/exception_handler.h"

static bool callback(const wchar_t* dump_path, const wchar_t* id,
	void* context, EXCEPTION_POINTERS* exinfo,
	MDRawAssertionInfo* assertion,
	bool succeeded) {
	if (succeeded) {
		printf("dump guid is %ws\n", id);
	}
	else {
		printf("dump failed\n");
	}
	fflush(stdout);

	return succeeded;
}

void crash()
{
  char szchar[8];
  sprintf(szchar, "%s", "1111111111122222");
}

static void CrashFunction() {
	int* i = reinterpret_cast<int*>(0x45);
	*i = 5;  // crash!
}

int _tmain(int argc, _TCHAR* argv[])
{
	//_CrtSetReportMode(_CRT_ASSERT, 0);
	//_CrtSetReportMode(_CRT_ERROR, 0);
	//_CrtSetReportMode(_CRT_ERRCNT, 0);
	google_breakpad::ExceptionHandler eh(L".",
		NULL,
		callback,
		NULL,
		google_breakpad::ExceptionHandler::HANDLER_ALL);

	// CrashFunction();
	CrashFunction();

	return 0;
}