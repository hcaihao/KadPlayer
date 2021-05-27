#ifndef KADEMLIA_H
#define KADEMLIA_H

#include <QObject>
#include <QTime>

#include "common.h"
#include "kadresult.h"

class KadSearch : public QObject
{
	Q_OBJECT

public:
	KadSearch(QString key, int seconds, QObject *parent = 0);
	~KadSearch();

private:
	QString m_key;
	int m_seconds;
	BYTE m_seed[16];

	QTimer *m_pTimer;
	QElapsedTimer m_elapsedTimer;
signals:
	void insertResult(const KadResult &result);
	void insertResultList(const QList<KadResult>& resultList);
public slots:
	void on_getResult_timeout();
};

#endif // KADEMLIA_H
