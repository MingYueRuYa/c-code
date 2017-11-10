/****************************************************************************
**
** Copyright (C) 2017 �Ϻ���������Ƽ����޹�˾. (http://www.2144.cn)
** All rights reserved.
**
****************************************************************************/

#ifndef basethread_h
#define basethread_h

#include <windows.h>

/*! \brief BaseThread
 	\author liushixiong (liushixiong@2144.cn)
 	\version 0.01
 	\date 2017/04/25
	
	�̻߳��ֻ࣬���������ʹ�ã��д�����
*/
class BaseThread
{
public:
	static DWORD WINAPI ThreadProcess(LPVOID p_lpParameter);

public:
	BaseThread();

	virtual ~BaseThread();

	bool GetIsStop();
	
	void Start();

	void Stop();

	void SetExtraParameter(LPVOID p_lpParameter);
	LPVOID GetExtraParameter();

	virtual void Run();

protected:
	void *m_Parameter;

	bool m_IsStop;

	DWORD m_ThreadID;

	HANDLE m_ThreadHandle;

	LPVOID m_lpExtraParameter;

};

#endif //basethread_h