/****************************************************************
 **
 **	Copyright (C) 2016 635672377@qq.com
 ** All rights reserved.
 **
 ***************************************************************/

#include <QtGui/QApplication>

#include "smemainwindow.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	//��ʼ����Դ��
	//qInitResources();
	smeMainWindow mainwindow;
	mainwindow.show();
	return app.exec();
}