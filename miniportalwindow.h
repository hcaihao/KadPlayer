#ifndef MINIPORTALWINDOW_H
#define MINIPORTALWINDOW_H

#include "basewindow.h"

class TitleBar;
class Content;
class StatusBar;
class WebAxWidget;

class MiniPortalWindow : public BaseWindow
{
	Q_OBJECT

public:
    MiniPortalWindow(QFrame *parent = 0);
    ~MiniPortalWindow();

	TitleBar* GetTitleBar();
	Content* GetContent();
	StatusBar* GetStatusBar();
protected:
	void paintEvent(QPaintEvent *event);
	bool eventFilter(QObject *obj, QEvent *event);
	bool nativeEvent(const QByteArray &eventType, void *message, long *result);
	void moveEvent(QMoveEvent * event);
	void resizeEvent(QResizeEvent *event);
	void showEvent(QShowEvent *event);
	void hideEvent(QHideEvent *event);
private:
	void InitControl();
	
	TitleBar *m_pTitleBar;
	Content *m_pContent;
	StatusBar *m_pStatusBar;

	WebAxWidget *m_pWebAxWidget;
private slots:
signals:

};

#endif // MINIPORTALWINDOW_H
