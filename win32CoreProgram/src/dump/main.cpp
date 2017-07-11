#include <windows.h>  
#include <DbgHelp.h>  
#include <tchar.h>

#pragma comment(lib, "dbghelp.lib")  

//���ǵĻص�����  
LONG __stdcall ExceptCallBack(EXCEPTION_POINTERS *pExcPointer)
{
    MessageBox(NULL, _T("��������������Ϣ��¼��D:\\Test.dmp�ļ��С�"), NULL, MB_OK);

    //����dump�ļ�  
    HANDLE hFile = CreateFile(_T("D:\\Test.dmp"), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

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

int _tmain(int argc, _TCHAR* argv[])
{
    //���ñ����ص�����  
    SetUnhandledExceptionFilter(ExceptCallBack);

    WrongFun();

    return 0;
}