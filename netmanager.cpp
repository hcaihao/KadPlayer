#include "stdafx.h"
#include "netmanager.h"

NetManager &NetManager::GetInstance()
{
	static NetManager instance(NULL);
	return instance;
}

NetManager::NetManager(QObject *parent)
	: m_pManager(NULL), QObject(parent)
{
	//// IE6
	//m_request.setRawHeader("User-Agent", "Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; .NET CLR 2.0.50727)");

	m_pManager = new QNetworkAccessManager(this);
	m_pManager->setCookieJar(new QNetworkCookieJar(this));
	connect(m_pManager, SIGNAL(finished(QNetworkReply *)), this, SLOT(finished(QNetworkReply *)));
	connect(m_pManager, SIGNAL(networkAccessibleChanged(QNetworkAccessManager::NetworkAccessibility)), this, SLOT(networkAccessibleChanged(QNetworkAccessManager::NetworkAccessibility)));
	connect(m_pManager, SIGNAL(sslErrors(QNetworkReply *, const QList<QSslError> &)), this, SLOT(sslErrors(QNetworkReply *, const QList<QSslError> &)));
}

NetManager::~NetManager()
{

}

quint64 NetManager::GetTimeStamp10()
{
	quint64 timeStamp = QDateTime::currentDateTime().toTime_t();
	return timeStamp;
}

quint64 NetManager::GetTimeStamp13()
{
	quint64 timeStamp = QDateTime::currentDateTime().toMSecsSinceEpoch();
	return timeStamp;
}

QList<QNetworkCookie> NetManager::GetCookies(QString url)
{
	QList<QNetworkCookie> cookies = m_pManager->cookieJar()->cookiesForUrl(QUrl(url));

	return cookies;
}

void NetManager::GetUrl(QString url, QString referer)  
{
	//m_pManager->setCookieJar(NULL);

	QUrl myUrl;
	myUrl.setUrl(url);

	QNetworkRequest request;
	request.setUrl(myUrl);
	request.setRawHeader("Accept", "application/json, text/javascript, application/x-ms-application, image/jpeg, application/xaml+xml, image/gif, image/pjpeg, application/x-ms-xbap, */*");
	request.setRawHeader("Accept-Encoding", "");	//request.setRawHeader("Accept-Encoding", "gzip,deflate,sdch");
	request.setRawHeader("Accept-Language", "zh-CN");
	request.setRawHeader("Content-Type", "application/x-www-form-urlencoded;charset=UTF-8");
	request.setRawHeader("Connection", "keep-alive");
	request.setRawHeader("Referer", referer.toLocal8Bit());
	request.setRawHeader("Host", myUrl.host().toLocal8Bit());
	request.setRawHeader("User-Agent", "Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 6.1; WOW64; Trident/6.0; SLCC2; .NET CLR 2.0.50727; .NET CLR 3.5.30729; .NET CLR 3.0.30729; Media Center PC 6.0; .NET4.0C; .NET4.0E)");

	m_pManager->get(request);
}

void NetManager::GetUrl(QNetworkRequest request)  
{
	m_pManager->get(request);
}

void NetManager::PostUrl(QString url, QByteArray data, QString referer)
{
	//m_pManager->setCookieJar(NULL);

	QUrl myUrl;
	myUrl.setUrl(url);

	QNetworkRequest request;
	request.setUrl(myUrl);
	request.setRawHeader("Accept", "application/json, text/javascript, application/x-ms-application, image/jpeg, application/xaml+xml, image/gif, image/pjpeg, application/x-ms-xbap, */*");
	request.setRawHeader("Accept-Encoding", "");	//request.setRawHeader("Accept-Encoding", "gzip,deflate,sdch");
	request.setRawHeader("Accept-Language", "zh-CN");
	request.setRawHeader("Content-Type", "application/x-www-form-urlencoded;charset=UTF-8");
	request.setRawHeader("Connection", "keep-alive");
	request.setRawHeader("Referer", referer.toLocal8Bit());
	request.setRawHeader("Host", myUrl.host().toLocal8Bit());
	request.setRawHeader("User-Agent", "Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 6.1; WOW64; Trident/6.0; SLCC2; .NET CLR 2.0.50727; .NET CLR 3.5.30729; .NET CLR 3.0.30729; Media Center PC 6.0; .NET4.0C; .NET4.0E)");
	
	m_pManager->post(request, data);
}

void NetManager::PostUrl(QNetworkRequest request, QByteArray data)
{
	m_pManager->post(request, data);
}

void NetManager::GetUrlSync(QString url, QString referer)
{
	GetUrl(url, referer);

	QEventLoop eventLoop;
	QTimer::singleShot(TIMEOUT_SECONDS, &eventLoop, SLOT(quit()));
	QObject::connect(m_pManager, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
	eventLoop.exec();	// Block until finish
}

void NetManager::GetUrlSync(QNetworkRequest request)
{
	GetUrl(request);

	QEventLoop eventLoop;
	QTimer::singleShot(TIMEOUT_SECONDS, &eventLoop, SLOT(quit()));
	QObject::connect(m_pManager, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
	eventLoop.exec();	// Block until finish
}

void NetManager::PostUrlSync(QString url, QByteArray data, QString referer)
{
	PostUrl(url, data, referer);

	QEventLoop eventLoop;
	QTimer::singleShot(TIMEOUT_SECONDS, &eventLoop, SLOT(quit()));
	QObject::connect(m_pManager, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
	eventLoop.exec();	// Block until finish
}

void NetManager::PostUrlSync(QNetworkRequest request, QByteArray data)
{
	PostUrl(request, data);

	QEventLoop eventLoop;
	QTimer::singleShot(TIMEOUT_SECONDS, &eventLoop, SLOT(quit()));
	QObject::connect(m_pManager, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
	eventLoop.exec();	// Block until finish
}

QUrl NetManager::GetUrl()
{
	return m_url;
}

QByteArray NetManager::GetContent()
{
	return m_content;
}

void NetManager::finished(QNetworkReply *reply)
{
	QNetworkReply::NetworkError error = reply->error();
	if(error == QNetworkReply::NoError)
	{
		int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
		if(statusCode == 302 || statusCode == 301)
		{
			QString location = reply->rawHeader("Location");

			if(location.startsWith("/"))
			{
				QUrl url;
				url.setScheme(reply->url().scheme());
				url.setHost(reply->url().host());
				url.setPath(location);
				location = url.toString();
			}

			GetUrl(location);
		}

		if(statusCode == 200)
		{
			//QString setCookie = reply->rawHeader("Set-Cookie");

			m_url = reply->url();
			m_content = reply->readAll();

			//QFile logFile("C:\\log.html");
			//if (logFile.open(QIODevice::Append | QIODevice::Text))
			//{
			//	logFile.write(m_url.toString().toLocal8Bit());
			//	logFile.write("\n\n");
			//	logFile.write(m_content);
			//	logFile.write("\n\n\n\n--------------------------------------------------------------------------------\n\n\n\n");

			//	logFile.close();
			//}

			emit loadFinish(m_url, m_content);
		}
	}
	else
	{
		m_url.clear();
		m_content.clear();
	}

	reply->deleteLater();
}

void NetManager::networkAccessibleChanged(QNetworkAccessManager::NetworkAccessibility accessible)
{
	//throw new exception("networkAccessibleChanged");
}

void NetManager::sslErrors(QNetworkReply * reply, const QList<QSslError> & errors)
{
	//throw new exception("sslErrors");
}
