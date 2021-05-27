#ifndef MESSAGEWINDOW_H
#define MESSAGEWINDOW_H

#include "basewindow.h"

class TitleBar;
class Content;
class StatusBar;

class MessageWindow : public BaseWindow
{
	Q_OBJECT

public:
	static MessageWindow& GetInstance();
	~MessageWindow();

	TitleBar* GetTitleBar();
	Content* GetContent();
	StatusBar* GetStatusBar();

	bool ShowMessage(QString title, QString content, bool isOkCancel = false);
protected:
    void paintEvent(QPaintEvent *event);
	bool eventFilter(QObject *obj, QEvent *event);
	bool nativeEvent(const QByteArray &eventType, void *message, long *result);
	void closeEvent(QCloseEvent * event);
private:
	MessageWindow(QFrame *parent = 0);

	void InitControl();
	void LoadIniData();
	void SaveIniData();
	bool exec();

	TitleBar *m_pTitleBar;
	Content *m_pContent;
	StatusBar *m_pStatusBar;

	QEventLoop m_loop;
	bool m_isAccept;

	QLabel *m_pMessageLabel;
	QLabel *m_pImageLabel;

	QPushButton *m_pOKButton;
	QPushButton *m_pCancelBbutton;
private slots:
    void accept();
    void reject();
signals:
	void okMessageHidden(bool is_checked);
	void msgChecked(bool is_checked, bool is_ok);
};

#endif  //MESSAGEWINDOW_H