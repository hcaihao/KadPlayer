#ifndef SERVICEMANAGER_H
#define SERVICEMANAGER_H

#include <QObject>

class ServiceManager : public QObject
{
	Q_OBJECT

public:
	ServiceManager(QObject *parent = 0);
	~ServiceManager();

signals:
	void getSystemMessage(QString message);
	void getSensitiveWords(QStringList sensitiveWords);
	void getHotEd2ks(QStringList hotEd2ks);
	void getHotWords(QStringList hotWords);
	void getRandomWord(QString randWord);
	void setSearchLog(QString key);
	void setFeedback(QString version, int type, QString content);
	void setPlayLog(QString link);
public slots:
	void GetSystemMessage(QString version);
	void GetSensitiveWords();
	void GetHotEd2ks();
	void GetHotWords();
	void GetRandomWord();
	void SetSearchLog(QString key);
	void SetFeedback(QString version, int type, QString content);
	void SetPlayLog(QString link);
};

#endif // SERVICEMANAGER_H
