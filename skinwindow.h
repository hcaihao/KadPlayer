#ifndef SKINWINDOW_H
#define SKINWINDOW_H

#include "basewindow.h"

class TitleBar;
class Content;
class StatusBar;
class ChangeSkinWidget;

class SkinWindow : public BaseWindow
{
	Q_OBJECT

public:
    SkinWindow(QFrame *parent = 0);
    ~SkinWindow();

	TitleBar* GetTitleBar();
	Content* GetContent();
	StatusBar* GetStatusBar();
protected:
	void paintEvent(QPaintEvent *event);
	bool eventFilter(QObject *obj, QEvent *event);
	bool nativeEvent(const QByteArray &eventType, void *message, long *result);
private:
	void InitControl();
	
	TitleBar *m_pTitleBar;
	Content *m_pContent;
	StatusBar *m_pStatusBar;


	int m_pageCount;	//总页数
	int m_lastPageCount;	//最后一页个数
	int m_currentPage;	//当前页数

	QPushButton *m_pFirstPagePushButton;
	QPushButton *m_pPreviousPagePushButton;
	QPushButton *m_pNextPagePushButton;
	QPushButton *m_pLastPagePushButton;

	QStringList m_skinNameList;
	QStringList m_skinTipList;

	QList<ChangeSkinWidget *> m_changeSkinWidgetList;
private slots:
	void showSkin(QString currentPage);
	void useSkin(QString skinName);
signals:
	void changeSkin();
};

#endif // SKINWINDOW_H
