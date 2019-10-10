// IPC.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include <Windows.h>

#define MANUAL_RESET 1

void test_create_event()
{
    HANDLE event_handle = CreateEvent(NULL,     // ��ȫ������
#ifdef MANUAL_RESET
                                        true, 
#else 
                                        false,
#endif // MANUAL_RESET
                                        false, // �տ�ʼ�Ƿ�����ź�falseû��
                                        NULL);
    if (INVALID_HANDLE_VALUE == event_handle) { return; }

    DWORD resultcode = 0;
    while (1) {

        resultcode = WaitForSingleObject(event_handle, 1000*10);

        switch(resultcode) {
            case WAIT_OBJECT_0:
                OutputDebugStringA("WAIT_OBJECT_0\r\n");
#ifndef MANUAL_RESET
                ResetEvent(event_handle);
#endif // MANUAL_RESET
                break;
            case WAIT_TIMEOUT:
            {
                static int i = 0;
                if (i++ % 2) {
                    SetEvent(event_handle);
                }
                OutputDebugStringA("WAIT_TIMEOUT\r\n");
            }
                break;
            case WAIT_FAILED:
                OutputDebugStringA("WAIT_FAILED\r\n");
                break;
        }

    }

}


int _tmain(int argc, _TCHAR* argv[])
{
    test_create_event();

	return 0;
}

