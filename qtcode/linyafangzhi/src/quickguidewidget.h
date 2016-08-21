/****************************************************************************
**
** Copyright (C) 2010-2011 Kazo Vision. (http://www.kazovision.com)
** All rights reserved.
**
****************************************************************************/

#ifndef quickguidewidget_h
#define quickguidewidget_h

#include <QtCore/QVariant>
#include <QtGui/QWidget>

class MainWindow;

/*! \brief QuickGuideWidget
 	\author tony (tonixinot@gmail.com)
 	\version 0.01
 	\date 2011.01.19
 	
	ͨ�����������ϵ�����ʾ��Ϣ���ﵽ����ʹ��ָ����Ч����
*/
class QuickGuideWidget : public QWidget
{
	Q_OBJECT

public:
	QuickGuideWidget(QWidget *parent, MainWindow *pMainWindow);

	~QuickGuideWidget();

protected:
	virtual void mousePressEvent(QMouseEvent *event);

	virtual void paintEvent(QPaintEvent *event);

	void PaintBubble(QPainter &pPainter, const QPoint &pStartPos, const QPoint &pEndPos, int pWidth, const QString &pContents);

private:
	MainWindow *mMainWindow;

};

#endif //quickguidewidget_h
