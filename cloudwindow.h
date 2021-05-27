#ifndef CLOUDWINDOW_H
#define CLOUDWINDOW_H

#include "basewindow.h"

class TitleBar;
class Content;
class StatusBar;
class WebAxWidget;

class CloudWindow : public BaseWindow
{
	Q_OBJECT

public:
    CloudWindow(QFrame *parent = 0);
    ~CloudWindow();

	TitleBar* GetTitleBar();
	Content* GetContent();
	StatusBar* GetStatusBar();
protected:
	void paintEvent(QPaintEvent *event);
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
	void stop();
signals:

};

#endif // CLOUDWINDOW_H
