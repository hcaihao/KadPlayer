#ifndef STATUSBAR_H
#define STATUSBAR_H

#include <QFrame>

class StatusBar : public QFrame
{
	Q_OBJECT

public:
	StatusBar(QFrame * parent = 0);
	~StatusBar();

	void SetHotWords(QStringList hotWords);
private:
	void InitControl();
	QStringList m_hotWords;

	QLabel *m_pCopyrightLabel;
	QLabel *m_pServerImageLabel;
	QLabel *m_pServerStatusLabel;

	QTimer *m_pStatusTimer;
	QTimer *m_pWordTimer;
private slots:
	void on_getStatus_timeout();
	void on_getWord_timeout();
	void on_copyrightLabel_linkActivated(const QString & link);
signals:
	void searchKey(QString key);
};

extern StatusBar *g_pStatusBar;

#endif // STATUSBAR_H
