#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "basewindow.h"

class TitleBar;
class Content;
class StatusBar;
class SkinWindow;
class MiniPortalWindow;
class CloudWindow;
class FeedbackWindow;
class SystemTray;
class ResultTabWidget;
class SettingWindow;
class WebAxWidget;
class PlayListWidget;

class MainWindow : public BaseWindow
{
	Q_OBJECT

public:
	MainWindow(QFrame *parent = 0);
	~MainWindow();

	TitleBar* GetTitleBar();
	Content* GetContent();
	StatusBar* GetStatusBar();
	PlayListWidget* GetPlayListWidget();

	void InsertResult(unsigned int searchId, const SEARCHRESULT &result);
protected:
	void paintEvent(QPaintEvent *);
	bool nativeEvent(const QByteArray &eventType, void *message, long *result);
	void moveEvent(QMoveEvent * event);
	void resizeEvent(QResizeEvent *event);
	void showEvent(QShowEvent *event);
	void hideEvent(QHideEvent *event);
private:
	void InitControl();
	void InitKadLibrary();
	void InitThunderLibrary();
	void LoadIniData();
	void SaveIniData();
	
	TitleBar *m_pTitleBar;
	Content *m_pContent;
	StatusBar *m_pStatusBar;

	WebAxWidget *m_pWebAxWidget;
	SystemTray *m_pSystemTray;
	SkinWindow *m_pSkinWindow;
	MiniPortalWindow *m_pMiniPortalWindow;
	SettingWindow *m_pSettingWindow;
	CloudWindow *m_pCloudWindow;
	FeedbackWindow *m_pFeedbackWindow;
	QTimer *m_pTimer;

	PlayListWidget *m_pPlayListWidget;
	QToolButton *m_pPlayListToolButton;
	QComboBox *m_pSearchComboBox;
	QPushButton *m_pSearchPushButton;
	QPushButton *m_pRandomPushButton;
	ResultTabWidget *m_pResultTabWidget;

	QLabel *m_pFilterLabel;
	QLineEdit *m_pFilterLineEdit;

	QStringList m_typeFilter;
	QString m_nameFilter;

	QStringList m_sensitiveWords;
	QStringList m_hotWords;
	QThread m_serviceThread;
private slots:
	void closePlayWindow();
	void showSkinWindow();
	void showMiniPortalWindow();
	void showSettingWindow();
	void showCloudWindow();
	void showFeedbackWindow(int type, QString content);
	void changeSkin();
	void checkUpdate();
	void showWidget();
	void showUpdate();
	void showHelp();
	void showAbout();
	void on_systemTray_activated(QSystemTrayIcon::ActivationReason reason);
	void on_typeButtonGroup_buttonClicked(QAbstractButton *button);
	void on_filterLineEdit_textChanged(const QString &text);
	void on_playListToolButton_clicked(bool checked);
	void on_searchComboBox_returnPressed();
	void on_searchPushButton_clicked();
	void on_randomPushButton_clicked();
	void on_resultTabWidget_currentChanged(int index);
	void on_searchKey_emited(QString key);
	void on_getSystemMessage_emited(QString result);
	void on_getSensitiveWords_emited(QStringList sensitiveWords);
	void on_getHotEd2ks_emited(QStringList hotEd2ks);
	void on_getHotWords_emited(QStringList hotWords);
	void on_getRandomWord_emited(QString randWord);
	void on_setSearchLog_emited(QString key);
	void on_setFeedback_emited(QString version, int type, QString content);
	void on_setPlayLog_emited(QString link);
};

extern MainWindow *g_pMainWindow;

#endif // MAINWINDOW_H
