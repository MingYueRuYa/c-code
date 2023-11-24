/****************************************************************************
**
** Copyright (C) 2018 635672377@qq.com
** All rights reserved.
**
****************************************************************************/

#include "thread_start.h"
#include "thread_mutex.h"
#include "SyncQueue.hpp"
#include "counter.hpp"
#include "thread_get_retvalue.h"
#include "thread_semphore.hpp"
#include "thread_pool.hpp"

#include <Windows.h>

#include <Windows.h>
#include <UserEnv.h>

#pragma comment(lib, "Userenv.lib")

int main()
{
  HANDLE hToken;
  HANDLE hNewToken;
  LPVOID lpEnvironment;

  // ��ȡ��ǰ���̵�����
  if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken))
  {
    printf("�޷��򿪽������ơ������룺%d\n", GetLastError());
    return 1;
  }

  // ��ȡ��ǰ�û�������
  if (!DuplicateTokenEx(hToken, MAXIMUM_ALLOWED, NULL, SecurityIdentification, TokenPrimary, &hNewToken))
  {
    printf("�޷��������ơ������룺%d\n", GetLastError());
    CloseHandle(hToken);
    return 1;
  }

  // ��ȡ��ǰ�û��Ļ�������
  if (!CreateEnvironmentBlock(&lpEnvironment, hNewToken, TRUE))
  {
    printf("�޷������û������顣�����룺%d\n", GetLastError());
    CloseHandle(hToken);
    CloseHandle(hNewToken);
    return 1;
  }

  // �����½���
  PROCESS_INFORMATION pi;
  STARTUPINFO si;
  ZeroMemory(&si, sizeof(STARTUPINFO));
  si.cb = sizeof(STARTUPINFO);
  if (!CreateProcessAsUser(hNewToken, L"notepad.exe", NULL, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, lpEnvironment, NULL, &si, &pi))
  {
    printf("�޷������½��̡������룺%d\n", GetLastError());
    CloseHandle(hToken);
    CloseHandle(hNewToken);
    DestroyEnvironmentBlock(lpEnvironment);
    return 1;
  }

  printf("�½����Ѵ�����\n");

  // �رվ��
  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);
  CloseHandle(hToken);
  CloseHandle(hNewToken);
  DestroyEnvironmentBlock(lpEnvironment);

  return 0;
}

int main02(int argc, char **argv)
{
	// thread_start();
	// thread_deatch(1, "liushixiong");
	// thread_move();
	// thread_bind();
	// thread_copy();

	// TestMutex();
	// TestRecursiveMutex();
	// TestTimeoutMutex();

	// TestSyncQueue();

	// TestMutexCounter();
	// TestAtomicCounter();

	// TestFuture();
	// TestAsync();

    /*
    thread *t1 = new thread([] () { 

        std::chrono::seconds sleepDuration(10);

        std::this_thread::sleep_for(sleepDuration); 
    });

    
    ::TerminateThread(t1->native_handle(), 0);
	system("pause");
    */

    // mutex test_mutex;

    // unique_lock<mutex> lock(test_mutex);

    // bool owns = lock.owns_lock();

    // lock.unlock();

    // owns = lock.owns_lock();
    // if (owns) {
    //     lock.unlock();
    // }


    // Thread::test_thread_seamphore();
    
    // ThreadPool::test_any();

    ThreadPool::test_thread_pool();

    getchar();

	return 0;
}
