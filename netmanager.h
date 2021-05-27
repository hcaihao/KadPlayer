#ifndef NETMANAGER_H
#define NETMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkCookieJar>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkProxy>
#include <QDateTime>
#include <QEventLoop>
#include <QUrl>
#include <QTimer>
#include <QNetworkCookie>
#include <QUrlQuery>
#include <QByteArray>

class NetManager : public QObject
{
	Q_OBJECT

public:
	static NetManager& GetInstance();
	~NetManager();

	quint64 GetTimeStamp10();
	quint64 GetTimeStamp13();

	QList<QNetworkCookie> GetCookies(QString url);
	void GetUrl(QString url, QString referer = "");
	void GetUrl(QNetworkRequest request);
	void PostUrl(QString url, QByteArray data, QString referer = "");
	void PostUrl(QNetworkRequest request, QByteArray data);

	void GetUrlSync(QString url, QString referer = "");
	void GetUrlSync(QNetworkRequest request);
	void PostUrlSync(QString url, QByteArray data, QString referer = "");
	void PostUrlSync(QNetworkRequest request, QByteArray data);

	QUrl GetUrl();
	QByteArray GetContent();
protected:
	QNetworkAccessManager *m_pManager;

	QUrl m_url;
	QByteArray m_content;
private:
	NetManager(QObject *parent = 0);
signals:
	void loadFinish(QUrl &url, QByteArray &content);
	void loadError(QNetworkReply::NetworkError error);
protected slots:
	void finished(QNetworkReply *reply);
	void networkAccessibleChanged(QNetworkAccessManager::NetworkAccessibility accessible);
	void sslErrors(QNetworkReply * reply, const QList<QSslError> & errors);
};

#endif // NETMANAGER_H