/****************************************************************************
**
** Copyright (C) 2017 �Ϻ���������Ƽ����޹�˾. (http://www.2144.cn)
** All rights reserved.
**
****************************************************************************/

#ifndef statistic_jzzmstatsthread_h
#define statistic_jzzmstatsthread_h

#include <Windows.h>
#include <tchar.h>
#include <list>

#include "basethread.h"

using std::list;
using std::wstring;

/*! \brief jzzmstatsthread 
 	\author liushixiong (liushixiong@2144.cn)
 	\version 0.01
 	\date 2017/06/16
	
	��������ͳ���߳�
*/
class jzzmStatsThread : public BaseThread
{
public:
	jzzmStatsThread();

	~jzzmStatsThread();

	void Apppend(const wstring pStrUrl);

	virtual void Run();

private:
	list<wstring> mStatsCommandList;

	HANDLE mMutexHandle;

	HANDLE mSemaphoreHandle;

	CRITICAL_SECTION mCriticalSection;

};

#endif //statistic_jzzmstatsthread_h