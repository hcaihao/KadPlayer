#ifndef SETTINGWINDOW_H
#define SETTINGWINDOW_H

#include "basewindow.h"

class TitleBar;
class Content;
class StatusBar;

class SettingWindow : public BaseWindow
{
	Q_OBJECT

public:
    SettingWindow(QFrame *parent = 0);
    ~SettingWindow();

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

	QCheckBox *m_pAutoStartCheckBox;
	QCheckBox *m_pSaveHistoryCheckBox;
	QButtonGroup *m_pChannelButtonGroup;

	QRadioButton *m_pClickPlayRadioButton;
	QRadioButton *m_pClickDownloadRadioButton;
	QButtonGroup *m_pClickButtonGroup;

	QPushButton *m_pOKButton;
	QPushButton *m_pCancelBbutton;
private slots:
	void on_okButton_cliecked();
	void on_cancelButton_cliecked();
signals:

};

#endif // SETTINGWINDOW_H
