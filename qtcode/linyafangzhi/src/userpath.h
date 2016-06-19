/****************************************************************
 **
 **	Copyright (C) 2016 635672377@qq.com
 ** All rights reserved.
 **
 ***************************************************************/

#ifndef userpath_h

#define userpath_h

#include <QtCore/QVariant>
#include <QtCore/QObject>
#include <QtCore/QDir>

/*! \brief common::UserPath
 	\author tony (tonixinot@gmail.com)
 	\version 0.01
 	\date 2008.08.13
	
	�û�Ŀ¼����WindowsXPϵͳ���û�Ŀ¼Ϊ��C:/Documents and Settings/Username/PVPLAYER/
*/
class UserPath : public QObject
{
	Q_OBJECT

public:
	/*!
		ɾ��Ŀ¼�Լ�����Ŀ¼��
	*/
	static bool RemovePath(const QDir &pPath);

	/*!
		���Ŀ¼�Լ�ɾ����Ŀ¼��
	*/
	static bool ClearPath(const QDir &pPath);

	/*!
		����Ŀ¼�Լ�����Ŀ¼�µ������ļ���
	*/
	static bool CopyPath(const QDir &pSrcPath, const QDir &pDesPath);

	/*!
		�ƶ�Ŀ¼�Լ�����Ŀ¼�µ������ļ���Ŀǰ�ú���������ԭĿ¼���µ�������Ŀ¼�С�
	*/
	static bool MovePath(const QDir &pSrcPath, const QDir &pDesPath);

	/*!
		���캯����ͬʱ�����û�Ŀ¼�Ƿ���ڣ���������ʱ�Զ�������Ŀ¼��
	*/
	UserPath();

	/*!
		����������
	*/
	~UserPath();

	/*!
		��ȡ�û�Ŀ¼��
	*/
	QDir GetUserPath();

	/*!
		���ָ�����û���Ŀ¼�Ƿ���ڡ�
	*/
	bool IsUserSubPathExists(const QString &pSubPathName);

	/*!
		��ȡ�û�Ŀ¼�µ�ĳ����Ŀ¼�����ƣ�����Ŀ¼������ʱ���Զ�������
	*/
	QDir GetUserSubPath(const QString &pSubPathName, bool pCreatePath = true);

	/*!
		ɾ���û�Ŀ¼�µ�ָ����Ŀ¼��
	*/
	bool RemoveUserSubPath(const QString &pSubPathName);

	/*!
		��ȡϵͳĿ¼���������װ��Ŀ¼��
	*/
	QDir GetSystemPath();

	/*!
		��ȡϵͳĿ¼�µ�ĳ����Ŀ¼�����ơ�
	*/
	QDir GetSystemSubPath(const QString &pSubPathName);

private:
	QDir mUserPath;

	QDir mSystemPath;

};

#endif //userpath_h
