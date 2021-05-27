#ifndef PLAYWINDOW_H
#define PLAYWINDOW_H

#include "basewindow.h"

class TitleBar;
class Content;
class StatusBar;
class WebAxWidget;

class PlayWindow : public BaseWindow
{
	Q_OBJECT

public:
    PlayWindow(QFrame *parent = 0);
    ~PlayWindow();

	TitleBar* GetTitleBar();
	Content* GetContent();
	StatusBar* GetStatusBar();

	void Play(QString title, QString link);
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

	bool m_isPlaying;
	QString m_link;
	WebAxWidget *m_pWebAxWidget;
private slots:
	void DocumentComplete(IDispatch* pDisp, QVariant& URL);
	void stop();
signals:

};

#endif // PLAYWINDOW_H
