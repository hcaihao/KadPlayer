#ifndef FEEDBACKWINDOW_H
#define FEEDBACKWINDOW_H

#include "basewindow.h"

class TitleBar;
class Content;
class StatusBar;

class FeedbackWindow : public BaseWindow
{
	Q_OBJECT

public:
    FeedbackWindow(QFrame *parent = 0);
    ~FeedbackWindow();

	TitleBar* GetTitleBar();
	Content* GetContent();
	StatusBar* GetStatusBar();

	void SetInfo(int feedbackType, QString content);
protected:
	void paintEvent(QPaintEvent *event);
	bool eventFilter(QObject *obj, QEvent *event);
	bool nativeEvent(const QByteArray &eventType, void *message, long *result);
private:
	void InitControl();
	
	TitleBar *m_pTitleBar;
	Content *m_pContent;
	StatusBar *m_pStatusBar;

	QPlainTextEdit *m_pFeedbackContentPlainTextEdit;
	QButtonGroup *m_pFeedbackTypeButtonGroup;

	QPushButton *m_pOKButton;
	QPushButton *m_pCancelBbutton;
private slots:
	void on_feedbackTypeButtonGroup_buttonClicked(int id);
	void on_okButton_cliecked();
	void on_cancelButton_cliecked();
signals:
};

#endif // FEEDBACKWINDOW_H
