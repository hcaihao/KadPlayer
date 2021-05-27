#ifndef SYSYTEMTRAY_H
#define SYSYTEMTRAY_H

#include <QSystemTrayIcon>

class SystemTray : public QSystemTrayIcon
{
	Q_OBJECT

public:

	SystemTray(QWidget *parent = 0);
	~SystemTray();
protected:
	bool eventFilter(QObject *obj, QEvent *event);
private:
	void InitControl();

	QMenu *m_pTrayMenu;
	QAction *m_pOpenAction;
	QAction *m_pClosePlayWindowAction;
	QAction *m_pCloseAction;

	QWidget *m_pTopWidget;
	QWidgetAction *m_pTopWidgetAction;
	QLabel *m_pTitleLabel;
    QPushButton *m_pEliteModePushButton;
    QPushButton *m_pFullModePushButton;

	QWidget *m_pBottomWidget;
	QWidgetAction *m_pBottomWidgetAction;
signals:
	void closePlayWindow();
	void showCloudWindow();
	void showWidget();
	void showUpdate();
	void showHelp();
	void showAbout();
private slots:
	void on_trayMenu_activated(QSystemTrayIcon::ActivationReason reason);
};

#endif //SYSYTEMTRAY_H
